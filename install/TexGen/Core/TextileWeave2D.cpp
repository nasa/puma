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
#include "TextileWeave2D.h"
#include "SectionEllipse.h"
#include "SectionRotated.h"
#include "PatternDraft.h"

using namespace TexGen;

CTextileWeave2D::CTextileWeave2D(int iWidth, int iHeight, double dSpacing, double dThickness, bool bRefine, bool bInPlaneTangents )
: CTextileWeave(iWidth, iHeight, dSpacing, dThickness)
, m_bRefine(bRefine)
, m_bInPlaneTangents(bInPlaneTangents)
{
	vector<bool> Cell;
	Cell.push_back(PATTERN_YYARN);
	Cell.push_back(PATTERN_XYARN);
	int i, j;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (j=0; j<m_iNumXYarns; ++j)
		{
			GetCell(i, j) = Cell;
		}
	}
}

CTextileWeave2D::~CTextileWeave2D(void)
{
}

CTextileWeave2D::CTextileWeave2D(TiXmlElement &Element)
: CTextileWeave(Element)
{
	m_bRefine = valueify<bool>(Element.Attribute("Refine"));
	m_bInPlaneTangents = valueify<bool>(Element.Attribute("InPlaneTangents"));
}

void CTextileWeave2D::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileWeave::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Refine", stringify(m_bRefine));
	Element.SetAttribute("InPlaneTangents", stringify(m_bInPlaneTangents));
}

void CTextileWeave2D::SwapPosition(int x, int y)
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to swap positions, index out of range: " << x << ", " << y);
		return;
	}
	vector<bool> &Cell = GetCell(x, y);
	assert(Cell.size() == 2);
	Cell[0] = !Cell[0];
	Cell[1] = !Cell[1];
	m_bNeedsBuilding = true;
}

void CTextileWeave2D::SwapAll()
{
	int i, j;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (j=0; j<m_iNumXYarns; ++j)
		{
			SwapPosition(i, j);
		}
	}
	m_bNeedsBuilding = true;
}

