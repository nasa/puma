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

#pragma once


namespace TexGen
{ 
	using namespace std;

	/// Class used to calculate a pattern draft from a weave pattern
	/// Also used to set up 3D model from a weave pattern
	class CLASS_DECLSPEC CPatternDraft 
	{
	public:
		CPatternDraft();
		virtual ~CPatternDraft(void);

		/// Add a row representing one weft insertion '1' indicates warp up, '0' warp down and '2' no yarn
		void AddRow( string Row );
		void ClearWeavePattern();
		int GetNumWarps();
		int GetNumWefts();
		int GetNumHeddles() { return m_NumHeddles; }
		/// Output the complete pattern draft in a text file
		void Output( string Filename );
		/// Calculates the pattern draft based on the weave pattern
		bool CreatePatternDraft();
		const vector<string>& GetWeavePattern() {return m_WeavePattern;}
		const vector<int>& GetHeddleDraft() { return m_HeddleDraft; }
		const vector<bool>& GetChainDraft() { return m_ChainDraft; }
		
	protected:
		/// Creates vector of strings where each string represents one unique column of the weave pattern
		void CreateColumnsVector();
		/// Create heddle draft. Stored as vector of int
		bool CreateHeddleDraft();
		/// Chain draft calculated from weave pattern and heddle draft.  Stored as vector of bools
		/// Width = number of heddles
		/// Height = number of wefts
		bool CreateChainDraft();

		vector<string> m_WeavePattern;
		vector<string> m_Columns;
		vector<string> m_UniqueColumns;
		vector<int>    m_HeddleDraft;
		vector<bool> m_ChainDraft;
		int  m_NumHeddles;
	};

};	// namespace TexGen
