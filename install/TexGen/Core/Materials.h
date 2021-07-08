/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2017 Louise Brown

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

namespace TexGen
{ 
	using namespace std;

	class CTextile;

	/// Abstract base class to represent a material definition
		class CLASS_DECLSPEC CMaterial
		{
		public:
			virtual CMaterial* Copy() const = 0;
			virtual string GetAbaqusCommands(string Type = "" ) = 0;
			virtual vector<double>& GetConstants() = 0;
			virtual string GetThermAbaqusCommands(string Type = "") { return ""; }
		};

		/// Represents a UMAT material definition
		class CLASS_DECLSPEC CUMAT : public CMaterial
		{
		public:
			CUMAT() {}
			CUMAT(const vector<double> &Constants){ m_Constants = Constants; }
			CMaterial* Copy() const { return new CUMAT(*this); }
			string GetAbaqusCommands(string Type = "");
			string GetThermAbaqusCommands(string Type = "");
			void SetConstants(vector<double> Constants) { m_Constants = Constants; }
			vector<double>& GetConstants() { return m_Constants; }

		protected:
			vector<double> m_Constants;
		};

		class CLASS_DECLSPEC CTextileMaterials
		{
		public:
			CTextileMaterials() {};
			~CTextileMaterials() {};

			/// Set up a material for each unique set of material constants
			void SetupMaterials( CTextile& Textile );
			/// Assign the material to a given yarn
			void AssignMaterial(string Material, int iYarn);
			/// Assign the material to a number of yarns
			void AssignMaterial(string Material, const vector<int> &Yarns);
			/// Compare two sets of material constants. Return true if identical
			bool CompareMaterialConstants( const vector<double> &MatConstants, const vector<double> &ThermConstants, const pair< vector<double>, vector<double> > &Constants );
			/// Add a material with associated name, this should then be assigned to yarns individually
			void AddMaterial(string Name, const pair<vector<double>, vector<double> > &Constants);
			/// Check if material constants have been assigned (ie != 0). Return false if all 0.0
			bool CheckYarnConstants( pair< vector<double>, vector<double> > &Constants );

			/// Output materials and assign to yarn element sets
			void OutputMaterials( ostream &Output, int iNumYarns, bool bMatrixOnly );

			map<string, pair< CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > > GetMaterials() { return m_Materials; }
			map<int, string> GetMaterialAssignements() { return m_MaterialAssignements; }
		protected:
			/// Output materials and assign to yarn element sets
			//void OutputMaterials(ostream &Output, int iNumYarns, bool bMatrixOnly );
			map<int, string> m_MaterialAssignements;
			map<string, pair< CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > > m_Materials;
		};

};  // namespace TexGen