/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2013 Louise Brown

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
#include "ShearedTextileWeave2D.h"
#include "SectionEllipse.h"
#include "SectionRotated.h"
#include "SectionPolygon.h"

using namespace TexGen;

#define MID_SECTIONS	4
# define TOL 0.000001

CShearedTextileWeave2D::CShearedTextileWeave2D(int iWidth, int iHeight, double dSpacing, double dThickness, double ShearAngle, bool bRefine, bool bInPlaneTangents )
: CTextileWeave2D(iWidth, iHeight, dSpacing, dThickness, bRefine, bInPlaneTangents )
, m_ShearAngle(ShearAngle)
, m_bShearedDomain(false)
{
}

CShearedTextileWeave2D::~CShearedTextileWeave2D(void)
{
}

CShearedTextileWeave2D::CShearedTextileWeave2D(TiXmlElement &Element)
: CTextileWeave2D(Element)
{
	Element.Attribute("ShearAngle", &m_ShearAngle);
}

void CShearedTextileWeave2D::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileWeave2D::PopulateTiXmlElement( Element, OutputType );
	Element.SetAttribute("ShearAngle", stringify(m_ShearAngle));
}

bool CShearedTextileWeave2D::BuildTextile() const
{
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	if (!Valid())
		return false;

	TGLOGINDENT("Building sheared textile weave \"" << GetName() << "\"");

	vector<int> Yarns;

	double x, y, z;

	GetYSpacings();
	
	// Add x yarns (yarns parallel to the x axis)
	int i, j, k, iYarn;
	y = 0;
	double Startx = 0;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		x = Startx;
		Yarns.clear();
		for (j=0; j<=m_iNumYYarns; ++j)
		{
			const vector<PATTERN2D> &Cell = GetCell(j%m_iNumYYarns, i);
			if (j==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN_XYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_XYarns[i] = Yarns;
			iYarn = 0;
			z = m_dFabricThickness/(2*Cell.size());
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN_XYARN)
				{
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
				}
				z += m_dFabricThickness/Cell.size();
			}
			if (j<m_iNumYYarns)
				x += m_YYarnData[j].dSpacing;
		}
		Startx += m_YSpacing[i].x;
		y += m_YSpacing[i].y;
	}

	// Add y yarns (yarns parallel to the y axis)
	Startx = 0;
	for (j=0; j<m_iNumYYarns; ++j)
	{
		x = Startx;
		y = 0;
		Yarns.clear();
		for (i=0; i<=m_iNumXYarns; ++i)
		{
			const vector<PATTERN2D> &Cell = GetCell(j, i%m_iNumXYarns);
			if (i==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN_YYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_YYarns[j] = Yarns;
			iYarn = 0;
			z = m_dFabricThickness/(2*Cell.size());
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN_YYARN)
				{
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
				}
				z += m_dFabricThickness/Cell.size();
			}
			if (i<m_iNumXYarns)
			{
				x += m_YSpacing[i].x;
				y += m_YSpacing[i].y;
			}
		}
		Startx += m_YYarnData[j].dSpacing;
	}


	// Assign sections and interpolation to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;
		CSectionEllipse Section(dWidth, dHeight);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			m_Yarns[*itpYarn].AssignInterpolation(CInterpolationBezier());
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		CSectionEllipse Section(dWidth, dHeight);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			m_Yarns[*itpYarn].AssignInterpolation(CInterpolationCubic());
		}
	}

	

	// Add repeats and set interpolation
	dWidth = GetWidth();
	XY Repeat = GetYRepeat();

	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->SetResolution(m_iResolution);
		itYarn->AddRepeat(XYZ(dWidth, 0, 0));
		itYarn->AddRepeat(XYZ(Repeat.x, Repeat.y, 0));
	}

	if (!m_bRefine && !m_bInPlaneTangents)
		return true;

	// Set interpolation to force in-plane tangents at node if m_bInPlaneTangents set
	if ( m_bInPlaneTangents )
		SetInPlaneTangents();
	AdjustTangents();

	if ( !m_bRefine )
		return true;

	//TGLOGINDENT("Refining textile 2d weave \"" << GetName() << "\"");

	Refine();
	
	return true;
}

CDomainPlanes CShearedTextileWeave2D::GetDefaultDomain( bool bSheared, bool bAddedDomainHeight )
{
	XYZ Min, Max;
	double dGap = 0.0;
	if ( bAddedDomainHeight )
		dGap = 0.05*m_dFabricThickness;
	
	m_bShearedDomain = bSheared;
	if ( m_YSpacing.empty() )
		GetYSpacings();
	if ( bSheared )
		Min.x = -0.5*m_YYarnData[m_iNumYYarns-1].dSpacing;
	else  // Work out minimum x taking into account shear angle
		Min.x = -0.5*(m_YYarnData[m_iNumYYarns-1].dSpacing + m_YSpacing[m_iNumYYarns-1].x);
	Min.y = -0.5*m_YSpacing[m_iNumXYarns-1].y;
	Min.z = -dGap;

	Max.x = Min.x + GetWidth();
	Max.y = Min.y + GetYRepeat().y;
	Max.z = m_dFabricThickness+dGap;
	if ( !bSheared )
		return CDomainPlanes(Min, Max);

	CDomainPlanes Domain;
	double cosAng = cos(m_ShearAngle);
	double sinAng = sin(m_ShearAngle);
	
	Domain.AddPlane( PLANE( XYZ( cosAng, -sinAng, 0), Min.x*cosAng) );
	Domain.AddPlane( PLANE( XYZ( -cosAng, sinAng, 0), -Max.x*cosAng) );
	Domain.AddPlane( PLANE( XYZ(0, 1, 0), Min.y ) );
	Domain.AddPlane( PLANE( XYZ(0, -1, 0), -Max.y ) );
	Domain.AddPlane( PLANE( XYZ(0, 0, 1), Min.z ) );
	Domain.AddPlane( PLANE( XYZ(0, 0, -1), -Max.z ) );
	return Domain;
}

