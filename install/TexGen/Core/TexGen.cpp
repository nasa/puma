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

#include "PrecompiledHeaders.h"
#include "TexGen.h"

using namespace TexGen;

// Create the single instance on program start
template<> CLASS_DECLSPEC CTexGen CSingleton<CTexGen>::m_SingleInstance = CTexGen();

CTexGen::CTexGen(void)
: m_pLogger(NULL)
, m_pTextileCallback(NULL)
, m_iMajorVersion(3)
, m_iMinorVersion(12)
, m_iRevision(0)
, m_bMessagesOn(true)
{
	SetLogger(CLoggerScreen());
	AddNewUnits("tex", "g/km");
	AddNewUnits("denier", "g / 9 km");
}

CTexGen::~CTexGen(void)
{
	DeleteTextiles();
	if (m_pLogger)
		delete m_pLogger;
}

string CTexGen::GetName(const CTextile* pTextile) const
{
	map<string, CTextile*>::const_iterator itTextile;
	for (itTextile = m_Textiles.begin(); itTextile != m_Textiles.end(); ++itTextile)
	{
		if (itTextile->second == pTextile)
			return itTextile->first;
	}
	return "";
}

CTextile* CTexGen::GetTextile(string TextileName)
{
	if (TextileName.empty())
	{
		if (m_Textiles.empty())
		{
			TGERROR("Unable to get textile, no textiles have been created");
		}
		else
		{
			return m_Textiles.begin()->second;
		}
	}
	else
	{
		map<string, CTextile*>::iterator itTextile;
		itTextile = m_Textiles.find(TextileName);
		if (itTextile != m_Textiles.end())
			return itTextile->second;
		TGERROR("Unable to get textile, textile does not exist: " + TextileName);
	}
	return NULL;
}

bool CTexGen::AddTextile(string TextileName, const CTextile &Textile, bool bOverwrite)
{
	// Check that the name given is not blank
	if (TextileName.empty())
	{
		TGERROR("Unable to add textile, given textile name is empty");
		return false;
	}
	// Check that the entry does not exist
	if (m_Textiles.find(TextileName) != m_Textiles.end())
	{
		if (!bOverwrite)
		{
			TGERROR("Unable to add textile, textile with that name already exists: " + TextileName);
			return false;
		}
		else
		{
			delete m_Textiles[TextileName];
			m_Textiles[TextileName] = Textile.Copy();
			TGLOG("Replaced textile \"" << TextileName << "\"");
			if ( m_pTextileCallback)
				m_pTextileCallback(TextileName, true);
			return true;
		}
	}
	// Add the section to the list using the name as the key
	m_Textiles[TextileName] = Textile.Copy();
	TGLOG("Added textile \"" << TextileName << "\"");
	if (m_pTextileCallback)
		m_pTextileCallback(TextileName, true);
	return true;
}

string CTexGen::AddTextile(const CTextile &Textile)
{
	string DefaultName = Textile.GetDefaultName();
	int iNumber = 1;
	// Check that the entry does not exist, and the name given is not blank
	while (m_Textiles.find(DefaultName) != m_Textiles.end())
	{
		DefaultName = Textile.GetDefaultName() + "-" + stringify(++iNumber);
	}
	// Add the section to the list using the generated name as the key
	m_Textiles[DefaultName] = Textile.Copy();
	TGLOG("Added textile \"" << DefaultName << "\"");
	if (m_pTextileCallback)
		m_pTextileCallback(DefaultName, true);
	return DefaultName;
}

bool CTexGen::DeleteTextile(string TextileName)
{
	map<string, CTextile*>::iterator itTextile;
	itTextile = m_Textiles.find(TextileName);
	if (itTextile != m_Textiles.end())
	{
		delete itTextile->second;
		m_Textiles.erase(itTextile);
		TGLOG("Deleted textile \"" << TextileName << "\"");
		if (m_pTextileCallback)
			m_pTextileCallback(TextileName, false);
		return true;
	}
	TGERROR("Unable to delete textile, textile does not exist: " + TextileName);
	return false;
}

void CTexGen::PopulateTiXmlElement(TiXmlElement &Element, string TextileName, OUTPUT_TYPE OutputType)
{
	if (TextileName.empty())
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = m_Textiles.begin(); itTextile != m_Textiles.end(); ++itTextile)
		{
			TiXmlElement Textile("Textile");
			Textile.SetAttribute("name", itTextile->first);
			itTextile->second->PopulateTiXmlElement(Textile, OutputType);
			Element.InsertEndChild(Textile);
		}
	}
	else
	{
		CTextile *pTextile = GetTextile(TextileName);
		if (pTextile)
		{
			TiXmlElement Textile("Textile");
			Textile.SetAttribute("name", TextileName);
			pTextile->PopulateTiXmlElement(Textile, OutputType);
			Element.InsertEndChild(Textile);
		}
	}
}

