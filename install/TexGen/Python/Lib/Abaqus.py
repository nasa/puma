# TexGen: Geometric textile modeller.
# Copyright (C) 2006 Martin Sherburn
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

import re
from sys import stderr
from TexGen.Core import *
import os, shutil
from tempfile import mkdtemp
from glob import glob

class TemporaryDirectoryRedirect(object):
	'''Class used to change the current working directory to a new temporary
	directory on initialisation. On destruction the working directory will
	be reverted back to its original location and the contents of the temporary
	directory will be deleted.
	'''
	def __init__(self):
		self._prevDir = os.getcwd()
		self._tempDir = mkdtemp(prefix='texgen')
		print('Changing current working directory to "%s"' % self._tempDir)
		os.chdir(self._tempDir)

	def __del__(self):
		print('Reverting current working directory back to "%s"' % self._prevDir)
		os.chdir(self._prevDir)
		print('Deleting contents of "%s"' % self._tempDir)
		#for fileName in glob(self._tempDir + r'\*'):
		#	os.remove(fileName)
		#os.rmdir(self._tempDir)
	
	@property
	def tempDir(self):
		return self._tempDir

class TextileDeformerAbaqus(CTextileDeformerVolumeMesh, CSimulationAbaqus):
	'''This class uses the finite element analysis program ABAQUS as a method of
	deforming textile geometry.
	'''
	def __init__(self):
		CTextileDeformerVolumeMesh.__init__(self)
		CSimulationAbaqus.__init__(self)
		self._fortranProgram = None
		self._simulationFilesPrefix = None

	def _ParseInpFile(self, inpFilename):
		'''Parse an ABAQUS input file to get information about the nodes
		elements and element sets:
		
		nodes are created as a dictionary, where the key is the node number
		and the value is a list of 3 float values representing the coordinates.
		The dictionary keys will generally range from 1 to the total number of
		nodes, but this is not a requirement for correct parsing of the ABAQUS
		file.
		
		elements are also created as a dictionary, where the key is the element
		number and the value is a tuple where the first value is the element
		type (on of CMesh.ELEMENT_TYPE) and the second is a list of node indices
		represented as integers.
		
		elset is a dictionary, where the key is a string of the element set name
		and the value is a list of element indices represented as integers.
		
		These three are returned as tuple (nodes, elements, elsets)
		'''
		file = open(inpFilename)
		unknownElementTypes = {}
		elemType = None
		section = None
		sectionParams = {}
		elements = {}
		nodes = {}
		elsets = {}
		for line in file:
			if line.startswith('**'):
				continue
			if line.startswith('*'):
				parts = [x.strip().lower() for x in line[1:].split(',')]
				section = parts[0]
				sectionParams = {}
				for part in parts[1:]:
					keyVal = part.split('=')
					if len(keyVal) < 2:
						sectionParams[keyVal[0]] = None
					else:
						sectionParams[keyVal[0]] = keyVal[1]
				#print section, sectionParams
				continue
			if section == "element":
				abaqusElemType = sectionParams['type']
				if abaqusElemType == "c3d4":
					elemType = CMesh.TET
				elif abaqusElemType == "c3d6":
					elemType = CMesh.WEDGE
				elif abaqusElemType == "c3d8r" or abaqusElemType == "c3d8":
					elemType = CMesh.HEX
				else:
					if abaqusElemType in unknownElementTypes:
						unknownElementTypes[sectionParams['type']] += 1
					else:
						unknownElementTypes[sectionParams['type']] = 1
					continue
				elem = [int(x) for x in line.split(',')]
				index = elem[0]
				nodeIndices = elem[1:]
				elements[index] = (elemType, nodeIndices)
			if section == "node":
				node = line.split(',')
				index = int(node[0])
				nodes[index] = [float(x) for x in node[1:]]
			if section == "elset":
				elems = []
				for x in line.split(','):
					try:
						elems.append(int(x))
					except ValueError:
						pass
				generate = 'generate' in sectionParams
				if generate:
					step = 1
					if len(elems)>2:
						step = elems[2]
					elems = list(range(elems[0], elems[1]+1, step))
				name = sectionParams['elset']
				if name in elsets:
					elsets[name] += elems
				else:
					elsets[name] = elems
		if len(unknownElementTypes) != 0:
			print('Warning: Some unknown elements were encountered while parsing', inpFilename, ':', unknownElementTypes, file=stderr)
		if len(nodes) == 0:
			print('Warning: No nodes found while parsing', inpFilename, file=stderr)
		if len(elements) == 0:
			print('Warning: No elements found while parsing', inpFilename, file=stderr)
		if len(elsets) == 0:
			print('Warning: No element sets found while parsing', inpFilename, file=stderr)
		return nodes, elements, elsets

	def _ParseDatFile(self, datFilename):
		'''Parse an ABAQUS .dat file to extract nodal displacements and
		potentially other nodal properties for the last time step.
		
		The file is searched for an occurancy of the string
		"N O D E   O U T P U T" at which point a column header is searched
		for typically looking something like this:
		
		"NODE FOOT-   U1          U2          U3"
		
		if this line contains U1, U2 and U3. This is taken as the column header.
		The following lines are checked and if they contain the same number of
		values as the number of header columns these values are parsed. The first
		value is interpreted as the node number and the following values are
		associated with the column headers with a dictionary.
		
		So finally we end up with a dictionary where the key is the node number
		and the value is a dictionary of header - value pairs. So to obtain
		the displacement U1 of the 20th node we can type:
		nodeDisplacements[20]['U1']
		
		Note: regular expressions are used here as the .dat file is a human
		readable file format rather than machine readable. It is possible that
		the file format may change in future version of ABAQUS or that output
		is displayed on a way that was not anticipated. So this function should
		not be considered as fool proof.
		'''
		nodeOutputStart = True
		columnHeaders = None
		nodeCount = 0
		file = open(datFilename)
		parseHeader = re.compile(r'[\w\.-]+(?: [\w\.-]+)*')
		parseNumbers = re.compile(r'[\w\.-]+')
		nodeDisplacements = {}

		for line in file:
		#	print line,