void CShearedTextileWeave2D::GetYSpacings() const
{
	m_YSpacing.clear();
	double sinAngle = sin(m_ShearAngle);
	double cosAngle = cos(m_ShearAngle);

	for ( int i = 0; i < m_iNumXYarns; ++i )
	{
		XY spacing;
		spacing.x = m_XYarnData[i].dSpacing * sinAngle;
		spacing.y = m_XYarnData[i].dSpacing * cosAngle;
		m_YSpacing.push_back( spacing );
	}
}

XY CShearedTextileWeave2D::GetYRepeat() const
{
	XY YRepeat;
	vector<XY>::iterator itYSpacings;

	for ( itYSpacings = m_YSpacing.begin(); itYSpacings != m_YSpacing.end(); ++itYSpacings )
	{
		YRepeat += *itYSpacings;
	}
	return YRepeat;
}

void CShearedTextileWeave2D::Refine() const
{
	CTimer timer;
	timer.start("Timing Refine");

	ConvertToInterpNodes();

	AdjustSectionsForRotation();

	CorrectInterference();
	if ( GetDomain() )
		AdjustSections();
	else
		TGERROR("Cannot adjust sections - no domain specified");
	//CorrectInterference( true );
	timer.check("End of Refine");
	timer.stop();
}

bool CShearedTextileWeave2D::AdjustSectionsForRotation() const
{
	int i, j;
	vector<vector<int> > *pTransverseYarns;
	vector<vector<int> > *pLongitudinalYarns;
	vector<YARNDATA> *pTransverseData;
	vector<YARNDATA> *pLongitudinalData;
	int iTransverseNum;
	int iLongitudinalNum;
	bool iTransversePattern;
	bool iLongitudinalPattern;

	CYarn *pYarn;

	double dTransWidth, dTransHeight;
	int iPrevTransYarnInd;
	int iNextTransYarnInd, iNextLongYarnInd;

	for (int iDir=0; iDir<2; ++iDir)
	{
		switch (iDir)
		{
		case 0:
			pTransverseYarns = &m_YYarns;
			pLongitudinalYarns = &m_XYarns;
			iTransverseNum = m_iNumYYarns;
			iLongitudinalNum = m_iNumXYarns;
			pTransverseData = &m_YYarnData;
			pLongitudinalData = &m_XYarnData;
			iTransversePattern = PATTERN_YYARN;
			iLongitudinalPattern = PATTERN_XYARN;
			break;
		case 1:
			pTransverseYarns = &m_XYarns;
			pLongitudinalYarns = &m_YYarns;
			iTransverseNum = m_iNumXYarns;
			iLongitudinalNum = m_iNumYYarns;
			pTransverseData = &m_XYarnData;
			pLongitudinalData = &m_YYarnData;
			iTransversePattern = PATTERN_XYARN;
			iLongitudinalPattern = PATTERN_YYARN;
			break;
		}
		// Transverse yarns
		for ( i = 0; i < iTransverseNum; ++i )
		{
			iPrevTransYarnInd = i-1;
			if (iPrevTransYarnInd < 0)
				iPrevTransYarnInd += iTransverseNum;
			iNextTransYarnInd = (i+1)%iTransverseNum;

			// Get the yarn width and height for this transverse yarn
			dTransWidth = (*pTransverseData)[i].dWidth;
			dTransHeight = (*pTransverseData)[i].dHeight;
			// Get a pointer to the current yarn
			pYarn = &m_Yarns[(*pTransverseYarns)[i][0]];
			CSectionEllipse EllipseSection(dTransWidth, dTransHeight);

			// Get a copy of the yarn sections that are applied to the nodes
			if (pYarn->GetYarnSection()->GetType() != "CYarnSectionInterpNode")
				return false;
			CYarnSectionInterpNode* pYarnSection = (CYarnSectionInterpNode*)pYarn->GetYarnSection()->Copy();

			// Calculate rotations between each pair of nodes along transverse yarn
			// u = 0 at current node, u = 1 at next node
			for (j=0; j<iLongitudinalNum; ++j)
			{
				iNextLongYarnInd = (j+1)%iLongitudinalNum;
			
				// Get widths of current and next longitudinal yarns
				double dLongWidth = (*pLongitudinalData)[j].dWidth;
				double dLongHeight = (*pLongitudinalData)[j].dHeight;
				
				double dNextLongWidth = (*pLongitudinalData)[iNextLongYarnInd].dWidth;
				double dNextLongHeight = (*pLongitudinalData)[iNextLongYarnInd].dHeight;
				double dDistToLongYarnEdge = 0.5 * dLongWidth / cos(m_ShearAngle);

				// Get nodes in transverse yarn and distance between them
				XYZ Node = pYarn->GetNode(j)->GetPosition();
				XYZ NextNode = pYarn->GetNode(iNextLongYarnInd)->GetPosition();
				double dDistBetweenNodes = GetLength( Convert(Node), Convert(NextNode) );

				// Calculate value of u between transverse yarn nodes where it crosses edges of this and next longitudinal yarns
				double dDistToNextLongYarnEdge = 0.5 * dNextLongWidth / cos(m_ShearAngle);

				// Calculate limits of R to avoid square root of -ve number in CalcSinAngle function
				double RCalc = 1.0 / sqrt( 1.0 - 1.0/((dLongWidth/dLongHeight)*(dLongWidth/dLongHeight)) );
				double RMax = 0.25 * ( 1.0 + 0.2*sin(m_ShearAngle) + RCalc );
				if ( RMax > 0.495 ) // Set to max of 0.99 * 0.5
					RMax = 0.495;
			

				RCalc = 1.0 / sqrt( 1.0 - 1.0/((dNextLongWidth/dNextLongHeight)*(dNextLongWidth/dNextLongHeight)) );
				double RMin = 0.25 * ( 1.0 - 0.2*sin(m_ShearAngle) - RCalc );
				if ( RMin < 0.005 )
					RMin = 0.005;	
				
				// For specified number of equispaced points across each yarn
				// calculate corresponding R values where R varies from 0 to 0.5 across width of yarn
				// and use to calculate rotations
				double dR = (RMax - 0.25) / (MID_SECTIONS/2);
				for ( int k = 0; k <= MID_SECTIONS/2; ++k )
				{
					double u = 0.0;
					double dist;
					double R;
					double dAngle;
					int iRot = 0;
					
					R = 0.25 + dR*k;
						
					dAngle = asin(CalcSinAngle( R, dTransWidth, dTransHeight ));
					if ( GetCellDir(i,j, iDir)[0] == iTransversePattern )
					{
						if ( GetCellDir( i, iNextLongYarnInd, iDir )[1] == iTransversePattern && GetCellDir(iNextTransYarnInd,j, iDir)[0] == iLongitudinalPattern )  
						{
							if ( iDir == 0 )
								iRot = -1;
							else
								iRot = 1; 
						}
					}
					else
					{
						if ( GetCellDir( i, iNextLongYarnInd, iDir)[0] == iTransversePattern && GetCellDir(iNextTransYarnInd, j, iDir)[1] == iLongitudinalPattern )
						{
							if ( iDir == 0 )
								iRot = 1;							
							else
								iRot = -1;
						}
					}
					
					dist = (R - 0.25) * (4.0 * dDistToLongYarnEdge);
					u = dist/dDistBetweenNodes;
					
					// If rotation required at node replace section
					if ( k == 0 )
					{
						if ( iRot )
							pYarnSection->ReplaceSection(j, CSectionRotated(EllipseSection, iRot*dAngle));
					}
					else  // Add mid-node sections, rotating if necessary
					{
						if ( iRot )
							pYarnSection->InsertSection( j, u, CSectionRotated( EllipseSection, iRot*dAngle));
						else
							pYarnSection->InsertSection( j, u, EllipseSection );
					}
						
				}

				dR = ( 0.25 - RMin ) / (MID_SECTIONS/2);
				for ( int k = 0; k < MID_SECTIONS/2; ++k )
				{
					double u = 0.0;
					double dist;
					double R;
					double dAngle;
					int iRot = 0;
					
					R = RMin + dR*k;
						
					dAngle = asin(CalcSinAngle( R, dTransWidth, dTransHeight ));
					
					if ( GetCellDir(i, iNextLongYarnInd, iDir)[0] == iTransversePattern )
					{
						if ( GetCellDir( i, j, iDir )[1] == iTransversePattern && GetCellDir(iPrevTransYarnInd,iNextLongYarnInd, iDir)[0] == iLongitudinalPattern )
						{
							if ( iDir == 0 )
								iRot = 1;
							else
								iRot = -1;
						}
					}
					else
					{
						if ( GetCellDir( i, j, iDir )[0] == iTransversePattern && GetCellDir( iPrevTransYarnInd, iNextLongYarnInd, iDir )[1] == iLongitudinalPattern )
						{
							if ( iDir == 0 )
								iRot = -1;
							else
								iRot = 1;
						}
					}
						
					dist = dDistBetweenNodes - (0.25 - R) * ( 4.0 * dDistToNextLongYarnEdge );
					u = dist/dDistBetweenNodes;

					// Add mid-node sections, rotating if necessary
					if ( iRot )
					{
						pYarnSection->InsertSection( j, u, CSectionRotated( EllipseSection, iRot*dAngle));
					}	
					else
					{
						pYarnSection->InsertSection( j, u, EllipseSection );
					}
				}
			}
			// Assign the same section to the end as at the start (periodic yarns)
			pYarnSection->ReplaceSection(j, pYarnSection->GetNodeSection(0));
			pYarn->AssignSection(*pYarnSection);
			
			delete pYarnSection;
		}
	}
	return true;
}

