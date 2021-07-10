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
#include "TextileWeave.h"

namespace TexGen
{ 
	using namespace std;

	class CPatternDraft;

	/// Respresents a 2d woven textile
	class CLASS_DECLSPEC CTextileWeave2D : public CTextileWeave
	{
	public:
		/// Build a 2d weave unit cell of given width, height, yarn spacing and fabric thickness
		CTextileWeave2D(int iWidth, int iHeight, double dSpacing, double dThickness, bool bRefine = true, bool bInPlaneTangents = true);
		CTextileWeave2D(TiXmlElement &Element);
		~CTextileWeave2D(void);

		CTextile* Copy() const { return new CTextileWeave2D(*this); }
		string GetType() const { return "CTextileWeave2D"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		string GetDefaultName() const;

		/// Swap the cross over order of yarns at given index
		void SwapPosition(int x, int y);
		/// Swap the cross over order of all yarns
		void SwapAll();

		// Accessor methods
		bool GetRefine() const { return m_bRefine; }
		bool GetInPlaneTangents() const { return m_bInPlaneTangents; }

		/// Refine the textile to eliminate interference. 
		/**
		 * May be called subsequent to initial generation of textile 
		 * allowing weave to be modified and then refined
		 * Must get CTextile::GetWeave() to use as member of CTextileWeave not CTextile
		 */
		void RefineTextile( bool bCorrectWidths = true, bool bCorrectInterference = true, bool bPeriodic = true );
		

		virtual CDomainPlanes GetDefaultDomain( bool bSheared = false, bool bAddedDomainHeight = true );

		void ConvertToPatternDraft( /*CPatternDraft& PatternDraft*/ );

		/// Function to set interpolation so that in-plane tangents are forced at master nodes
		/**
		\param bInPlaneTangents Set to true to force master node tangents to be in x-y plane
		*/
		void SetInPlaneTangents( bool bInPlaneTangents = true ) const;

	protected:
		bool BuildTextile() const;
		virtual void Refine( bool bCorrectWidths = true, bool bCorrectInterference = true, bool bPeriodic = true ) const;
		void AdjustTangents() const;
		virtual bool AdjustSectionsForRotation( bool bPeriodic = true ) const;
		
		void SetInPlaneTangents( int Yarn, bool bInPlaneTangents = true ) const;

		bool m_bRefine;
		bool m_bInPlaneTangents;
	};
};	// namespace TexGen