bool CTextileWeave2D::AdjustSectionsForRotation( bool bPeriodic ) const
{
	int i, j;

	CYarn *pYarn;
	XYZ PrevXPos, NextXPos;
	XYZ PrevYPos, NextYPos;
	XYZ Up;
	CNode NewNode;

	// Assign more adequate cross sections
	double dWidth, dHeight;
	int iPrevYarnx, iPrevYarny;
	int iNextYarnx, iNextYarny;
//	int iNextCrossx, iNextCrossy;

	double dAngle;
	int iNumYarns, iYarnLength;
	int iDirection;
	int x, y;

	// First loop for Y yarns, second loop for X yarns
	for (iDirection=0; iDirection<2; ++iDirection)
	{
		if (iDirection==0)
		{
			iNumYarns = m_iNumYYarns;
			iYarnLength = m_iNumXYarns;
		}
		else
		{
			iNumYarns = m_iNumXYarns;
			iYarnLength = m_iNumYYarns;
		}
		int start = 0;
		if ( !bPeriodic )
		{
			start = 1;
		}
		
		for ( i = start; i < iNumYarns; ++i)
		{
			if (iDirection==0)
			{
				iPrevYarnx = i-1;
				if (iPrevYarnx < 0)
					iPrevYarnx += iNumYarns;
				if ( bPeriodic )
					iNextYarnx = (i+1)%iNumYarns;
				else
					iNextYarnx = i+1;  //  Assumes that one extra node than number of yarns

				// The angle is the maximum rotation angle to apply to the yarn at points where it needs
				// rotating, specified in radians.
				dAngle = atan2(0.5*m_dFabricThickness, m_YYarnData[iPrevYarnx].dSpacing+m_YYarnData[i].dSpacing);
				// Get the yarn width and height for this X yarn
				dWidth = m_YYarnData[i].dWidth;
				dHeight = m_YYarnData[i].dHeight;
				// Get a pointer to the current yarn
				pYarn = &m_Yarns[m_YYarns[i][0]];
				// Used for getting the cell coordinates
				x = i;
			}
			else
			{
				iPrevYarny = i-1;
				if (iPrevYarny < 0)
					iPrevYarny += iNumYarns;
				if ( bPeriodic )
					iNextYarny = (i+1)%iNumYarns;  //  Assumes that one extra node than number of yarns
				else 
					iNextYarny = i+1;

				// The angle is the maximum rotation angle to apply to the yarn at points where it needs
				// rotating, specified in radians.
				dAngle = atan2(0.5*m_dFabricThickness, m_XYarnData[iPrevYarny].dSpacing+m_XYarnData[i].dSpacing);
				// Get the yarn width and height for this X yarn
				dWidth = m_XYarnData[i].dWidth;
				dHeight = m_XYarnData[i].dHeight;
				// Get a pointer to the current yarn
				pYarn = &m_Yarns[m_XYarns[i][0]];
				// Used for getting the cell coordinates
				y = i;
			}

			CSectionEllipse DefaultEllipseSection(dWidth, dHeight);

			// Get a copy of the yarn sections that is applied to the nodes
			if (pYarn->GetYarnSection()->GetType() != "CYarnSectionInterpNode")
				return false;
			CYarnSectionInterpNode* pYarnSection = (CYarnSectionInterpNode*)pYarn->GetYarnSection()->Copy();
//			CYarnSectionInterpNode YarnSection(true, true);
			int iRot;		// Should have 1 of three values (-1, 0 or 1). -1 (rotation to the right), 0 (no rotation), 1 (rotation to the left)
			for (j=0; j<iYarnLength; ++j)
			{
				if (iDirection==0)
				{
					// Set the parameters which will be used by GetCell for traversing an X yarn
					y = iPrevYarny = iNextYarny = j;
				}
				else
				{
					// Set the parameters which will be used by GetCell for traversing a Y yarn
					x = iPrevYarnx = iNextYarnx = j;
				}
				// If the yarns on either side are the same then no rotation should occur
				if (GetCell(iPrevYarnx, iPrevYarny) == GetCell(iNextYarnx, iNextYarny))
					iRot = 0;
				else if (GetCell(iPrevYarnx, iPrevYarny)[0] == PATTERN_YYARN)
					iRot = -1;	// Rotate to the right
				else
					iRot = 1;	// Rotate to the left

				CSectionEllipse* EllipseSection = NULL;
				if ( pYarnSection->GetNodeSection(j).GetType() == "CSectionEllipse" )
					EllipseSection = (CSectionEllipse*)pYarnSection->GetNodeSection(j).Copy();
				else
					EllipseSection = (CSectionEllipse*)DefaultEllipseSection.Copy();
				// Assign section based on the rotation it should have
				switch (iRot)
				{
				case 0:
//					pYarnSection->ReplaceSection(j, EllipseSection);
					break;
				case -1:
					pYarnSection->ReplaceSection(j, CSectionRotated(*EllipseSection, -dAngle));
					break;
				case 1:
					pYarnSection->ReplaceSection(j, CSectionRotated(*EllipseSection, dAngle));
					break;
				}
				delete EllipseSection;
			}

			// Assign the same section to the end as at the start (periodic yarns)
			if ( bPeriodic )
				pYarnSection->ReplaceSection(j, pYarnSection->GetNodeSection(0));

/*			// Now additional sections will be added between cross overs
			for (j=0; j<iYarnLength; ++j)
			{
				if (iDirection == 0)
				{
					// Set the parameters which will be used by GetCell for traversing an X yarn
					y = iPrevYarny = iNextYarny = j;
					iNextCrossx = x;
					iNextCrossy = (y+1)%iYarnLength;
				}
				else
				{
					// Set the parameters which will be used by GetCell for traversing a Y yarn
					x = iPrevYarnx = iNextYarnx = j;
					iNextCrossx = (x+1)%iYarnLength;
					iNextCrossy = y;
				}
				// If the yarn is going from the top to the bottom or vice versa, an additional section
				// is placed half way between the two
				if (GetCell(x, y) != GetCell(iNextCrossx, iNextCrossy))
				{
					YarnSection.InsertSection(j, 0.5, EllipseSection);
				}
			}*/

			pYarn->AssignSection(*pYarnSection);
			delete pYarnSection;
		}
	}

	return true;
}

void CTextileWeave2D::AdjustTangents() const
{
	vector<int>::iterator itpYarn;
	vector<CNode>::iterator itNode;
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			vector<CNode> MasterNodes = m_Yarns[*itpYarn].GetMasterNodes();
			for (itNode = MasterNodes.begin(); itNode != MasterNodes.end(); ++itNode)
			{
				itNode->SetTangent(XYZ(1, 0, 0));
			}
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			vector<CNode> MasterNodes = m_Yarns[*itpYarn].GetMasterNodes();
			for (itNode = MasterNodes.begin(); itNode != MasterNodes.end(); ++itNode)
			{
				itNode->SetTangent(XYZ(0, 1, 0));				
			}
		}
	}
}