double CShearedTextileWeave2D::CalcSinAngle( double R, double Width, double Height ) const
{
	double denom;
	double WidthHeight = (Width/Height);
	WidthHeight *= WidthHeight;
	if ( R <= 0.25 )
		denom = (1 - 4*R - 0.2*sin(m_ShearAngle));
	else
		denom = (1 - 4*R + 0.2*sin(m_ShearAngle));
	denom *= denom;
	denom = 1/denom - 1;
	denom = 1 + WidthHeight*denom;
	denom = sqrt(denom);

	return sin(m_ShearAngle)/denom;
}

void CShearedTextileWeave2D::CorrectInterference( bool bMaxCorrection ) const
{
	TGLOGINDENT("Correcting interference for \"" << GetName() << 
		"\" with gap size of " << m_dGapSize);

	vector<vector<int> > *pTransverseYarns;
	vector<vector<int> > *pLongitudinalYarns;
	vector<YARNDATA> *pTransverseData;
	int iTransverseNum;
	int iLongitudinalNum;
	int iDir;
	int i, j;
	CMesh TransverseYarnsMesh;
	CMesh NextTransverseYarnsMesh;
	vector<int>::iterator itpYarn;
	vector<pair<int, int> > RepeatLimits;
	vector<pair<double, XYZ> > Intersections;
	XYZ Center, P;
	
	const CInterpolation* pInterpolation;
	CSlaveNode Node;
	XYZ Side, Up;
	YARN_POSITION_INFORMATION YarnPosInfo;

	RepeatLimits.resize(2, pair<int, int>(-1, 1));
	vector<double> Modifiers;
	vector<vector<vector<double> > > YarnSectionModifiers;
	YarnSectionModifiers.resize(m_Yarns.size());

	// Find at how much the cross sections need to be compressed to leave given gap size
	for (iDir=0; iDir<2; ++iDir)
	{
		switch (iDir)
		{
		case 0:
			pTransverseYarns = &m_YYarns;
			pLongitudinalYarns = &m_XYarns;
			iTransverseNum = m_iNumYYarns;
			iLongitudinalNum = m_iNumXYarns;
			pTransverseData = &m_YYarnData;
			break;
		case 1:
			pTransverseYarns = &m_XYarns;
			pLongitudinalYarns = &m_YYarns;
			iTransverseNum = m_iNumXYarns;
			iLongitudinalNum = m_iNumYYarns;
			pTransverseData = &m_XYarnData;
			break;
		}
		for (i=0; i<iTransverseNum; ++i)
		{
			TransverseYarnsMesh.Clear();
			if ( i == 0 )
			{
				for (itpYarn = (*pTransverseYarns)[i].begin(); itpYarn != (*pTransverseYarns)[i].end(); ++itpYarn)
				{
					m_Yarns[*itpYarn].AddSurfaceToMesh(TransverseYarnsMesh, RepeatLimits);
				}
				//TransverseYarnsMesh.Convert3Dto2D();
				TransverseYarnsMesh.ConvertQuadstoTriangles();
			}
			else
			{
				TransverseYarnsMesh = NextTransverseYarnsMesh;
			}

			NextTransverseYarnsMesh.Clear();
			for (itpYarn = (*pTransverseYarns)[(i+1)%iTransverseNum].begin(); itpYarn != (*pTransverseYarns)[(i+1)%iTransverseNum].end(); ++itpYarn)
			{
				m_Yarns[*itpYarn].AddSurfaceToMesh(NextTransverseYarnsMesh, RepeatLimits);
			}
			//NextTransverseYarnsMesh.Convert3Dto2D();
			NextTransverseYarnsMesh.ConvertQuadstoTriangles();

			for (j=0; j<iLongitudinalNum; ++j)
			{
				for (itpYarn = (*pLongitudinalYarns)[j].begin(); itpYarn != (*pLongitudinalYarns)[j].end(); ++itpYarn)
				{
					YarnPosInfo.iSection = i;
					YarnPosInfo.dSectionPosition = 0;
					YarnPosInfo.SectionLengths = m_Yarns[*itpYarn].GetYarnSectionLengths();

					XYZ Node = m_Yarns[*itpYarn].GetNode(j)->GetPosition();
					XYZ NextNode = m_Yarns[*itpYarn].GetNode((j+1)%iLongitudinalNum)->GetPosition();

					double dDistBetweenNodes = GetLength( Convert(Node), Convert(NextNode) );

					pInterpolation = m_Yarns[*itpYarn].GetInterpolation();
					// Calculate adjustments to section at node
					CalculateModifiers( pInterpolation, *itpYarn, YarnPosInfo, TransverseYarnsMesh, YarnSectionModifiers, bMaxCorrection );

					// Calculate the value of u at the edge of the yarn crossing the first node 
					double DistToEdgeU = (0.5 * (*pTransverseData)[i].dWidth / cos(m_ShearAngle) )/ dDistBetweenNodes;

					CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[*itpYarn].GetYarnSection()->Copy();
					
					// Calculate adjustments for each of the mid node sections
					int iMidSize = pYarnSections->GetNumMidNodeSections(i);
					for ( int k = 0; k < iMidSize; ++k )
					{
						YarnPosInfo.dSectionPosition = pYarnSections->GetMidNodeSectionPos( i, k );
						if ( YarnPosInfo.dSectionPosition <= DistToEdgeU )
							CalculateModifiers( pInterpolation, *itpYarn, YarnPosInfo, TransverseYarnsMesh, YarnSectionModifiers, bMaxCorrection );
						else 
							CalculateModifiers( pInterpolation, *itpYarn, YarnPosInfo, NextTransverseYarnsMesh, YarnSectionModifiers, bMaxCorrection );
					}
					delete pYarnSections;
				}
			}
		}
	}

	// Apply modifiers to yarn sections
	
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		CYarnSectionInterpNode NewYarnSection(false, true);
		CYarnSectionInterpNode* pYarnSection = (CYarnSectionInterpNode*)m_Yarns[i].GetYarnSection()->Copy();
		YarnPosInfo.SectionLengths = m_Yarns[i].GetYarnSectionLengths();
		
		int iNodeIndex = 0;
		for (j=0; j<(int)YarnSectionModifiers[i].size(); )
		{
			// Modify section at node
			YarnPosInfo.iSection = iNodeIndex;
			YarnPosInfo.dSectionPosition = 0;
			
			ModifySection(pYarnSection, YarnPosInfo, YarnSectionModifiers[i][j], iNodeIndex);
			
			// Modify mid node sections
			int iMidSize = pYarnSection->GetNumMidNodeSections(iNodeIndex);
			j++;
			for ( int iMidIndex = 0; iMidIndex < iMidSize; ++iMidIndex )
			{
				YarnPosInfo.dSectionPosition = pYarnSection->GetMidNodeSectionPos( iNodeIndex, iMidIndex );
				
				ModifySection(pYarnSection, YarnPosInfo, YarnSectionModifiers[i][j], iNodeIndex, iMidIndex);
				j++;
			}
			iNodeIndex++;
		}
		// Make sure end matches start
		pYarnSection->ReplaceSection(pYarnSection->GetNumNodeSections()-1, pYarnSection->GetNodeSection(0) );

		m_Yarns[i].AssignSection(*pYarnSection);
		delete pYarnSection;
	}
}


