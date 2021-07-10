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
	/// Template used as a base class for creating singletons
	/**
	Declare a singleton class as
	class CMyClass : public CSingleton<CMyClass>
	To declare the single instance this line should be put at the top of the .cpp file
	template<> CMyClass CSingleton<CMyClass>::m_SingleInstance = CMyClass();
	*/
	template<typename T>
	class CLASS_DECLSPEC CSingleton
	{
	public:
		CSingleton()
		{
			// There should only be one instance created, only use static GetInstance() function
			// instead of instantiating manually
			assert(this == &CSingleton::GetInstance());
		}
		static T &GetInstance()
		{
			return m_SingleInstance;
		}
	protected:
		static T m_SingleInstance;
	};

};	// namespace TexGen
