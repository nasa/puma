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
	/// Object container to help handle memory management issues
	/**
	This class acts as a pointer to an object with a little more functionality.
	It is automatically initialised to NULL and whenever the object needs to be
	copied it creates a copy of the object it contains. The difference with a
	normal pointer is that a normal pointer will not create a copy of the object
	it points to. It simply creates a copy of the address, which can lead to
	problems when the object is deleted. The copied pointers will no longer be
	valid. This problem is avoided by creating a copy of the object it contains.
	For this to work the object	must have a function called "Copy" that creates
	a copy of itself and returns a pointer to the new copy.

	Note: This is NOT a reference counting system.
	*/
	template <typename T>
	class CObjectContainer
	{
	public:
		CObjectContainer():m_pObject(NULL){}
		CObjectContainer(const T &Object):m_pObject(NULL)
		{
			*this = Object;
		}
		CObjectContainer(const CObjectContainer<T> &CopyMe):m_pObject(NULL)
		{
			*this = CopyMe;
		}
		~CObjectContainer()
		{
			if(m_pObject) delete m_pObject;
		}
		CObjectContainer &operator = (const CObjectContainer<T> &CopyMe)
		{
			if (m_pObject) delete m_pObject;
			if (CopyMe.m_pObject)
				m_pObject = dynamic_cast<T*>(CopyMe.m_pObject->Copy());
			else
				m_pObject = NULL;
			return *this;
		}
		CObjectContainer &operator = (const T &Object)
		{
			if (m_pObject) delete m_pObject;
			m_pObject = dynamic_cast<T*>(Object.Copy());
			return *this;
		}
		// Object should act as if it was a pointer of type T
		/// Conversion operator
		/**
		If the container object is passed to a function that expects to receive an
		object of type T* then it will be converted automatically. Similarly if
		the container object is dereferenced an object of type T will be obtained.
		*/
		operator T*() { return m_pObject; }
		/// Pointer overload operator to act as a pointer of type T
		T* operator->() { return m_pObject; }
		/// Remove the object contained within the container class
		void Clear() { if(m_pObject) delete m_pObject; m_pObject = NULL; }
		/// Conversion operator for when the class is defined as const
		operator const T*() const { return m_pObject; }
		/// Pointer overload operator for the class when it is defined as const
		const T* operator->() const { return m_pObject;	}
	private:
		/// Object contained within the class
		T* m_pObject;
	};

	/// Used to sort double-objectref pairs
	template <typename T>
	struct LessPairDoubleObjectRef : public std::binary_function<std::pair<double, CObjectContainer<T> >, std::pair<double, CObjectContainer<T> >, bool>
	{
		bool operator()(std::pair<double, CObjectContainer<T> > x, std::pair<double, CObjectContainer<T> > y) { return x.first < y.first; }
	};



	/// Weak pointer that acts as normal pointer except when copied
	/**
	When the copy constructor is called for this class the pointer is set to NULL.
	This ensures that when copies of a class are made which contain pointers to their parent,
	the pointer will not be copied to the copied class. This is not wanted since the copied
	class will not necessarily be a child of the parent it was copied from.
	*/
	template <typename T>
	class CWeakPointer
	{
	public:
		CWeakPointer():m_pPointer(NULL){}
		// Don't copy the pointer when it is copied
		CWeakPointer(const CWeakPointer<T> &CopyMe):m_pPointer(NULL){}
		CWeakPointer &operator = (T* pPointer)
		{
			m_pPointer = pPointer;
			return *this;
		}
		// Object should act as if it was a pointer of type T
		/// Conversion operator
		/**
		If the weak pointer is passed to a function that expects to receive an
		object of type T* then it will be converted automatically. Similarly if
		the container object is dereferenced an object of type T will be obtained.
		*/
		operator T*() { return m_pPointer; }
		/// Pointer overload operator to act as a pointer of type T
		T* operator->() { return m_pPointer; }
		/// Conversion operator for when the class is defined as const
		operator const T*() const { return m_pPointer; }
		/// Pointer overload operator for the class when it is defined as const
		const T* operator->() const { return m_pPointer;	}
	private:
		/// Object contained within the class
		T* m_pPointer;
	};


};	// namespace TexGen