void CShearedTextileWeave2D::CalculateModifiers( const CInterpolation *pInterpolation, int Yarn, YARN_POSITION_INFORMATION YarnPosInfo, CMesh& Mesh, vector<vector<vector<double> > >& YarnSectionModifiers, bool bMaxCorrection) const
{
	XYZ Up, Side;
	CSlaveNode Node;
	const CYarnSection* pYarnSection;
	XYZ Centre;
	vector<double> Modifiers;

	Node = pInterpolation->GetNode(m_Yarns[Yarn].GetMasterNodes(), YarnPosInfo.iSection, YarnPosInfo.dSectionPosition);
	Up = Node.GetUp();
	Side = CrossProduct(Node.GetTangent(), Up);
	Centre = Node.GetPosition();

	pYarnSection = m_Yarns[Yarn].GetYarnSection();
	vector<XY> Points = pYarnSection->GetSection(YarnPosInfo, m_Yarns[Yarn].GetNumSectionPoints());
	
	// Calculate intersections between line joining centre of section and each point and the yarn mesh
	vector<XY>::iterator itPoint;
	Modifiers.clear();
	for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint)
	{
		XYZ P = itPoint->x * Side + itPoint->y * Up + Centre;
		vector<pair<double, XYZ> > Intersections;
		if (Mesh.IntersectLine(Centre, P, Intersections, make_pair(true, false)))
		{
			double dU = Intersections[0].first;
			if ( !bMaxCorrection && dU > 0 && dU < 1 )  // Only want to change by half intersection otherwise create gap
				dU = 0.5 + 0.5*dU;
			XYZ Normal = Intersections[0].second;
			double dProjectedGap = m_dGapSize / DotProduct(Normal, Centre-P);
			dU -= 0.5 * dProjectedGap;
			if (dU > 1)
				dU = 1;
			if (dU < 0)
				dU = 0;
			Modifiers.push_back(dU);
		}
		else
			Modifiers.push_back(1);
	}
	YarnSectionModifiers[Yarn].push_back(Modifiers);
}