bool CTexGen::LoadTiXmlElement(TiXmlElement &Element)
{
	bool bOverwrite = false;
	FOR_EACH_TIXMLELEMENT(pTextile, Element, "Textile")
	{
		string Name = pTextile->Attribute("name");
		const string* pType = pTextile->Attribute(string("type"));
		if (pType)
		{
			if (*pType == "CTextileWeave2D")
				AddTextile(Name, CTextileWeave2D(*pTextile), bOverwrite);
			else if (*pType == "CTextileWeave3D")
				AddTextile(Name, CTextileWeave3D(*pTextile), bOverwrite);
			else if (*pType == "CTextileWeave") 
				AddTextile(Name, CTextileWeave(*pTextile), bOverwrite);
			else if (*pType == "CTextileOrthogonal")
				AddTextile(Name, CTextileOrthogonal(*pTextile), bOverwrite);
			else if (*pType == "CTextileAngleInterlock")
				AddTextile(Name, CTextileAngleInterlock(*pTextile), bOverwrite);
			else if (*pType == "CTextileOffsetAngleInterlock")
				AddTextile(Name, CTextileOffsetAngleInterlock(*pTextile), bOverwrite);
			else if (*pType == "CTextileLayerToLayer")
				AddTextile(Name, CTextileLayerToLayer(*pTextile), bOverwrite);
			else if (*pType == "CTextile3DWeave")
				AddTextile(Name, CTextile3DWeave(*pTextile), bOverwrite);
			else if (*pType == "CShearedTextileWeave2D")
				AddTextile(Name, CShearedTextileWeave2D(*pTextile), bOverwrite);
			else if (*pType == "CTextileLayered")
				AddTextile(Name, CTextileLayered(*pTextile), bOverwrite);
			else
				AddTextile(Name, CTextile(*pTextile), bOverwrite);
		}
	}
	return true;
}

void CTexGen::SaveToXML(string FileName, string TextileName, OUTPUT_TYPE OutputType)
{
	TiXmlDocument doc(FileName);
	TiXmlDeclaration Declaration("1.0", "", "");
	doc.InsertEndChild(Declaration);
	TiXmlElement Root("TexGenModel");
	Root.SetAttribute("version", GetVersion());
	PopulateTiXmlElement(Root, TextileName, OutputType);
	doc.InsertEndChild(Root);
	if (doc.SaveFile())
	{
		TGLOG("XML file saved to \"" << FileName << "\"");
	}
	else
	{
		TGERROR("Error saving XML file to \"" << FileName << "\"");
	}
}

bool CTexGen::ReadFromXML(string FileName)
{
	TGLOGINDENT("Loading XML file: \"" << FileName << "\"");
	TiXmlDocument doc(FileName);
	if (doc.LoadFile())
	{
		TiXmlElement* pRoot = doc.RootElement();
		if (pRoot)
		{
			string Version = pRoot->Attribute("version");
			if (Version != GetVersion())
			{
				TGERROR("Warning: File was created with version " << Version << ", current version is " << GetVersion());
			}
			if (LoadTiXmlElement(*pRoot))
			{
				TGLOG("XML file loaded from \"" << FileName << "\"");
				return true;
			}
		}
	}

	TGERROR("Error loading XML file from \"" << FileName << "\"");

	return false;
}

void CTexGen::DeleteTextiles()
{
	while (!m_Textiles.empty())
	{
		DeleteTextile(m_Textiles.begin()->first);
	}
}

void CTexGen::SetLogger(const CLogger &Logger)
{
	if (m_pLogger)
		delete m_pLogger;
	m_pLogger = Logger.Copy();
}

void CTexGen::SetTextileCallback(void (*pTextileCallback)(string TextileName, bool bAdded))
{
	m_pTextileCallback = pTextileCallback;
}

string CTexGen::GetVersion() const
{
	return stringify(m_iMajorVersion) + "." + stringify(m_iMinorVersion) + "." + stringify(m_iRevision);
}

void CTexGen::SetMessages( bool bMessagesOn, const CLogger &Logger )
{
	m_bMessagesOn = bMessagesOn;
	SetLogger( Logger );
}

void CTexGen::SetMessages( bool bMessagesOn )
{
	m_bMessagesOn = bMessagesOn;
	if ( m_bMessagesOn )
		SetLogger( CLoggerScreen() );
	else
		SetLogger( CLoggerNull() );
}

void CTexGen::GetTextileNames( vector<string> &Names )
{
	map<string, CTextile*>::iterator itTextile;
	Names.clear();

	for ( itTextile = m_Textiles.begin(); itTextile != m_Textiles.end(); ++itTextile )
	{
		Names.push_back( itTextile->first );
	}
}