bool CTextileWeave2D::BuildTextile() const
{
	if (!CTextileWeave::BuildTextile( ))
		return false;

	if (!m_bRefine && !m_bInPlaneTangents)
		return true;

	// Set interpolation to force in-plane tangents at node if m_bInPlaneTangents set
	if ( m_bInPlaneTangents )
		SetInPlaneTangents();
	AdjustTangents();

	if ( !m_bRefine )
		return true;

	TGLOGINDENT("Refining textile 2d weave \"" << GetName() << "\"");

	Refine();
	
	// TODO: Create a new function that modifies the cross sections (i.e. rotated ones)
	// Just get the yarn section of the yarn, replace the sections at the nodes without
	// touching the mid node sections

	// TODO: Then call correct interference again...

//	AssignRotatedSections();

//	CorrectInterference();

/*
	int i, j;

	CYarn *Yarn;
	XYZ PrevXPos, NextXPos;
	XYZ PrevYPos, NextYPos;
	XYZ Up;
	CNode NewNode;

	// Assign more adequate cross sections
	double dWidth, dHeight;
	int iPrevYarnx, iPrevYarny;
	int iNextYarnx, iNextYarny;
	int iNextCrossx, iNextCrossy;
	int iNextCrossPrevYarnx, iNextCrossPrevYarny;
	int iNextCrossNextYarnx, iNextCrossNextYarny;

	double dAngle;
	int iNumYarns, iYarnLength;
	int iDirection;
	int x, y;
	int iNodeNum;

	// First loop for X yarns, second loop for Y yarns
	for (iDirection=0; iDirection<2; ++iDirection)
	{
		if (iDirection==0)
		{
			iNumYarns = m_iNumYYarns;
			iYarnLength = m_iNumXYarns;
		}
		else
		{
			iNumYarns = m_iNumXYarns;
			iYarnLength = m_iNumYYarns;
		}
		for (i=0; i<iNumYarns; ++i)
		{
			if (iDirection==0)
			{
				iPrevYarnx = i-1;
				if (iPrevYarnx < 0)
					iPrevYarnx += iNumYarns;
				iNextYarnx = (i+1)%iNumYarns;

				// The angle is the maximum rotation angle to apply to the yarn at points where it needs
				// rotating, specified in radians.
				dAngle = atan2(0.5*m_dFabricThickness, m_YYarnData[iPrevYarnx].dSpacing+m_YYarnData[i].dSpacing);
				// Get the yarn width and height for this X yarn
				dWidth = m_YYarnData[i].dWidth;
				dHeight = m_YYarnData[i].dHeight;
				// Get a pointer to the current yarn
				Yarn = &m_Yarns[m_YYarns[i][0]];
				// Used for getting the cell coordinates
				x = i;
			}
			else
			{
				iPrevYarny = i-1;
				if (iPrevYarny < 0)
					iPrevYarny += iNumYarns;
				iNextYarny = (i+1)%iNumYarns;

				// The angle is the maximum rotation angle to apply to the yarn at points where it needs
				// rotating, specified in radians.
				dAngle = atan2(0.5*m_dFabricThickness, m_XYarnData[iPrevYarny].dSpacing+m_XYarnData[i].dSpacing);
				// Get the yarn width and height for this X yarn
				dWidth = m_XYarnData[i].dWidth;
				dHeight = m_XYarnData[i].dHeight;
				// Get a pointer to the current yarn
				Yarn = &m_Yarns[m_XYarns[i][0]];
				// Used for getting the cell coordinates
				y = i;
			}

			// Create the relevant sections that may be needed add different points along the yarn
			// NonContact will be used at cross over points where the yarn is not likely to interfere
			// with crossing yarn
			CSectionScaled NonContact(*m_pNonContact, XY(dWidth, dHeight));

			// Contact is used where the section is likely to interfere with crossing yarn
			// and no rotation is required. Also used half way between cross over points.
			CSectionScaled Contact(*m_pContact, XY(dWidth, dHeight));

			// RotatedContact is used where the section is likely to interfere with crossing yarn
			// and rotation is required
			CSectionScaled RotatedContact(*m_pRotatedContact, XY(dWidth, dHeight));

			// Build a yarn sections that will be applied to the nodes
			CYarnSectionInterpNode YarnSection(true, true);
			int iRot;		// Should have 1 of three values (-1, 0 or 1). -1 (rotation to the right), 0 (no rotation), 1 (rotation to the left)
			for (j=0; j<iYarnLength; ++j)
			{
				if (iDirection==0)
				{
					// Set the parameters which will be used by GetCell for traversing an X yarn
					y = iPrevYarny = iNextYarny = j;
				}
				else
				{
					// Set the parameters which will be used by GetCell for traversing a Y yarn
					x = iPrevYarnx = iNextYarnx = j;
				}
				// If the yarn pattern is the same on either side of the yarn at this cross over then no interference is likely
				// to occur thus assign a neutral cross section
				if (GetCell(iPrevYarnx, iPrevYarny) == GetCell(x, y) && GetCell(iNextYarnx, iNextYarny) == GetCell(x, y))
				{
					YarnSection.AddSection(NonContact);
				}
				else
				{
					// If the yarns on either side are the same then no rotation should occur
					if (GetCell(iPrevYarnx, iPrevYarny) == GetCell(iNextYarnx, iNextYarny))
						iRot = 0;
					else if (GetCell(iPrevYarnx, iPrevYarny)[0] == false)
						iRot = -1;	// Rotate to the right
					else
						iRot = 1;	// Rotate to the left
					// Determine weither the yarn is above or bellow
					if ((GetCell(x, y)[0] == true && iDirection == 0) ||
						(GetCell(x, y)[0] == false && iDirection == 1))
					{
						// Assign section based on the rotation it should have
						switch (iRot)
						{
						case 0:
							YarnSection.AddSection(CSectionHybrid(NonContact, Contact));
							break;
						case -1:
							YarnSection.AddSection(CSectionRotated(CSectionHybrid(NonContact, RotatedContact), -dAngle));
							break;
						case 1:
							YarnSection.AddSection(CSectionRotated(CSectionHybrid(NonContact, RotatedContact), dAngle));
							break;
						}
					}
					else
					{
						// Assign section based on the rotation it should have
						switch (iRot)
						{
						case 0:
							YarnSection.AddSection(CSectionHybrid(Contact, NonContact));
							break;
						case -1:
							YarnSection.AddSection(CSectionRotated(CSectionHybrid(RotatedContact, NonContact), -dAngle));
							break;
						case 1:
							YarnSection.AddSection(CSectionRotated(CSectionHybrid(RotatedContact, NonContact), dAngle));
							break;
						}
					}
				}
			}

			// Assign the same section to the end as at the start (periodic yarns)
			YarnSection.AddSection(YarnSection.GetSection(0));
			iNodeNum = 0;

			// Now additional nodes will be added between cross overs to ensure the path is
			// the way we want it
			for (j=0; j<iYarnLength; ++j)
			{
				if (iDirection == 0)
				{
					// Set the parameters which will be used by GetCell for traversing an X yarn
					y = iPrevYarny = iNextYarny = j;
					iNextCrossx = x;
					iNextCrossy = (y+1)%iYarnLength;
					iNextCrossPrevYarnx = iPrevYarnx;
					iNextCrossPrevYarny = iNextCrossy;
					iNextCrossNextYarnx = iNextYarnx;
					iNextCrossNextYarny = iNextCrossy;
				}
				else
				{
					// Set the parameters which will be used by GetCell for traversing a Y yarn
					x = iPrevYarnx = iNextYarnx = j;
					iNextCrossx = (x+1)%iYarnLength;
					iNextCrossy = y;
					iNextCrossPrevYarnx = iNextCrossx;
					iNextCrossPrevYarny = iPrevYarny;
					iNextCrossNextYarnx = iNextCrossx;
					iNextCrossNextYarny = iNextYarny;
				}
				// If the yarn is going from the top to the bottom or vice versa, an additional node
				// is placed half way between the two to ensure the path passes through that point
				// reducing the risk of interference between other yarns.
				if (GetCell(x, y) != GetCell(iNextCrossx, iNextCrossy))
				{
					// Insert a node
					CNode NewNode(0.5*(Yarn->GetMasterNodes()[iNodeNum].GetPosition() + Yarn->GetMasterNodes()[iNodeNum+1].GetPosition()));
					Yarn->InsertNode(NewNode, ++iNodeNum);

					// Decide what each quadrant of the section should look like based on the
					// pattern around the cross over
					CSection *pTopRight = &NonContact;
					CSection *pTopLeft = &NonContact;
					CSection *pBottomLeft = &NonContact;
					CSection *pBottomRight = &NonContact;
					if (GetCell(x, y) != GetCell(iNextYarnx, iNextYarny))
						pTopRight = &Contact;
					if (GetCell(x, y) != GetCell(iPrevYarnx, iPrevYarny))
						pTopLeft = &Contact;
					if (GetCell(iNextCrossx, iNextCrossy) != GetCell(iNextCrossPrevYarnx, iNextCrossPrevYarny))
						pBottomLeft = &Contact;
					if (GetCell(iNextCrossx, iNextCrossy) != GetCell(iNextCrossNextYarnx, iNextCrossNextYarny))
						pBottomRight = &Contact;
					// Swap the upper and lower parts of the section
					if ((GetCell(x, y)[0] == true && iDirection == 0) ||
						(GetCell(x, y)[0] == false && iDirection == 1))
					{
						swap(pTopRight, pBottomRight);
						swap(pTopLeft, pBottomLeft);
					}
					// Swap the side parts of the section
					if (iDirection == 1)
					{
						swap(pTopRight, pTopLeft);
						swap(pBottomRight, pBottomLeft);
					}

					CSectionHybrid Switch(*pTopRight, *pTopLeft, *pBottomLeft, *pBottomRight);
					YarnSection.InsertSection(iNodeNum, Switch);
				}
				++iNodeNum;
			}
			Yarn->AssignSection(YarnSection);
		}
	}
*/
	return true;
}

