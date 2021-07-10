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
namespace TexGen
{
	/** \file Misc.h
	This file contains miscellaneous global functions that do not belong anywhere else
	*/
	#ifndef CLASS_DECLSPEC
		#ifdef WIN32
			#pragma warning(disable:4251)
			#ifdef EXPORT
				#define CLASS_DECLSPEC __declspec(dllexport)
			#else
				#define CLASS_DECLSPEC __declspec(dllimport)
			#endif
		#else
			#define CLASS_DECLSPEC
		#endif
	#endif

	/// Macro to enable looping over tinyxml easier
	/**
	\param CHILDELEMENT Is the name of the TiXmlElement pointer that will be created used as an iterator
	\param PARENTELEMENT The parent TiXmlElement to loop over
	\param ELEMENTNAME The name of the element type to loop over given as an array of characters
	*/
	#define FOR_EACH_TIXMLELEMENT(CHILDELEMENT, PARENTELEMENT, ELEMENTNAME) \
	for (TiXmlElement* CHILDELEMENT = (PARENTELEMENT).FirstChildElement(ELEMENTNAME); CHILDELEMENT; CHILDELEMENT = CHILDELEMENT->NextSiblingElement(ELEMENTNAME))

	/// Function to convert a value (e.g. int, double, etc...) to a string
	template<typename T>
	inline std::string stringify(const T& x, int iPrecision = 12, bool bScientific = true)
	{
		std::ostringstream o;
		if (bScientific)
			o << std::scientific;
		o << std::setprecision(iPrecision) << x;
		return o.str();
	}

	/// Function to convert a string to a value (e.g. int, double, etc...)
	template<typename T>
	inline T valueify(const std::string& x)
	{
		T value;
		memset((void*)&value, 0, sizeof(T));
		std::stringstream i(x);
		i >> value;
		return value;
	}

	/// Function to convert a string to a value (e.g. int, double, etc...)
	template<typename T>
	inline T valueify(const char* x)
	{
		T value;
		memset((void*)&value, 0, sizeof(T));
		if (x)
		{
			std::stringstream i(x);
			i >> value;
		}
		return value;
	}

	/// Used to sort double-int pairs
	struct LessPairDoubleInt : public std::binary_function<std::pair<double, int>, std::pair<double, int>, bool>
	{
		bool operator()(std::pair<double, int> x, std::pair<double, int> y) { return x.first < y.first; }
	};

	/// Used to sort double-XYZ pairs
	struct LessPairDoubleXYZ : public std::binary_function<std::pair<double, XYZ>, std::pair<double, XYZ>, bool>
	{
		bool operator()(std::pair<double, XYZ> x, std::pair<double, XYZ> y) { return x.first < y.first; }
	};

	struct COLOR
	{
		double r, g, b;
		COLOR(double r, double g, double b):r(r), g(g), b(b) {}
		COLOR():r(1), g(1), b(1) {}
		inline double* Array() { return (double*)this; }
	};

	enum OUTPUT_TYPE
	{
		OUTPUT_MINIMAL,
		OUTPUT_STANDARD,
		OUTPUT_FULL,
	};

	enum EXPORT_TYPE
	{
		INP_EXPORT,
		VTU_EXPORT,
		SCIRUN_EXPORT,
	};

	enum DOMAIN_TYPE
	{
		BOX_DOMAIN,
		SHEARED_DOMAIN,
		ROTATED_DOMAIN,
	};

	CLASS_DECLSPEC COLOR GetIndexedColor(int iIndex);
	CLASS_DECLSPEC void CopyToRange(std::vector<XYZ> &Offsets, XYZ Vector, int iLowerLimit, int iUpperLimit);

	// Wrapper functions around the units library
	CLASS_DECLSPEC double ConvertUnits(double dValue, std::string SourceUnits, std::string TargetUnits);
	CLASS_DECLSPEC bool CompatibleUnits(std::string SourceUnits, std::string TargetUnits, std::string *pErrorMessage = NULL);
	CLASS_DECLSPEC std::string ReduceUnits(std::string Units);
	CLASS_DECLSPEC void AddNewUnits(std::string NewUnit, std::string BaseUnits);

	// Misc functions
	/// Strip the path from the filename (e.g. "c:\folder\file.ext -> file.ext")
	CLASS_DECLSPEC std::string StripPath(std::string Filename);
	/// Adds an extension to the filename if it is missing otherwise do nothing (e.g. picture -> picture.png)
	CLASS_DECLSPEC void AddExtensionIfMissing(std::string &Filename, std::string Extension);
	/// Replaces spaces in filename with underscore
	CLASS_DECLSPEC std::string ReplaceFilenameSpaces(std::string Filename);
	/// Strip the extension from the filename
	CLASS_DECLSPEC std::string RemoveExtension( std::string &Filename, std::string Extension );
	/// Write orientations header for ABAQUS .ori files
	CLASS_DECLSPEC void WriteOrientationsHeader( std::ostream &Output );
	/// Write elements header for ABAQUS .eld files
	CLASS_DECLSPEC void WriteElementsHeader( std::ostream &Output );

