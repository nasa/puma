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

#include "PrecompiledHeaders.h"
#include "Materials.h"
#include "TexGen.h"

#define DEFAULT_E		1e+9
#define DEFAULT_v		0.1
#define DEFAULT_G		1e+9
#define DEFAULT_A		3e-6

using namespace TexGen;

void CTextileMaterials::SetupMaterials( CTextile &Textile )
{
	int iNumYarns = Textile.GetNumYarns();
	int i;
	int j = 0;

	map<string, pair<CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > >::iterator itMaterial;

	m_Materials.clear();

	for( i = -1; i < iNumYarns; ++i )
	{
		pair< vector<double>,vector<double> > YarnConstants;
		if ( i == -1 )
		{
			YarnConstants.first.push_back( Textile.GetMatrixYoungsModulus("Pa") );
			YarnConstants.first.push_back( Textile.GetMatrixPoissonsRatio() );
			YarnConstants.second.push_back( Textile.GetMatrixAlpha() );
		}
		else
		{
			CYarn* Yarn = Textile.GetYarn(i);
			YarnConstants.first.push_back( Yarn->GetYoungsModulusX("Pa") );
			YarnConstants.first.push_back( Yarn->GetYoungsModulusY("Pa") );
			YarnConstants.first.push_back( Yarn->GetYoungsModulusZ("Pa") );
			YarnConstants.first.push_back( Yarn->GetPoissonsRatioX() );
			YarnConstants.first.push_back( Yarn->GetPoissonsRatioY() );
			YarnConstants.first.push_back( Yarn->GetPoissonsRatioZ() );
			YarnConstants.first.push_back( Yarn->GetShearModulusXY("Pa") );
			YarnConstants.first.push_back( Yarn->GetShearModulusXZ("Pa") );
			YarnConstants.first.push_back( Yarn->GetShearModulusYZ("Pa") );
			YarnConstants.second.push_back( Yarn->GetAlphaX("/K") );
			YarnConstants.second.push_back( Yarn->GetAlphaY("/K") );
			YarnConstants.second.push_back( Yarn->GetAlphaZ("/K") );
		}

		if ( CheckYarnConstants( YarnConstants ) )
		{
			if ( i == -1 )
			{
				TGERROR("One or more material constant not set for matrix, assigning defaults" );
			}
			else
			{
				TGERROR("One or more material constant not set for yarn " << i << ", assigning defaults" );
			}
		}	
		
		for (itMaterial = m_Materials.begin(); itMaterial != m_Materials.end(); ++itMaterial)
		{
			// Find corresponding material if already created and assign to yarn
			pair< CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > Material = itMaterial->second;
			vector<double> &MatConstants = Material.first->GetConstants();
			vector<double> &ThermConstants = Material.second->GetConstants();

			if ( MatConstants.size() == YarnConstants.first.size() && ThermConstants.size() == YarnConstants.second.size() )
			{
				if ( CompareMaterialConstants( MatConstants, ThermConstants, YarnConstants ) )
				{
					AssignMaterial( itMaterial->first, i );
					break;
				}
			}
		}
		if ( itMaterial == m_Materials.end() )  // Yarn properties not from existing material so create new one & assign to yarn
		{
			string Name = "Mat" + stringify(j);
			AddMaterial( Name, YarnConstants );
			AssignMaterial( Name, i );
			++j;
		}	
	}
	
	
}

void CTextileMaterials::AssignMaterial(string Material, int iYarn)
{
	m_MaterialAssignements[iYarn] = Material;
}

void CTextileMaterials::AssignMaterial(string Material, const vector<int> &Yarns)
{
	vector<int>::const_iterator itYarn;
	for (itYarn = Yarns.begin(); itYarn != Yarns.end(); ++itYarn)
	{
		m_MaterialAssignements[*itYarn] = Material;
	}
}