void CTextileWeave2D::Refine( bool bCorrectWidths, bool bCorrectInterference, bool bPeriodic ) const
{
	CTimer timer;
	timer.start("Timing Refine");
	
	if ( bCorrectWidths )
	{
		CorrectYarnWidths();
	}

	if ( bCorrectInterference )
		CorrectInterference();

	AdjustSectionsForRotation( bPeriodic );

	if ( bCorrectInterference )
		CorrectInterference();

	timer.check("End of Refine");
	timer.stop();
}


void CTextileWeave2D::RefineTextile( bool bCorrectWidths, bool bCorrectInterference, bool bPeriodic )
{
	//BuildTextileIfNeeded();
	if ( GetType() != "CTextileWeave2D" )
		return;
	Refine( bCorrectWidths, bCorrectInterference, bPeriodic );
	//m_bNeedsBuilding = true;
}



string CTextileWeave2D::GetDefaultName() const
{
	 return "2DWeave(W:" + stringify(m_iNumYYarns) + ",H:" + stringify(m_iNumXYarns) + ")";
}


/// Function to set interpolation so that in-plane tangents are forced at master nodes
void CTextileWeave2D::SetInPlaneTangents( bool bInPlaneTangents ) const
{
	vector<int>::iterator itpYarn;
	
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			SetInPlaneTangents(*itpYarn, bInPlaneTangents );
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			SetInPlaneTangents(*itpYarn, bInPlaneTangents );
		}
	}
}

