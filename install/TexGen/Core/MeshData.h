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
	using namespace std;

	class CLASS_DECLSPEC CMeshDataBase
	{
	public:
		enum DATA_TYPE
		{
			NODE,
			ELEMENT,
		};
		CMeshDataBase(string Name, DATA_TYPE Type) : m_Name(Name), m_DataType(Type) {}
		virtual ~CMeshDataBase() {}

		virtual void InsertVTKData(TiXmlElement &Parent) const = 0;

		string m_Name;
		DATA_TYPE m_DataType;
	};

	template <typename T>
	class CLASS_DECLSPEC CMeshData : public CMeshDataBase
	{
	public:
		CMeshData(string Name, DATA_TYPE Type) : CMeshDataBase(Name, Type) {}

		void InsertVTKData(TiXmlElement &Parent) const
		{
			ostringstream DataStream;
			typename vector<T>::const_iterator itData;
			for (itData = m_Data.begin(); itData != m_Data.end(); ++itData)
			{
				WriteVTKDataElement(DataStream, *itData) << " ";
			}
/*			for(unsigned int i=0; i<m_Data.size(); ++i)
			{
				WriteVTKDataElement(DataStream, m_Data[i]) << " ";
			}*/
			TiXmlElement Element("DataArray");
			{
				Element.SetAttribute("type", GetVTKDataType());
				Element.SetAttribute("NumberOfComponents", GetNumberOfComponents());
				Element.SetAttribute("format", "ascii");
				Element.SetAttribute("Name", m_Name.c_str());
				Element.InsertEndChild(TiXmlText(DataStream.str()));
			}
			Parent.InsertEndChild(Element);
		}
		static string GetVTKDataType()
		{
			// Data type unknown?
			assert(false);
			return "";
		}
		static int GetNumberOfComponents()
		{
			return 1;
		}
		ostream &WriteVTKDataElement(ostream &output, T Element) const
		{
			output << Element;
			return output;
		}

		vector<T> m_Data;
	};

	// Need some specialisations on the char to make sure it gets output as an integer rather than a character
	template <>
	inline ostream &CMeshData<char>::WriteVTKDataElement(ostream &output, char Element) const
	{
		output << (int)Element;
		return output;
	}
	template <>
	inline ostream &CMeshData<unsigned char>::WriteVTKDataElement(ostream &output, unsigned char Element) const
	{
		output << (int)Element;
		return output;
	}

	#define DEFINE_INT_DATA_TYPE(TYPE) template <> inline string CMeshData<TYPE>::GetVTKDataType() {return "Int" + stringify(sizeof(TYPE)*8);}
	#define DEFINE_UINT_DATA_TYPE(TYPE) template <> inline string CMeshData<TYPE>::GetVTKDataType() {return "UInt" + stringify(sizeof(TYPE)*8);}

	DEFINE_INT_DATA_TYPE(char)
	DEFINE_INT_DATA_TYPE(short)
	DEFINE_INT_DATA_TYPE(int)
	DEFINE_INT_DATA_TYPE(long)
	DEFINE_UINT_DATA_TYPE(unsigned char)
	DEFINE_UINT_DATA_TYPE(unsigned short)
	DEFINE_UINT_DATA_TYPE(unsigned int)
	DEFINE_UINT_DATA_TYPE(unsigned long)

	#undef DEFINE_INT_DATA_TYPE
	#undef DEFINE_UINT_DATA_TYPE

	template <> string inline CMeshData<float>::GetVTKDataType() {return "Float" + stringify(sizeof(float)*8);}
	template <> string inline CMeshData<double>::GetVTKDataType() {return "Float" + stringify(sizeof(double)*8);}

	////////////////////////////////////////
	// DEFINE THE USER-DEFINED DATA TYPES //
	////////////////////////////////////////

	/// XYZ
	template <>
	inline string CMeshData<XYZ>::GetVTKDataType()
	{
		return CMeshData<double>::GetVTKDataType();
	}
	template <>
	inline int CMeshData<XYZ>::GetNumberOfComponents()
	{
		return 3;
	}
	template <>
	inline ostream &CMeshData<XYZ>::WriteVTKDataElement(ostream &output, XYZ Element) const
	{
		output << Element.x << " " << Element.y << " " << Element.z;
		return output;
	}

	/// XY
	template <>
	inline string CMeshData<XY>::GetVTKDataType()
	{
		return CMeshData<double>::GetVTKDataType();
	}
	template <>
	inline int CMeshData<XY>::GetNumberOfComponents()
	{
		return 2;
	}
	template <>
	inline ostream &CMeshData<XY>::WriteVTKDataElement(ostream &output, XY Element) const
	{
		output << Element.x << " " << Element.y;
		return output;
	}

};	// namespace TexGen













