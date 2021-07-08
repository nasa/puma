/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#pragma once

#include "TextileDeformer.h"
#include "Mesh.h"

namespace TexGen
{
	using namespace std;

	class CTextile;

	/// Deform the geometry of a textile given a volume mesh
	/**
	The idea is that a volume mesh of a textile can be created and used to run
	a mechanical analysis using a finite element analysis package. The displacements
	of the nodes calculated by the FE package are read back in with this class. And
	using these displacements, the textile geometry is deformed.
	*/
	class CLASS_DECLSPEC CTextileDeformerVolumeMesh : public CTextileDeformer
	{
	public:
		CTextileDeformerVolumeMesh(void);
		virtual ~CTextileDeformerVolumeMesh(void);

		bool SetYarnMeshDisplacements(int iYarn, const CMesh &Mesh, vector<XYZ> &Displacements);

	protected:
		struct YARN_MESH
		{
			CMesh Mesh;
			vector<XYZ> NodeDisplacements;
		};

		virtual double GetDisplacement(XYZ Pos, int iYarn, XYZ &Disp) const;
		void GetBarycentricCoordinates(const XYZ &P, const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P4, double &a, double &b, double &c, double &d) const;

		vector<YARN_MESH> m_YarnMeshes;
	};


};	// namespace TexGen