bool CTextileMaterials::CompareMaterialConstants( const vector<double> &MatConstants, const vector<double> &ThermConstants, const pair< vector<double>, vector<double> > &Constants )
{
	vector<double>::const_iterator itMatConstants, itThermConstants, itConstants;

	for( itMatConstants = MatConstants.begin(), itConstants = Constants.first.begin(); itMatConstants != MatConstants.end(); ++itMatConstants, ++itConstants )
	{
		if ( *itMatConstants != *itConstants )
			return false;
	}
	for( itThermConstants = ThermConstants.begin(), itConstants = Constants.second.begin(); itThermConstants != ThermConstants.end(); ++itThermConstants, ++itConstants )
	{
		if ( *itThermConstants != *itConstants )
			return false;
	}
	return true;
}

void CTextileMaterials::AddMaterial(string Name, const pair< vector<double>, vector<double> > &Constants)
{
	pair< CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > materials;
	materials.first = CUMAT(Constants.first);
	materials.second = CUMAT(Constants.second);
	//m_Materials[Name] = CUMAT(Constants);
	m_Materials[Name] = materials;
}

bool CTextileMaterials::CheckYarnConstants( pair< vector<double>, vector<double> > &Constants )
{
	vector<double>::iterator itConstants;
	bool bSetDefault = false;
	
	int i;
	int iNumConstants = Constants.first.size();

	for( itConstants = Constants.first.begin(), i = 0; itConstants != Constants.first.end(); ++itConstants, ++i )
	{
		
		if ( *itConstants == 0.0 )
		{
			if ( iNumConstants == 2 )
			{
				*itConstants = i == 0 ? DEFAULT_E : DEFAULT_v;
			}
			else
			{
				if ( i < 3 )
					*itConstants = DEFAULT_E;
				else if ( i > 5 )
					*itConstants = DEFAULT_G;
				else
					*itConstants = DEFAULT_v;
			}
			bSetDefault = true;
		}
	}
	for( itConstants = Constants.second.begin(), i = 0; itConstants != Constants.second.end(); ++itConstants, ++i )
	{
		if ( *itConstants == 0.0 )
		{
			*itConstants = DEFAULT_A;
			bSetDefault = true;
		}
	}
	return bSetDefault;
}

void CTextileMaterials::OutputMaterials(ostream &Output, int iNumYarns, bool bMatrixOnly )
{
	Output << "*****************" << endl;
	Output << "*** MATERIALS ***" << endl;
	Output << "*****************" << endl;
	
	map<string, pair<CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > >::iterator itMaterial;
	for (itMaterial = m_Materials.begin(); itMaterial != m_Materials.end(); ++itMaterial)
	{
		Output << "*Material, Name=" << itMaterial->first << endl;
		if ( itMaterial->second.first->GetConstants().size() == 2 )
			Output << itMaterial->second.first->GetAbaqusCommands();	
		else
			Output << itMaterial->second.first->GetAbaqusCommands( "ENGINEERING CONSTANTS" );

		if ( itMaterial->second.second->GetConstants().size() == 1 )
			Output << itMaterial->second.second->GetThermAbaqusCommands("");	
		else
			Output << itMaterial->second.second->GetThermAbaqusCommands( "ORTHO" );
	}
	int i;
	string MatName;
	for (i = -1; i < iNumYarns; ++i)
	{
		if (m_MaterialAssignements.count(i))
			MatName = m_MaterialAssignements[i];
		else
			MatName = m_Materials.begin()->first;
		if ( i == -1 )
		{
			Output << "*Solid Section, ElSet=Matrix, Material=" << MatName << endl;
			Output << "1.0," << endl;
		}
		else if (!bMatrixOnly)
		{
			Output << "*Solid Section, ElSet=Yarn" << i << ", Material=" << MatName << ", Orientation=TexGenOrientations" << endl;
			Output << "1.0," << endl;
		}
	}	
}

string CUMAT::GetAbaqusCommands( string Type )
{
	ostringstream Output;
	if ( Type == "" )
		Output << "*Elastic" << endl;
	else
		Output << "*Elastic, type=" << Type << endl;
	
	WriteValues(Output, m_Constants, 8);
	return Output.str();
}

string CUMAT::GetThermAbaqusCommands( string Type )
{
	ostringstream Output;
	if ( Type == "" )
		Output << "*Expansion" << endl;
	else
		Output << "*Expansion, type=" << Type << endl;
	WriteValues(Output, m_Constants, 8);
	return Output.str();
}