void CShearedTextileWeave2D::ModifySection(CYarnSectionInterpNode* pYarnSection, YARN_POSITION_INFORMATION& YarnPosInfo, vector<double>& Modifiers, int iNodeIndex, int iMidIndex) const
{
	vector<XY> Points;
	Points = pYarnSection->GetSection(YarnPosInfo, (int)Modifiers.size());
	for (int k=0; k<(int)Points.size(); ++k)
	{
		Points[k] *= Modifiers[k];
	}
	CSectionPolygon Section(Points);
	if (m_pSectionMesh)
		Section.AssignSectionMesh(*m_pSectionMesh);
	if ( iMidIndex == -1 )
		pYarnSection->ReplaceSection( iNodeIndex, Section );
	else
		pYarnSection->ReplaceMidSection( iNodeIndex, iMidIndex, Section );
}

void CShearedTextileWeave2D::AdjustSections() const
{
	int i, j;
	vector<double> YarnSectionModifiers;
	const CInterpolation* pInterpolation;

	CMesh TopMesh, BottomMesh;
	CreateTopAndBottomMeshes( TopMesh, BottomMesh );
	
	// Y yarns (parallel to y axis)
	for ( i = 0; i < m_iNumYYarns; ++i )
	{
		// Get Y yarn(i) 
		int YarnInd = m_YYarns[i][0];
		pInterpolation = m_Yarns[YarnInd].GetInterpolation();
		CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[YarnInd].GetYarnSection()->Copy();

		// Get area of original yarn section
		CSectionEllipse Section(m_YYarnData[i].dWidth, m_YYarnData[i].dHeight);
		double dTargetArea = Section.GetArea( Section.GetPoints(40) );

		YarnSectionModifiers.clear();
		for ( j = 0; j < m_iNumXYarns; ++j )
		{
			YARN_POSITION_INFORMATION YarnPosInfo;
			YarnPosInfo.iSection = j;
			YarnPosInfo.dSectionPosition = 0;
			YarnPosInfo.SectionLengths = m_Yarns[YarnInd].GetYarnSectionLengths();
			// For section at node (i,j)
			const vector<PATTERN2D> &Cell = GetCell(i,j);
			if ( Cell[0] == PATTERN_YYARN )
				CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, BottomMesh, YarnSectionModifiers );
			else
				CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, TopMesh, YarnSectionModifiers );
		
			ModifySection( pYarnSections, YarnPosInfo, YarnSectionModifiers, dTargetArea, m_YYarnData[i].dHeight, j );	
					
			// Calculate adjustments for each of the mid node sections
			for ( int k = 0; k < MID_SECTIONS; ++k )
			{
				YarnPosInfo.dSectionPosition = pYarnSections->GetMidNodeSectionPos( j, k );
				if ( k < MID_SECTIONS/2 )
				{
					if ( Cell[0] == PATTERN_YYARN )
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, BottomMesh, YarnSectionModifiers );
					else
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, TopMesh, YarnSectionModifiers );
				}
				else
				{
					const vector<PATTERN2D> &NextCell = GetCell(i,(j+1)%m_iNumXYarns);
					if ( NextCell[0] == PATTERN_YYARN )
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, BottomMesh, YarnSectionModifiers );
					else
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, TopMesh, YarnSectionModifiers );
				}
				ModifySection( pYarnSections, YarnPosInfo, YarnSectionModifiers, dTargetArea, m_YYarnData[i].dHeight, j, k );	
			}
		}
		// Make sure end matches start
		pYarnSections->ReplaceSection(pYarnSections->GetNumNodeSections()-1, pYarnSections->GetNodeSection(0) );
		m_Yarns[YarnInd].AssignSection( *pYarnSections );
		delete pYarnSections;
	}

	// X yarns (parallel to x axis)
	for ( j = 0; j < m_iNumXYarns; ++j )
	{
		// Get X yarn(j) 
		int YarnInd = m_XYarns[j][0];
		pInterpolation = m_Yarns[YarnInd].GetInterpolation();
		CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[YarnInd].GetYarnSection()->Copy();

		// Get area of original yarn section
		CSectionEllipse Section(m_XYarnData[j].dWidth, m_XYarnData[j].dHeight);
		double dTargetArea = Section.GetArea( Section.GetPoints(40) );

		YarnSectionModifiers.clear();
		for ( i = 0; i < m_iNumYYarns; ++i )
		{
			YARN_POSITION_INFORMATION YarnPosInfo;
			YarnPosInfo.iSection = i;
			YarnPosInfo.dSectionPosition = 0;
			YarnPosInfo.SectionLengths = m_Yarns[YarnInd].GetYarnSectionLengths();
			// For section at node (i,j)
			const vector<PATTERN2D> &Cell = GetCell(i,j);
			if ( Cell[0] == PATTERN_XYARN )
				CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, BottomMesh, YarnSectionModifiers );
			else
				CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, TopMesh, YarnSectionModifiers );

			ModifySection( pYarnSections, YarnPosInfo, YarnSectionModifiers, dTargetArea, m_XYarnData[j].dHeight, i);	
					
			// Calculate adjustments for each of the mid node sections
			for ( int k = 0; k < MID_SECTIONS; ++k )
			{
				YarnPosInfo.dSectionPosition = pYarnSections->GetMidNodeSectionPos( i, k );
				if ( k < MID_SECTIONS/2 )
				{
					if ( Cell[0] == PATTERN_XYARN )
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, BottomMesh, YarnSectionModifiers );
					else
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, TopMesh, YarnSectionModifiers );
				}
				else
				{
					const vector<PATTERN2D> &NextCell = GetCell((i+1)%m_iNumYYarns,j);
					if ( NextCell[0] == PATTERN_XYARN )
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, BottomMesh, YarnSectionModifiers );
					else
						CalculateModifiers( pInterpolation, YarnInd, YarnPosInfo, TopMesh, YarnSectionModifiers );
				}
				ModifySection( pYarnSections, YarnPosInfo, YarnSectionModifiers, dTargetArea, m_XYarnData[j].dHeight, i, k );	
			}	
		}
		// Make sure end matches start
		pYarnSections->ReplaceSection(pYarnSections->GetNumNodeSections()-1, pYarnSections->GetNodeSection(0) );
		m_Yarns[YarnInd].AssignSection( *pYarnSections );
		delete pYarnSections;
	}
}

