/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2014 Louise Brown

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
#include "PatternDraft.h"

using namespace TexGen;

CPatternDraft::CPatternDraft()
: m_NumHeddles(0)
{
}

CPatternDraft::~CPatternDraft(void)
{
}

/*void CPatternDraft::AddRow( vector<int>& Row )
{
	m_WeavePattern.push_back( Row );
}*/

void CPatternDraft::ClearWeavePattern()
{
	m_WeavePattern.clear();
}

void CPatternDraft::AddRow( string Row )
{
	// Remove end of line characters
	Row.erase(remove(Row.begin(), Row.end(), '\n'), Row.end() );
	Row.erase(remove(Row.begin(), Row.end(), '\r'), Row.end() );
	m_WeavePattern.push_back( Row );
}

int CPatternDraft::GetNumWarps()
{
	if ( !m_WeavePattern.empty() )
		return (int)m_WeavePattern[0].size();
	return 0;
}

int CPatternDraft::GetNumWefts()
{
	if ( !m_WeavePattern.empty() )
		return (int)m_WeavePattern.size();
	return 0;
}

bool CPatternDraft::CreatePatternDraft()
{
	if ( m_WeavePattern.empty() )
	{
		TGERROR("Can't create pattern draft: no weave pattern specified");
		return false;
	}
	CreateColumnsVector();
	if ( !CreateHeddleDraft() )
	{
		TGERROR("Failed to create heddle draft");
		return false;
	}
	if ( !CreateChainDraft() )
	{
		TGERROR("Failed to create chain draft: no heddle draft specified");
		return false;
	}
	return true;
}

void CPatternDraft::CreateColumnsVector()
{
	// Create a set of unique columns in the weave pattern
	vector<string>::iterator itWeavePattern;
	int Warps = (int)m_WeavePattern[0].size();
	m_Columns.clear();
	m_Columns.resize( Warps );

	// Create string for each column pattern
	for ( itWeavePattern = m_WeavePattern.begin(); itWeavePattern != m_WeavePattern.end(); ++itWeavePattern )
	{
		for ( int i = 0; i < Warps; ++i )
		{
			if ( (*itWeavePattern)[i] == '2' )     // For purposes of this exercise no yarn equates to warp down
				m_Columns[i].push_back( '0' );
			else
				m_Columns[i].push_back( (*itWeavePattern)[i] );  // if 0 or 1 leave as it is
		}
	}

	// Only save one copy of each column pattern in UniqueColumns vector
	vector<string>::iterator itColumns;
	m_UniqueColumns.clear();
	for ( itColumns = m_Columns.begin(); itColumns != m_Columns.end(); ++itColumns )
	{
		vector<string>::iterator itFind;
		itFind = find( m_UniqueColumns.begin(),m_UniqueColumns.end(), *itColumns );
		if ( itFind == m_UniqueColumns.end() )
			m_UniqueColumns.push_back( *itColumns );
	}
	
	m_NumHeddles = (int)m_UniqueColumns.size();
}

bool CPatternDraft::CreateHeddleDraft()
{
	if ( m_Columns.empty() || m_UniqueColumns.empty() )
		return false;
	
	int NumWarps = (int)m_Columns.size();
	vector<string>::iterator itColumns;

	m_HeddleDraft.clear();

	/// Heddle draft entry corresponds to the index into the unique columns which matches the column in the weave pattern
	for ( itColumns = m_Columns.begin(); itColumns != m_Columns.end(); ++itColumns )
	{
		vector<string>::iterator itFind;
		itFind = find(m_UniqueColumns.begin(), m_UniqueColumns.end(), *itColumns );
		m_HeddleDraft.push_back( m_NumHeddles - 1 - (int)distance( m_UniqueColumns.begin(), itFind ) );  //Heddle index is max at top
	}
	return true;
}

bool CPatternDraft::CreateChainDraft()
{
	if ( m_HeddleDraft.empty() )
		return false;
	vector<string>::iterator itWeavePattern;
	int NumWarps = (int)m_Columns.size();

	m_ChainDraft.resize( m_NumHeddles * GetNumWefts() );
	m_ChainDraft.assign( m_ChainDraft.size(), 0 );

	int j;

	for ( itWeavePattern = m_WeavePattern.begin(), j=0; itWeavePattern != m_WeavePattern.end(); ++itWeavePattern, ++j )
	{
		for ( int i = 0; i < NumWarps; ++i )
		{
			if ( (*itWeavePattern)[i] == '1' )  // Weft
				m_ChainDraft[ m_NumHeddles*j + m_HeddleDraft[i] ] = 1;  // Set the chain draft entry on the current row, corresponding to the heddle draft entry
		}
	}
	return true;
}

void CPatternDraft::Output( string Filename )
{
	AddExtensionIfMissing(Filename, ".txt");
	ofstream Output(Filename.c_str());

	vector<string>  HeddleDraft;
	HeddleDraft.clear();
	HeddleDraft.resize( m_NumHeddles );
	// Create output strings for heddle draft. Each string represents one row of draft
	for ( int i = 0; i < m_HeddleDraft.size(); ++i )
	{
		for ( int j = 0; j < m_NumHeddles; ++j )
		{
			if ( m_HeddleDraft[i] == j )
				HeddleDraft[j].push_back('1');
			else
				HeddleDraft[j].push_back('0');
		}
	}

	// Create tie-up - assumed to be straight tie-up
	// Sits next to heddle draft in output so added to heddle draft strings
	for ( int i = 0; i < m_NumHeddles; ++i )
	{
		HeddleDraft[i].push_back('\t');
		for ( int j = 0; j < m_NumHeddles; ++j )
		{
			if ( i == j )
				HeddleDraft[i].push_back('1');
			else
				HeddleDraft[i].push_back('0');
		}
	}

	vector<string>::reverse_iterator itHeddleDraft;
	for ( itHeddleDraft = HeddleDraft.rbegin(); itHeddleDraft != HeddleDraft.rend(); ++itHeddleDraft )
	{
		Output << itHeddleDraft->c_str() << "\n";
	}
	Output << "\n\n";

	// Output for weave pattern and chain draft stored in ChainDraft vector
	vector< string >::iterator itWeavePattern;
	vector<string> ChainDraft;
	// First duplicate weave pattern..
	for ( itWeavePattern = m_WeavePattern.begin(); itWeavePattern != m_WeavePattern.end(); ++itWeavePattern )
	{
		ChainDraft.push_back(*itWeavePattern);
	}

	// ..then add chain draft to its right
	vector<bool>::iterator itChainDraft;
	int j;
	for ( itChainDraft = m_ChainDraft.begin(), j = 0; itChainDraft != m_ChainDraft.end(); ++j )
	{
		string ChainRow;
		for ( int i = 0; i < m_NumHeddles; ++i )
		{
			ChainRow.push_back( *itChainDraft ? '1':'0' );
			itChainDraft++;
		}

		ChainDraft[j].push_back('\t');
		ChainDraft[j].append(ChainRow);
		Output << ChainDraft[j].c_str() << "\n";
	}
}
