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
#include "Misc.h"
#include "../units/units.h"

namespace TexGen
{
	using namespace std;

	COLOR GetIndexedColor(int iIndex)
	{
		static double ColorArray[][3] =
		{
			{	1,	0,	0	},
			{	0,	1,	0	},
			{	0,	0,	1	},
			{	1,	1,	0	},
			{	0,	1,	1	},
			{	1,	0,	1	},
			{	1,	0,	0.5	},
			{	0,	0.5,	1	},
			{	1,	0.5,	0	},
			{	0.5,	1,	0	},
		};
		double* Color = ColorArray[iIndex%(sizeof(ColorArray)/(3*sizeof(double)))];
		return COLOR(Color[0], Color[1], Color[2]);
	}

	void CopyToRange(vector<XYZ> &Offsets, XYZ Vector, int iLowerLimit, int iUpperLimit)
	{
		vector<XYZ> OriginalOffsets = Offsets;
		vector<XYZ>::iterator itOffset;
		Offsets.clear();
		int i;
		for (i=iLowerLimit; i<=iUpperLimit; ++i)
		{
			for (itOffset=OriginalOffsets.begin(); itOffset != OriginalOffsets.end(); ++itOffset)
			{
				Offsets.push_back(*itOffset+i*Vector);
			}
		}
	}

	// Warning possible memory leaks contained within the units library!!!
	double ConvertUnits(double dValue, std::string SourceUnits, std::string TargetUnits)
	{
		units_clear_exception();
		double dResult = dValue*units_convert(const_cast<char*>(SourceUnits.c_str()), const_cast<char*>(TargetUnits.c_str()));
		char* szError = units_check_exception();
		if (szError)
			TGERROR("Error converting units: " << szError);
		return dResult;
	}

	bool CompatibleUnits(std::string SourceUnits, std::string TargetUnits, std::string *pErrorMessage)
	{
		units_clear_exception();
		units_convert(const_cast<char*>(SourceUnits.c_str()), const_cast<char*>(TargetUnits.c_str()));
		char* szError = units_check_exception();
		if (szError)
		{
			if (pErrorMessage)
				*pErrorMessage = szError;
			return false;
		}
		else
			return true;
	}

	std::string ReduceUnits(std::string Units)
	{
		units_clear_exception();
		std::string Result = units_reduce(const_cast<char*>(Units.c_str()));
		char* szError = units_check_exception();
		if (szError)
			TGERROR("Error reducing units: " << szError);
		return Result;
	}

	void AddNewUnits(std::string NewUnit, std::string BaseUnits)
	{
		units_clear_exception();
		units_new(const_cast<char*>(NewUnit.c_str()), const_cast<char*>(BaseUnits.c_str()));
		char* szError = units_check_exception();
		if (szError)
			TGERROR("Error adding new units: " << szError);
	}

	std::string StripPath(std::string Filename)
	{
		int iPos = Filename.find_last_of("\\/");
		if (iPos == std::string::npos)
			return Filename;
		else
			return Filename.substr(iPos+1);
	}

	void AddExtensionIfMissing(std::string &Filename, std::string Extension)
	{
		if (Filename.size()<Extension.size() || Filename.rfind(Extension) != Filename.size()-Extension.size())
			Filename += Extension;
	}

	std::string RemoveExtension( std::string &Filename, std::string Extension)
	{
		int iPos = Filename.rfind(Extension);
		if (iPos == std::string::npos)
			return Filename;
		return Filename.substr(0, iPos);
	}

	std::string ReplaceFilenameSpaces(std::string Filename)
	{
		int iPos = Filename.find_last_of("\\/");
		int iEnd = Filename.size();
		std::string str;
		if (iPos == std::string::npos)
			str = Filename;
		else
			str = Filename.substr(iPos+1);

		size_t found;

		found=str.find_first_of(" ");
		while (found!=string::npos)
		{
			str[found]='_';
			found=str.find_first_of(" ",found+1);
		}

		if (iPos == std::string::npos)
			return str;
		else
		{
			return Filename.replace( iPos+1, iEnd, str );
		}
	}

	void WriteOrientationsHeader( std::ostream &Output )
	{
		Output << "********************" << std::endl;
		Output << "*** ORIENTATIONS ***" << std::endl;
		Output << "********************" << std::endl;
		Output << "** Orientation vectors" << std::endl;
		Output << "** 1st vector represents the fibre direction" << std::endl;
		Output << "** 2nd vector is an arbitrary vector perpendicular to the first" << std::endl;
	}

	void WriteElementsHeader( std::ostream &Output )
	{
		Output << "********************" << std::endl;
		Output << "*** ELEMENT DATA ***" << std::endl;
		Output << "********************" << std::endl;
		Output << "** Element data stored as a depvars " << std::endl;
		Output << "** 1 - Yarn Index (-1 for matrix, first yarn starting at 0) " << std::endl;
		Output << "** 2/3 - Location (x and y cross-section coordinates of element relative to yarn centerline) " << std::endl;
		Output << "** 4 - Volume fraction " << std::endl;
		Output << "** 5 - Distance of element from the surface of the yarn (for yarn elements only, distance is negative) " << std::endl;
	}
}