void CShearedTextileWeave2D::CreateTopAndBottomMeshes( CMesh& TopMesh, CMesh& BottomMesh ) const
{
	pair<XYZ, XYZ> AABB = GetDomain()->GetMesh().GetAABB();  
	double sizex = AABB.second.x - AABB.first.x;
	double sizey = AABB.second.y - AABB.first.y;
	double minx = AABB.first.x - sizex;
	double maxx = AABB.second + sizex;
	double miny = AABB.first.y - sizey;
	double maxy = AABB.second.y + sizey;

	/*TopMesh.AddNode( XYZ(minx, miny, AABB.second.z) );
	TopMesh.AddNode( XYZ(minx, maxy, AABB.second.z) );
	TopMesh.AddNode( XYZ(maxx, maxy, AABB.second.z) );
	TopMesh.AddNode( XYZ(maxx, miny, AABB.second.z) );

	BottomMesh.AddNode( XYZ(minx, miny, AABB.first.z) );
	BottomMesh.AddNode( XYZ(minx, maxy, AABB.first.z) );
	BottomMesh.AddNode( XYZ(maxx, maxy, AABB.first.z) );
	BottomMesh.AddNode( XYZ(maxx, miny, AABB.first.z) );*/

	TopMesh.AddNode( XYZ(minx, miny, m_dFabricThickness) );
	TopMesh.AddNode( XYZ(minx, maxy, m_dFabricThickness) );
	TopMesh.AddNode( XYZ(maxx, maxy, m_dFabricThickness) );
	TopMesh.AddNode( XYZ(maxx, miny, m_dFabricThickness) );

	BottomMesh.AddNode( XYZ(minx, miny, 0.0) );
	BottomMesh.AddNode( XYZ(minx, maxy, 0.0) );
	BottomMesh.AddNode( XYZ(maxx, maxy, 0.0) );
	BottomMesh.AddNode( XYZ(maxx, miny, 0.0) );

	vector<int> Indices;
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(3);
	TopMesh.AddElement(CMesh::TRI, Indices);
	BottomMesh.AddElement(CMesh::TRI, Indices);
	Indices.clear();
	Indices.push_back(1);
	Indices.push_back(2);
	Indices.push_back(3);
	TopMesh.AddElement(CMesh::TRI, Indices);
	BottomMesh.AddElement(CMesh::TRI, Indices);
}

