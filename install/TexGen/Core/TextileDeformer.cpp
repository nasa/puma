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
#include "TextileDeformer.h"
#include "TexGen.h"

using namespace TexGen;

CTextileDeformer::CTextileDeformer(void)
{
	m_RepeatDeformation.InitialiseIdentity(3);
}

CTextileDeformer::~CTextileDeformer(void)
{
}

void CTextileDeformer::DeformTextile(CTextile &Textile, bool bDeformDomain)
{
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
		return;
	int i, j;
	CYarn* pYarn;
//	const CNode *pNode;
	CNode NewNode;
	XYZ Pos, Disp, BestDisp;
	XYZ NodeDisp, Adjust;
	double dAccuracy, dBestAccuracy;
	vector<CSlaveNode>::const_iterator itNode;
	vector<XYZ> Translations;
	vector<XYZ>::iterator itTranslation;
//	vector<XYZ>::const_iterator itSecPt;
//	vector<XY>::const_iterator itSecPt2D;
//	double dX;
//	XYZ Side;
	for (i=0; i<Textile.GetNumYarns(); ++i)
	{
		pYarn = Textile.GetYarn(i);

		// Deform the textile path and cross-sections
		CInterpolationAdjusted AdjustedInterp(*pYarn->GetInterpolation());
		CYarnSectionAdjusted AdjustedYarnSection(*pYarn->GetYarnSection());
		Translations = pDomain->GetTranslations(*pYarn);
		const vector<CSlaveNode> &Nodes = pYarn->GetSlaveNodes(CYarn::SURFACE);
		for (itNode = Nodes.begin(); itNode != Nodes.end(); ++itNode)
		{
			// Adjust center-line
			for (itTranslation = Translations.begin(); itTranslation != Translations.end(); ++itTranslation)
			{
				dAccuracy = GetDisplacement(itNode->GetPosition()+*itTranslation, i, NodeDisp);
				if (itTranslation == Translations.begin() || dAccuracy>dBestAccuracy)
				{
					Adjust = m_RepeatDeformation * (*itTranslation) - *itTranslation;
					dBestAccuracy = dAccuracy;
					BestDisp = NodeDisp-Adjust;
				}
			}
			NodeDisp = BestDisp;
			AdjustedInterp.AddAdjustment(itNode->GetIndex(), itNode->GetT(), NodeDisp);

			// Adjust yarn twist
			vector<pair<double, XY> > SectionAdjust;
			const vector<XYZ> &SectionPoints = itNode->GetSectionPoints();
			const int iSectionPoints = SectionPoints.size();
			for (j=0; j<iSectionPoints; ++j)
			{
				double dU = j/double(iSectionPoints);
				Pos = SectionPoints[j];
				dAccuracy = 0;
				for (itTranslation = Translations.begin(); itTranslation != Translations.end(); ++itTranslation)
				{
					dAccuracy = GetDisplacement(Pos+*itTranslation, i, Disp);
					if (itTranslation == Translations.begin() || dAccuracy>dBestAccuracy)
					{
						Adjust = m_RepeatDeformation * (*itTranslation) - *itTranslation;
						dBestAccuracy = dAccuracy;
						BestDisp = Disp-Adjust;
					}
				}
				Disp = BestDisp;
				// Subtract the node displacement otherwise we are adjusting twice for node
				// displacement
				Disp -= NodeDisp;
				XY PlaneDisp;
				PlaneDisp.x = DotProduct(itNode->GetSide(), Disp);
				PlaneDisp.y = DotProduct(itNode->GetUp(), Disp);
				SectionAdjust.push_back(make_pair(dU, PlaneDisp));
			}
			if (!SectionAdjust.empty())
			{
				// Add the first displacement back to the end of the list with dU = 1
				SectionAdjust.push_back(SectionAdjust.front());
				SectionAdjust.back().first = 1.0;
				AdjustedYarnSection.AddAdjustment(itNode->GetIndex(), itNode->GetT(), SectionAdjust);
			}
		}
		pYarn->AssignInterpolation(AdjustedInterp);
		pYarn->AssignSection(AdjustedYarnSection);

		// Adjust the repeat vectors
		vector<XYZ> Repeats = pYarn->GetRepeats();
		vector<XYZ>::iterator itRepeat;
		for (itRepeat = Repeats.begin(); itRepeat != Repeats.end(); ++itRepeat)
		{
			*itRepeat = m_RepeatDeformation * (*itRepeat);
		}
		pYarn->SetRepeats(Repeats);
	}
	if (bDeformDomain)
	{
		CDomain* pDomain = Textile.GetDomain();
		if (pDomain)
		{
			pDomain->Deform(m_RepeatDeformation);
		}
	}
}

CTextile* CTextileDeformer::GetDeformedCopyOfTextile(CTextile &Textile, bool bDeformDomain)
{
	CTextile* pCopy = Textile.Copy();
	DeformTextile(*pCopy, bDeformDomain);
	string Name = TEXGEN.AddTextile(*pCopy);
	delete pCopy;
	return TEXGEN.GetTextile(Name);
}