#			if not nodeOutputStart:
#				result = re.search('N O D E   O U T P U T', line)
#				if result:
#					nodeOutputStart = True
#			else:
				if not columnHeaders:
					result = parseHeader.findall(line)
					if ('U.U1' in result and 'U.U2' in result and 'U.U3' in result) \
						or ('U1' in result and 'U2' in result and 'U3' in result):
						columnHeaders = result
						#print columnHeaders
				else:
					result = parseNumbers.findall(line)
					if len(result) == len(columnHeaders):
						try:
							nodeNum = int(result[0])
						except ValueError:
							pass
						else:
							# nodeCount += 1
							# if nodeNum != nodeCount:
								# print 'nodeNum:', nodeNum, 'nodeCount:', nodeCount
								# assert(False)
							nodeDisplacements[nodeNum] = dict(list(zip(columnHeaders, [float(x) for x in result])))

		if len(nodeDisplacements) == 0:
			print("Warning: No nodal displacements found while parsing", datFilename, file=stderr)
		return nodeDisplacements

	def DeformTextileFromAbaqusResults(self, textile, baseName, deformDomain = True):
		'''Using data collection from an ABAQUS input deck (.inp file) and .dat file
		this function builds a TexGen mesh for each yarn where the nodes contain
		displacement data. This is then fed into the CTextileDeformerVolumeMesh
		class to deform the TexGen textile.
		'''
		inpFilename = baseName + '.inp'
		datFilename = baseName + '.dat'	
		nodes, elements, elsets = self._ParseInpFile(inpFilename)
		nodeDisplacements = self._ParseDatFile(datFilename)
		# nodeDisplacements = {}
		#print nodes
		#print nodeDisplacements
		mesh = CMesh()
		nodeMap = {}
		dispData = XYZMeshData("Displacements", CMeshDataBase.NODE)
		for nodeNum, node in list(nodes.items()):
			nodeMap[nodeNum] = mesh.AddNode(XYZ(node[0], node[1], node[2]))
			try:
				x = nodeDisplacements[nodeNum]
				d = XYZ(x['U.U1'], x['U.U2'], x['U.U3'])
			except:
				d = XYZ()
			dispData.m_Data.append(d)
		for elemNum, elem in list(elements.items()):
			mesh.AddElement(elem[0], [nodeMap[x] for x in elem[1]])

		#meshDataList = MeshDataVector()
		#meshDataList.append(dispData)
		#mesh.SaveToVTK(baseName + '-def', meshDataList)

		iYarn = 0
		while ('yarn%d' % iYarn) in elsets:
			elset = elsets['yarn%d' % iYarn]
			yarnMesh = CMesh()
			for node in mesh.GetNodes():
				yarnMesh.AddNode(node)
			for elemIndex in elset:
				element = elements[elemIndex]
				yarnMesh.AddElement(element[0], [nodeMap[x] for x in element[1]])
			# yarnMesh.SaveToVTK(baseName + '-yarn%d' % iYarn, meshDataList)
			self.SetYarnMeshDisplacements(iYarn, yarnMesh, dispData.m_Data)
			iYarn += 1

		CTextileDeformerVolumeMesh.SetRepeatVectorDeformation(self, self.GetDeformation())
		CTextileDeformerVolumeMesh.DeformTextile(self, textile, deformDomain)

		# tg3DeformedFile = baseName + '-def.tg3'
		# SaveToXML(tg3DeformedFile)

	def SetRepeatVectorDeformation(self, deformation):
		raise AttributeError("This method is protected, please use SetDeformation instead")

	def DeformTextile(self, textile, deformDomain = True):
		'''This function automates the entire process of exporting a TexGen mesh
		to ABAQUS, defining material properties, contact surfaces, boundary conditions,
		etc... Running the ABAQUS simulation and re-reading the results back into
		TexGen in order to deform the Textile.
		
		The only input required is a textile. The desired linear deformation
		should have been specified before calling this function. See base class
		CSimulationAbaqus for details on what parameters can be used.
		
		If no simulation file prefix is specified a temporary directory will be
		created to store all the abaqus simulation files. These will then be
		deleted after they are no longer needed. If a file prefix is specified
		then the files will be created in the current directory with the given
		name and will not be deleted afterwards.
		'''
		fortranProg = self._fortranProgram
		if not self._simulationFilesPrefix:
			# Get the absolute path of the fortran path before switching to the temporary directory
			if fortranProg:
				fortranProg = os.path.abspath(fortranProg)
			tempDirRedirect = TemporaryDirectoryRedirect()
			# Copy the fortran program to the temporary directory
			if fortranProg:
				shutil.copy(fortranProg, '.')
				fortranProg = os.path.basename(fortranProg)
			# Assign an arbitrary prefix
			name = 'temp'
		else:
			name = self._simulationFilesPrefix
		if not self.CreateAbaqusInputFile(textile, name + '.inp', False):
			raise RuntimeError('Unable to create ABAQUS input file')
		command = 'abaqus job="%s"' % name
		if fortranProg:
			command += ' user="%s"' % fortranProg
		command += ' ask_delete=off -interactive'
		print('Launching ABAQUS:', command)
		result = os.system(command)
		if result == 0:
			print('ABAQUS seems to have completed successfully')
			self.DeformTextileFromAbaqusResults(textile, name, deformDomain)
		else:
			raise RuntimeError('ABAQUS has failed with error code: %d' % result)

	def SetFortranProgram(self, filename):
		self._fortranProgram = filename

	def GetFortranProgram(self):
		return self._fortranProgram

	def SetSimulationFilesPrefix(self, prefix):
		self._simulationFilesPrefix = prefix

	def GetSimulationFilesPrefix(self):
		return self._simulationFilesPrefix





