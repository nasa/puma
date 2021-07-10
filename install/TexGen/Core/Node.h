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
class TiXmlElement;
namespace TexGen
{
	using namespace std;

	/// Represents a point on the centreline of a yarn
	class CLASS_DECLSPEC CNode
	{
	public:
		/// Create a node
		/**
		\param Position Position of the node
		\param Tangent Tangent of the node
		\param Up Up vector of the node
		*/
		CNode(XYZ Position = XYZ(), XYZ Tangent = XYZ(), XYZ Up = XYZ(0, 0, 1), double Angle = 0.0);
		CNode(TiXmlElement &Element);
		virtual ~CNode(void);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Rotate the Node by given quaternion
		virtual void Rotate(WXYZ Rotation, XYZ Origin = XYZ(0,0,0));

		/// Translate the Node by given vector
		virtual void Translate(XYZ Vector);

		/// Project the Up vector such that it is perpendicular to the tangent
		void ProjectUp();

		/// Get the side vector
		//XYZ GetSide() const { return CrossProduct(m_Tangent, m_Up); }
		XYZ GetSide() const;
		//XYZ RotateSide( XYZ Side );
		// Various accessor methods
		XYZ GetPosition() const { return m_Position; }
		XYZ GetTangent() const { return m_Tangent; }
		XYZ GetUp() const { return m_Up; }
		double GetAngle() const { return m_Angle; }
		XYZ GetNormal() const;
		void SetPosition(XYZ Position) { m_Position = Position; }
		void SetTangent(XYZ Tangent) { m_Tangent = Tangent; }
		void SetUp(XYZ Up) { m_Up = Up; }
		void SetAngle( double Angle ) { m_Angle = Angle; }

	protected:
		/// Coordinates representing the position of the node
		XYZ m_Position;
		/// Unit vector tangent to the yarn path at this node
		XYZ m_Tangent;
		/// Unit vector which represents the up direction (must be perpendicular to m_Tangent)
		XYZ m_Up;
		double m_Angle;
	};
};	// namespace TexGen