	/// Get an interpolated value
	template<typename T>
	T GetInterpedValue(const std::vector<std::pair<double, T> > &InterpValues, double fraction)
	{
		if (InterpValues.empty())
			return T();
		if (fraction <= InterpValues.front().first)
			return InterpValues.front().second;
		if (fraction >= InterpValues.back().first)
			return InterpValues.back().second;
		typename std::vector<std::pair<double, T> >::const_iterator itCurrentValue;
		typename std::vector<std::pair<double, T> >::const_iterator itPrevValue = InterpValues.end();
		for (itCurrentValue = InterpValues.begin(); itCurrentValue != InterpValues.end(); ++itCurrentValue)
		{
			if (itPrevValue != InterpValues.end())
			{
				if (fraction <= itCurrentValue->first)
				{
					double u = (fraction-itPrevValue->first)/(itCurrentValue->first-itPrevValue->first);
					T value = itPrevValue->second + u*(itCurrentValue->second-itPrevValue->second);
					return value;
				}
			}
			itPrevValue = itCurrentValue;
		}
		return T();   // Should never get here
	}

	/// Get the two closest values to fraction in the vector InterpValues
	template<typename T>
	std::pair<double, std::pair<T, T> > GetClosestValues(const std::vector<std::pair<double, T> > &InterpValues, double fraction)
	{
		std::pair<double, std::pair<T, T> > ClosestVals;
		if (InterpValues.empty())
			return ClosestVals;
		if (fraction <= InterpValues.front().first)
		{
			ClosestVals.first = 0.0;
			ClosestVals.second = std::make_pair(InterpValues.front().second, InterpValues.front().second);
			return ClosestVals;
		}
		if (fraction >= InterpValues.back().first)
		{
			ClosestVals.first = 1.0;
			ClosestVals.second = std::make_pair(InterpValues.back().second, InterpValues.back().second);
			return ClosestVals;
		}
		typename std::vector<std::pair<double, T> >::const_iterator itCurrentValue;
		typename std::vector<std::pair<double, T> >::const_iterator itPrevValue = InterpValues.end();
		for (itCurrentValue = InterpValues.begin(); itCurrentValue != InterpValues.end(); ++itCurrentValue)
		{
			if (itPrevValue != InterpValues.end())
			{
				if (fraction <= itCurrentValue->first)
				{
					ClosestVals.first = (fraction-itPrevValue->first)/(itCurrentValue->first-itPrevValue->first);
					ClosestVals.second.first = itPrevValue->second;
					ClosestVals.second.second = itCurrentValue->second;
					return ClosestVals;
				}
			}
			itPrevValue = itCurrentValue;
		}
		return ClosestVals;   // Should never get here
	}

	/// Structure used to retreive information about a particular point in space
	struct POINT_INFO
	{
		int iYarnIndex;		///< Index of the yarn, -1 when the point is not inside a yarn
		XYZ YarnTangent;	///< Tangent of the yarn centreline
		XY Location;		///< Location of the point relative to the yarn centerline
		double dVolumeFraction; ///< Local volume fraction, value determined by FibreDistribution and yarn properties
		/// Returns the closest distance from the point to the surface of the yarn.
		/**
		Negative values represent a point inside the yarn. Positive values represent a point lying outside the yarn.
		*/
		double dSurfaceDistance; 
		XYZ Orientation;   ///< Local fibre orientation
		XYZ Up;			   ///< Local Up vector
		POINT_INFO() : iYarnIndex(-1), dVolumeFraction(0), dSurfaceDistance(0) {}
	};

	struct MESHER_ELEMENT_DATA : public POINT_INFO
	{
		int iLayer;
		int iRegion;
/*		int iYarnIndex;
		XYZ YarnTangent;
		XY Location;
		double dVolumeFraction;
		double dSurfaceDistance;*/
		MESHER_ELEMENT_DATA():iLayer(-1),iRegion(-1)/*,iYarnIndex(-1),dVolumeFraction(0),dSurfaceDistance(0)*/{}
	};

	inline void GetMinMaxXY(const std::vector<XY> &Points, XY &Min, XY &Max )
	{
		Min = Points[0];
		Max = Points[0];
		for ( int i = 1; i < (int)Points.size(); ++i )
		{
			if ( Points[i].x < Min.x )
				Min.x = Points[i].x;
			else if ( Points[i].x > Max.x )
				Max.x = Points[i].x;

			if ( Points[i].y < Min.y )
				Min.y = Points[i].y;
			else if ( Points[i].y > Max.y )
				Max.y = Points[i].y;
		}
	}

	enum PERIODIC_BOUNDARY_CONDITIONS
	{
		MATERIAL_CONTINUUM,
		SINGLE_LAYER_RVE,
		STAGGERED_BC,
		SHEARED_BC,
		ROTATED_BC,
		NO_BOUNDARY_CONDITIONS
	};

	template <typename T>
	void WriteValues(std::ostream &Output, T &Values, int iMaxPerLine)
	{
		int iLinePos = 0;
		typename T::const_iterator itValue;
		for (itValue = Values.begin(); itValue != Values.end(); ++itValue)
		{
			if (iLinePos == 0)
			{
				// Do nothing...
			}
			else if (iLinePos < iMaxPerLine)
			{
				Output << ", ";
			}
			else
			{
				Output << "\n";
				iLinePos = 0;
			}
			Output << *itValue;
			++iLinePos;
		}
		Output << "\n";
	}

};	// namespace TexGen