void CTextileWeave2D::SetInPlaneTangents( int Yarn, bool bInPlaneTangents ) const
{
	const CInterpolation* Interpolation = m_Yarns[Yarn].GetInterpolation();
	if ( Interpolation->GetType() == "CInterpolationCubic" )
	{
		m_Yarns[Yarn].AssignInterpolation(CInterpolationCubic( true, false, bInPlaneTangents ));
	}
	else
	{
		m_Yarns[Yarn].AssignInterpolation(CInterpolationBezier( true, false, bInPlaneTangents ));
	}
}

CDomainPlanes CTextileWeave2D::GetDefaultDomain( bool bSheared, bool bAddedDomainHeight )
{
	return CTextileWeave::GetDefaultDomain( bSheared, bAddedDomainHeight );
}


void CTextileWeave2D::ConvertToPatternDraft( /*CPatternDraft& PatternDraft*/ )
{
	m_PatternDraft.ClearWeavePattern();
	for ( int i = m_iNumYYarns-1; i >=0; --i )
	{
		string Row;
		for ( int j = m_iNumXYarns-1; j >= 0 ; --j )
		{
			if ( GetCell(i,j)[1] == PATTERN_XYARN )
				Row.push_back('1');
			else
				Row.push_back('0');
		}
		m_PatternDraft.AddRow( Row );
	}
}