void CShearedTextileWeave2D::CalculateModifiers( const CInterpolation *pInterpolation, int Yarn, YARN_POSITION_INFORMATION YarnPosInfo, CMesh& Mesh, vector<double>& YarnSectionModifiers) const
{
	XYZ Up, Side;
	CSlaveNode Node;
	const CYarnSection* pYarnSection = m_Yarns[Yarn].GetYarnSection();
	XYZ Centre;
	//vector<double> Modifiers;

	Node = pInterpolation->GetNode(m_Yarns[Yarn].GetMasterNodes(), YarnPosInfo.iSection, YarnPosInfo.dSectionPosition);
	Up = Node.GetUp();
	Side = CrossProduct(Node.GetTangent(), Up);
	Centre = Node.GetPosition();

	int numPoints = m_Yarns[Yarn].GetNumSectionPoints();
	vector<XY> Points = pYarnSection->GetSection(YarnPosInfo, numPoints);
	//Centre = m_Yarns[Yarn].GetMasterNodes()[i].GetPosition();
	vector<XY>::iterator itPoint;
	double angle = atan2( Points[0].y, Points[0].x );
	int i = 0;

	YarnSectionModifiers.clear();
	for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint, ++i)
	{
		if ( i == 0 || i == numPoints/2 )
		{
			YarnSectionModifiers.push_back(1);  // Don't modify points at max and min x values
			continue;
		}
		XYZ P = itPoint->x * Side + itPoint->y * Up + Centre;
		XY RefPoint2D = Get2DRefPoint( *itPoint, angle );
		XYZ RefPoint = RefPoint2D.x * Side + RefPoint2D.y * Up + Centre;

		vector<pair<double, XYZ> > Intersections;
		if (Mesh.IntersectLine(RefPoint, P, Intersections, make_pair(true, false)))
		{
			double dU = Intersections[0].first;
			
			if (dU < 0)
				dU = 0;
			YarnSectionModifiers.push_back(dU);
		}
		else
			YarnSectionModifiers.push_back(1);
	}
	//YarnSectionModifiers.push_back(Modifiers);
}

XY CShearedTextileWeave2D::Get2DRefPoint( XY& Point, double dAngle ) const
{
	// Rotate original point
	XY RotatedPoint = RotatePoint( Point, -dAngle );
	// Create ref point
	XY RotRefPoint(RotatedPoint.x, 0.0);
	// Rotate back to angle of section
	XY RefPoint = RotatePoint( RotRefPoint, dAngle );

	return RefPoint;
}

