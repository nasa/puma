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

from TexGen.Core import *
from io import StringIO

def ExportAnsys(Filename, TextileName):
	""" Export a textile as an Ansys Parametric Design Language script. """
	# Open file for writing
	File = open(Filename, 'w')
	
	Textile = GetTextile(TextileName)
	Domain = Textile.GetDomain()
	
	File.write('/PREP7\n')

	Size = Textile.GetApproximateSize()
	File.write('/VIEW,,0,%f,%f\n' % (-Size, Size/2))
	
	PointCommands = StringIO()
	LineCommands = StringIO()
	TopSurfaceCommands = StringIO()
	BottomSurfaceCommands = StringIO()
	EndSurfaceCommands = StringIO()
	VolumeCommands = StringIO()
	RepeatCommands = StringIO()
	DeleteCommands = StringIO()
	DomainCommands = StringIO()
	BooleanCommands = StringIO()
	
	PointCount = 1
	LineCount = 1
	YarnCount = 1
	NumVolumes = 0
	NumYarns = len(Textile.GetYarns())
	for Yarn in Textile.GetYarns():
		NumSections = len(Yarn.GetSlaveNodes(Yarn.SURFACE))
		TopSurfaceCommands.write('FLST,2,%d,4\n' % NumSections)
		BottomSurfaceCommands.write('FLST,2,%d,4\n' % NumSections)
		EndSurfaceCommands.write('FLST,2,2,4\nFITEM,2,%d\nFITEM,2,%d\nAL,P51X\n' %
								(LineCount, LineCount+1))

		for Nodes in Yarn.GetSlaveNodes(Yarn.SURFACE):
			NumSectionPoints = len(Nodes.GetSectionPoints())
			TopPoints = list(Nodes.GetSectionPoints()[:NumSectionPoints/2])
			BottomPoints = list(Nodes.GetSectionPoints()[NumSectionPoints/2:])

			LineCommands.write('FLST,3,%d,3\n' % (len(TopPoints)+1))
			for Point in TopPoints:
				PointCommands.write('K,%d,%f,%f,%f\n' % (PointCount, Point.x, Point.y, Point.z))
				LineCommands.write('FITEM,3,%d\n' % PointCount)
				PointCount+=1
			LineCommands.write('FITEM,3,%d\n' % PointCount)
			LineCommands.write('BSPLIN, ,P51X\n')
			TopSurfaceCommands.write('FITEM,2,%d\n' % LineCount)
			LineCount+=1

			LineCommands.write('FLST,3,%d,3\n' % (len(BottomPoints)+1))
			for Point in BottomPoints:
				PointCommands.write('K,%d,%f,%f,%f\n' % (PointCount, Point.x, Point.y, Point.z))
				LineCommands.write('FITEM,3,%d\n' % PointCount)
				PointCount+=1
			LineCommands.write('FITEM,3,%d\n' % (PointCount-NumSectionPoints))
			LineCommands.write('BSPLIN, ,P51X\n')
			BottomSurfaceCommands.write('FITEM,2,%d\n' % LineCount)
			LineCount+=1

		TopSurfaceCommands.write('ASKIN,P51X\n')
		BottomSurfaceCommands.write('ASKIN,P51X\n')
		EndSurfaceCommands.write('FLST,2,2,4\nFITEM,2,%d\nFITEM,2,%d\nAL,P51X\n' % (LineCount-2, LineCount-1))

		VolumeCommands.write('FLST,2,4,5\nFITEM,2,%d\nFITEM,2,%d\nFITEM,2,%d\nFITEM,2,%d\nVA,P51X\n' % 
							(YarnCount, YarnCount+NumYarns, YarnCount*2-1+2*NumYarns, YarnCount*2+2*NumYarns))
		
		Translations = Domain.GetTranslations(Yarn)
		NumVolumes += len(Translations)
		for Translation in Translations:
			RepeatCommands.write('FLST,3,1,6\nFITEM,3,%d\nVGEN,2,P51X, , ,%f,%f,%f, ,0\n' % (YarnCount, Translation.x, Translation.y, Translation.z))
			
		DeleteCommands.write('VDELE,%d\n' % YarnCount)

		YarnCount += 1
	
	Min, Max = Domain.GetMesh().GetAABB()
	
	DomainCommands.write('BLOCK,%f,%f,%f,%f,%f,%f\n' % (Min.x, Max.x, Min.y, Max.y, Min.z, Max.z))

	BooleanCommands.write('FLST,3,%d,6\n' % (NumVolumes))
	for i in range(YarnCount, NumVolumes+YarnCount):
		BooleanCommands.write('FITEM,3,%d\n' % (i))
	BooleanCommands.write('VSBV,1,P51X\n')

	File.write(PointCommands.getvalue())
	File.write(LineCommands.getvalue())
	File.write(TopSurfaceCommands.getvalue())
	File.write(BottomSurfaceCommands.getvalue())
	File.write(EndSurfaceCommands.getvalue())
	File.write(VolumeCommands.getvalue())
	File.write(RepeatCommands.getvalue())
	File.write(DeleteCommands.getvalue())
	File.write(DomainCommands.getvalue())
	File.write(BooleanCommands.getvalue())

	File.write('FINISH\n')

if __name__ == '__main__':
	weave = CTextileWeave2D(2, 2, 1, 0.2, True)
	weave.SetGapSize(0.01)
	weave.SwapPosition(0, 0)
	weave.SwapPosition(1, 1)
	# weave.SetResolution(6)
	weave.AssignDefaultDomain()
	name = AddTextile(weave)
	# ReadFromXML('LeuvenOrthogonal.tg3')
	# name = GetTextiles().keys()[0]
	ExportAnsys('texgen.mac', name)