void CShearedTextileWeave2D::ModifySection(CYarnSectionInterpNode* pYarnSection, YARN_POSITION_INFORMATION& YarnPosInfo, vector<double>& Modifiers, double dTargetArea, double dOriginalHeight, int iNodeIndex, int iMidIndex) const
{
	vector<XY> Points;
	vector<XY>::iterator itPoints;

	Points = pYarnSection->GetSection(YarnPosInfo, (int)Modifiers.size());
	double dAngle = atan2( Points[0].y, Points[0].x );
	for ( itPoints = Points.begin(); itPoints != Points.end(); ++itPoints )
	{
		*itPoints = RotatePoint( *itPoints, -dAngle );
	}

	// Modify any points which were outside top/bottom plane and get min/max y values
	// Set up vector of max possible y values
	double miny, maxy;
	double dHeight = 0;
	vector<XY> MaxHeight;
	double maxdy = 0.0;
	int k;
	int iModCount = 0;
	for ( itPoints = Points.begin(), k=0; itPoints != Points.end(); ++itPoints, ++k )
	{
		if ( Modifiers[k] < 1.0 )
		{
			itPoints->y = itPoints->y * Modifiers[k];
			Modifiers[k] = 1.0;
			++iModCount;
		}
		else if ( Modifiers[k] == 1.0 )
		{
			++iModCount;
		}

		if ( k == 0 )
		{
			miny = maxy = itPoints->y;
		}
		else
		{
			if ( itPoints->y < miny )
				miny = itPoints->y;
			if ( itPoints->y > maxy )
				maxy = itPoints->y;
		}

		// Set up vector of maximum possible height points
		XY Point( itPoints->x, itPoints->y );
		Point.y = itPoints->y * Modifiers[k];
		MaxHeight.push_back( Point );
		if ( fabs(Point.y - itPoints->y) > maxdy )
			maxdy = fabs(Point.y - itPoints->y);
	}

	double dArea = GetArea( MaxHeight );
	if ( dArea < dTargetArea )
	{
		// Max area available less than area of original section.  Needs changing to check volume fraction
		TGERROR("Modify Section: all points increased to max value. Unable to achieve original section area");
		// Set section to max area section
		ReplaceSection( MaxHeight, pYarnSection, iNodeIndex, iMidIndex );
		return;
	}

	dHeight = maxy - miny;

	double dy = dOriginalHeight - dHeight;
	if ( dy > maxdy )
		dy = maxdy;

	int iNumPoints = (int)Points.size();
	vector<XY> NewPoints;
	
	int iCount = ModifyPoints( Points, Modifiers, MaxHeight, iModCount, dy, NewPoints );
	
	dArea = GetArea( NewPoints );
	double mindy = 0.0;
	while ( fabs( dArea - dTargetArea ) > TOL )
	{
		if ( dArea > dTargetArea )
		{
			maxdy = dy;
		}
		else
		{
			if ( iCount == iNumPoints )  // All points increased to max value. Finish as can't increase area any more
			{							 // Add check to see if Vf ok
				TGERROR("Modify Section: all points increased to max value. Unable to achieve original section area");
				break;
			}
			mindy = dy;
		}
		dy = (maxdy + mindy)/2.0;
		
		iCount = ModifyPoints( Points, Modifiers, MaxHeight, iModCount, dy, NewPoints );
		
		dArea = GetArea( NewPoints );
	}
	
	for ( itPoints = NewPoints.begin(); itPoints != NewPoints.end(); ++itPoints )
	{
		*itPoints = RotatePoint( *itPoints, dAngle);
	}

	ReplaceSection( NewPoints, pYarnSection, iNodeIndex, iMidIndex );
}

XY CShearedTextileWeave2D::RotatePoint( XY& Point, double dAngle ) const
{
	XY RotatedPoint;
	RotatedPoint.x = Point.x*cos(dAngle) - Point.y*sin(dAngle);
	RotatedPoint.y = Point.x*sin(dAngle) + Point.y*cos(dAngle);
	return RotatedPoint;
}

double CShearedTextileWeave2D::GetArea( vector<XY>& Points ) const
{
	CSectionPolygon Section( Points );
	return Section.GetArea( Section.GetPoints(40) );
}

void CShearedTextileWeave2D::ReplaceSection( vector<XY>& Points, CYarnSectionInterpNode* pYarnSection, int iNodeIndex, int iMidIndex ) const
{
	CSectionPolygon Section(Points);
	if (m_pSectionMesh)
		Section.AssignSectionMesh(*m_pSectionMesh);
	if ( iMidIndex == -1 )
		pYarnSection->ReplaceSection( iNodeIndex, Section );
	else
		pYarnSection->ReplaceMidSection( iNodeIndex, iMidIndex, Section );
}

const vector<PATTERN2D> &CShearedTextileWeave2D::GetCellDir( int x, int y, int iDir ) const
{
	if ( iDir )
		return GetCell( y, x );
	return GetCell( x, y );
}

int CShearedTextileWeave2D::ModifyPoints( vector<XY>& Points, vector<double>& Modifiers, vector<XY>& MaxHeight, int iModCount, double dy, vector<XY>& NewPoints ) const
{
	int iCount = iModCount;
	vector<XY>::iterator itPoints;
	int k;

	NewPoints.clear();
	for ( itPoints = Points.begin(), k = 0; itPoints != Points.end(); ++itPoints, ++k )
	{
		XY Point( itPoints->x, itPoints->y );
		if ( Modifiers[k] > 1.0 )
		{
			if ( itPoints->y > 0.0 && (itPoints->y + dy) < MaxHeight[k].y )
				Point.y = itPoints->y + dy;
			else if ( itPoints->y < 0.0 && (itPoints->y - dy) > MaxHeight[k].y )
				Point.y = itPoints->y - dy;
			else
			{
				Point.y = MaxHeight[k].y;
				++iCount;
			}
		}
		NewPoints.push_back( Point );
	}
	return iCount;
}