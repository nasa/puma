/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2014 Louise Brown
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
#include "PatternDraftDialog.h"
#include "WindowIDs.h"
#include "TexGenApp.h"

#define PRINT_GAP  2

BEGIN_EVENT_TABLE(CPatternDraftDialog, wxDialog)
	//EVT_INIT_DIALOG(CTextileLayersDialog::OnInit)
	//EVT_PAINT(CPatternDraftDialog::OnPaint)
	EVT_BUTTON(wxID_PRINT, CPatternDraftDialog::OnPrint)
	EVT_BUTTON(wxID_OK, CPatternDraftDialog::OnOK)	
	EVT_BUTTON(wxID_SAVE, CPatternDraftDialog::OnSave)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CPatternDraftPainter, wxEvtHandler)
	EVT_PAINT(CPatternDraftPainter::OnPaint)
	//EVT_BUTTON(wxID_PRINT, CPatternDraftPainter::OnPrint)
	//EVT_SIZE(CPatternDraftPainter::OnSize)
END_EVENT_TABLE()

CPatternDraftDialog::CPatternDraftDialog( )
{
	BuildDialog();
}

CPatternDraftDialog::CPatternDraftDialog( CPatternDraft& PatternDraft )
: m_PatternDraft( PatternDraft )
{
	BuildDialog();
}

CPatternDraftDialog::~CPatternDraftDialog(void)
{
	m_pPatternPanel->PopEventHandler( true );
}

void CPatternDraftDialog::BuildDialog()
{
	if (wxXmlResource::Get()->LoadDialog(this, ((CTexGenApp*)wxTheApp)->GetMainFrame(), wxT("PatternDraft")))
	{
		m_pPatternPanel = XRCCTRL(*this, "WeavePattern", wxPanel);
		m_pPatternPanel->PushEventHandler( new CPatternDraftPainter(*this));
	}
}

void CPatternDraftDialog::OnOK(wxCommandEvent& event)
{
	if (!Validate())
		return;
	if (!TransferDataFromWindow())
		return;
	
	if (IsModal())
	{
		EndModal(wxID_OK);
	}
	else
	{
		SetReturnCode(wxID_OK);
		this->Show(false);
	}
}

void CPatternDraftDialog::DrawPatternDraft(wxDC &dc)
{
	wxRect Rect = m_pPatternPanel->GetClientRect();
	CalculatePixelRatio( Rect );
	
	dc.SetBackground(wxBrush(*wxWHITE_BRUSH));
	dc.Clear();
	
	DrawWeaveRegion( dc, Rect );
	DrawHeddleRegion( dc, Rect );
	DrawTieUpRegion( dc, Rect );
	DrawChainRegion( dc, Rect );
}

void CPatternDraftDialog::PrintPatternDraft( wxDC &dc, wxRect& Rect )
{
	CalculatePixelRatio( Rect );
	
	dc.SetBackground(wxBrush(*wxWHITE_BRUSH));
	dc.Clear();
	
	DrawWeaveRegion( dc, Rect );
	DrawHeddleRegion( dc, Rect );
	DrawTieUpRegion( dc, Rect );
	DrawChainRegion( dc, Rect );
}

void CPatternDraftDialog::CalculatePixelRatio( wxRect& Rect )
{
	int iNumHeddles = m_PatternDraft.GetNumHeddles();

	int Width = m_PatternDraft.GetNumWarps() + iNumHeddles + PRINT_GAP;  
	int Height = m_PatternDraft.GetNumWefts() + iNumHeddles + PRINT_GAP;
	
	m_iPixelRatio = min(Rect.width/Width, Rect.height/Height);
	m_iFreeWidth = Rect.width - m_iPixelRatio*Width;
	m_iFreeHeight = Rect.height - m_iPixelRatio*Height;
}

void CPatternDraftDialog::DrawWeaveRegion( wxDC &dc, wxRect& ClientRect )
{
	wxRect Rect = ClientRect;
	Rect.width = m_PatternDraft.GetNumWarps() * m_iPixelRatio;
	Rect.height = m_PatternDraft.GetNumWefts() * m_iPixelRatio;

	Rect.x += m_iFreeWidth/2;
	Rect.y += m_iFreeHeight/2;

	dc.SetPen(*wxTRANSPARENT_PEN);
	const vector<string>& WeavePattern = m_PatternDraft.GetWeavePattern();
	vector<string>::const_reverse_iterator itWeavePattern;
	
	int j;
	wxPoint BottomLeft = Rect.GetTopLeft();  // Top left because reversed y axis orientation for dc
	for ( itWeavePattern = WeavePattern.rbegin(), j = 0; itWeavePattern != WeavePattern.rend(); ++itWeavePattern, ++j )
	{
		for ( int i = 0; i < itWeavePattern->size(); ++i )
		{
			if ( (*itWeavePattern)[i] == '0' )
				continue;
			if ( (*itWeavePattern)[i] == '1' )
			{
				dc.SetBrush( wxBrush( *wxBLACK, wxSOLID ) );
			}
			else if ( (*itWeavePattern)[i] == '2' )
			{
				dc.SetBrush( wxBrush( *wxGREEN, wxSOLID ) );
			}
			DrawSquare( dc, BottomLeft, i, j );
		}
	}
	dc.SetPen( wxPen(*wxBLACK) );
	DrawGrid( dc, Rect, m_PatternDraft.GetNumWarps(), m_PatternDraft.GetNumWefts() );
}

void CPatternDraftDialog::DrawGrid( wxDC &dc, wxRect &Rect, int iNumX, int iNumY )
{
	int x = Rect.x;
	int y2 = Rect.y + Rect.height;

	for ( int i = 0; i <= iNumX; ++i )
	{
		dc.DrawLine( x, Rect.y, x, y2 );
		x += m_iPixelRatio;
	}

	int x2 = Rect.x + Rect.width;
	int y = Rect.y;
	for ( int i = 0; i <= iNumY; ++i )
	{
		dc.DrawLine( Rect.x, y, x2, y );
		y += m_iPixelRatio;
	}
}

void CPatternDraftDialog::DrawSquare( wxDC &dc, wxPoint &BottomLeft, int i, int j )
{
	
	dc.DrawRectangle( BottomLeft.x + i*m_iPixelRatio, BottomLeft.y + j*m_iPixelRatio, m_iPixelRatio, m_iPixelRatio );
}

void CPatternDraftDialog::DrawHeddleRegion( wxDC &dc, wxRect& ClientRect)
{
	wxRect Rect = ClientRect;

	Rect.width = m_PatternDraft.GetNumWarps() * m_iPixelRatio;
	Rect.height = m_PatternDraft.GetNumHeddles() * m_iPixelRatio;

	Rect.x += m_iFreeWidth/2;
	Rect.y = m_iFreeHeight/2 + (m_PatternDraft.GetNumWefts() + PRINT_GAP) * m_iPixelRatio;

	//Rect.y = ClientRect.height - Rect.height-1 - m_iFreeHeight/2;

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush( wxBrush( *wxBLACK, wxSOLID ) );
	wxPoint BottomLeft = Rect.GetTopLeft();  // Top left because reversed y axis orientation for dc

	const vector<int>& HeddleDraft = m_PatternDraft.GetHeddleDraft();
	for ( int i = 0; i < HeddleDraft.size(); ++i )
	{
		DrawSquare( dc, BottomLeft, i, HeddleDraft[i] );
	}
	dc.SetPen( wxPen(*wxBLACK) );
	DrawGrid( dc, Rect, m_PatternDraft.GetNumWarps(), m_PatternDraft.GetNumHeddles() );
}

void CPatternDraftDialog::DrawTieUpRegion( wxDC &dc, wxRect& ClientRect )
{
	wxRect Rect = ClientRect;

	int iNumHeddles = m_PatternDraft.GetNumHeddles();
	Rect.width = iNumHeddles * m_iPixelRatio;
	Rect.height = Rect.width;

	Rect.y = m_iFreeHeight/2 + (m_PatternDraft.GetNumWefts() + PRINT_GAP) * m_iPixelRatio;
	//Rect.y = ClientRect.height - Rect.height-1 - m_iFreeHeight/2;
	//Rect.x = ClientRect.width - Rect.width-1 - m_iFreeWidth/2;
	Rect.x = m_iFreeWidth/2 + (m_PatternDraft.GetNumWarps() + PRINT_GAP) * m_iPixelRatio;

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush( wxBrush( *wxBLACK, wxSOLID ) );
	wxPoint BottomLeft = Rect.GetTopLeft();  // Top left because reversed y axis orientation for dc

	for ( int i = 0; i < iNumHeddles; ++i )
	{
		DrawSquare( dc, BottomLeft, i, i );
	}
	dc.SetPen( wxPen(*wxBLACK) );

	DrawGrid( dc, Rect, iNumHeddles, iNumHeddles );
}

void CPatternDraftDialog::DrawChainRegion( wxDC &dc, wxRect& ClientRect )
{
	wxRect Rect = ClientRect;

	int Width = m_PatternDraft.GetNumHeddles();
	int Height = m_PatternDraft.GetNumWefts();
	Rect.width = Width * m_iPixelRatio;
	Rect.height =  Height * m_iPixelRatio;

	Rect.y += m_iFreeHeight/2;
	//Rect.x = ClientRect.width - Rect.width-1 - m_iFreeWidth/2;
	Rect.x = m_iFreeWidth/2 + (m_PatternDraft.GetNumWarps() + PRINT_GAP) * m_iPixelRatio;

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush( wxBrush( *wxBLACK, wxSOLID ) );
	wxPoint BottomLeft = Rect.GetTopLeft();  // Top left because reversed y axis orientation for dc

	const vector<bool>& ChainDraft = m_PatternDraft.GetChainDraft();
	for ( int i = 0; i < ChainDraft.size(); ++i )
	{
		if ( ChainDraft[i] )
			DrawSquare( dc, BottomLeft, i%Width, Height-1-(i/Width) );
	}
	dc.SetPen( wxPen(*wxBLACK) );

	DrawGrid( dc, Rect, m_PatternDraft.GetNumHeddles(), m_PatternDraft.GetNumWefts() );
}

void CPatternDraftDialog::OnPrint( wxCommandEvent& event )
{
	CPatternDraftPrintout* myPrint = new CPatternDraftPrintout( 1, wxT("Pattern Draft"), 30, *this );
	if ( !myPrint->PerformPageSetup(true) )
	{
		delete myPrint;
		return;
	}
	wxPrintDialogData data(myPrint->GetPrintData());
    wxPrinter printer(&data);
    const bool success = printer.Print(NULL, myPrint, true /* show dialog */);
 
    delete myPrint;
}

void CPatternDraftDialog::OnSave( wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Save Pattern Draft file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("text file (*.txt)|*.txt"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		m_PatternDraft.Output( ConvertString(dialog.GetPath()) );
	}
}

void CPatternDraftPainter::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(dynamic_cast<wxPanel*>(event.GetEventObject()));

	dc.SetAxisOrientation(true, true);
	dc.SetDeviceOrigin(0, dc.GetSize().GetHeight()-1 );

	m_PatternDraftDialog.DrawPatternDraft(dc);
}

/*void CPatternDraftPainter::OnSize(wxSizeEvent& event)
{
}*/

CPatternDraftPrintout::CPatternDraftPrintout( int page_amount, wxString title, float units_per_cm, CPatternDraftDialog &PatternDraftDialog)
: wxPrintout( title )
, m_PatternDraftDialog( PatternDraftDialog )
{
    m_page_amount = page_amount;

    m_orient = wxPORTRAIT; // wxPORTRAIT, wxLANDSCAPE
    m_paper_type = wxPAPER_A4;
    m_margin_left   = 16;
    m_margin_right  = 16;
    m_margin_top    = 32;
    m_margin_bottom = 32;

    m_units_per_cm   = units_per_cm;
}

/** shows the page setup dialog, OR sets up defaults */
bool CPatternDraftPrintout::PerformPageSetup(const bool showPageSetupDialog)
{
    // don't show page setup dialog, use default values
    wxPrintData printdata;
    printdata.SetPrintMode( wxPRINT_MODE_PRINTER );
    printdata.SetOrientation( m_orient );
    printdata.SetNoCopies(1);
    printdata.SetPaperId( m_paper_type ); 

    m_page_setup = wxPageSetupDialogData(printdata);
    m_page_setup.SetMarginTopLeft    (wxPoint(m_margin_left,  m_margin_top));
    m_page_setup.SetMarginBottomRight(wxPoint(m_margin_right, m_margin_bottom)); 

    if (showPageSetupDialog)
    {
        wxPageSetupDialog dialog( NULL, &m_page_setup );
        if (dialog.ShowModal() == wxID_OK)
        {

            m_page_setup = dialog.GetPageSetupData();
            m_orient = m_page_setup.GetPrintData().GetOrientation();
            m_paper_type = m_page_setup.GetPrintData().GetPaperId();

            wxPoint marginTopLeft = m_page_setup.GetMarginTopLeft();
            wxPoint marginBottomRight = m_page_setup.GetMarginBottomRight();
            m_margin_left   = marginTopLeft.x;
            m_margin_right  = marginBottomRight.x;
            m_margin_top    = marginTopLeft.y;
            m_margin_bottom = marginBottomRight.y;
        }
        else
        {
            std::cout << "user canceled at first dialog" << std::endl;
            return false;
        }
    }
    return true;
}

/** Called when printing starts */
void CPatternDraftPrintout::OnBeginPrinting()
{
    // set-up coordinate system however we want, we'll use it when drawing

    // take paper size and margin sizes into account when setting up coordinate system
    // so that units are "square" (1 unit x is a wide as 1 unit y is high)
    // (actually, if we don't make it square, on some platforms wx will even resize your
    //  margins to make it so)
    wxSize paperSize = m_page_setup.GetPaperSize();  // in millimeters

    // still in millimeters
    float large_side = std::max(paperSize.GetWidth(), paperSize.GetHeight());
    float small_side = std::min(paperSize.GetWidth(), paperSize.GetHeight());

    float large_side_cm = large_side / 10.0f;  // in centimeters
    float small_side_cm = small_side / 10.0f;  // in centimeters

    if (m_orient == wxPORTRAIT)
    {
        float ratio = float(large_side - m_margin_top  - m_margin_bottom) /
                      float(small_side - m_margin_left - m_margin_right);

        m_coord_system_width  = (int)((small_side_cm - m_margin_left/10.f -
                                       m_margin_right/10.0f)*m_units_per_cm);
        m_coord_system_height = m_coord_system_width*ratio;
    }
    else
    {
        float ratio = float(large_side - m_margin_left - m_margin_right) /
                      float(small_side - m_margin_top  - m_margin_bottom);

        m_coord_system_height = (int)((small_side_cm - m_margin_top/10.0f -
                                       m_margin_bottom/10.0f)* m_units_per_cm);
        m_coord_system_width  = m_coord_system_height*ratio;

    }

}

/** returns the data obtained from the page setup dialog (or the defaults,
 * if dialog was not shown) */
wxPrintData CPatternDraftPrintout::GetPrintData()
{
    return m_page_setup.GetPrintData();
}

/** Called when starting to print a document */
bool CPatternDraftPrintout::OnBeginDocument(int startPage, int endPage)
{
    return wxPrintout::OnBeginDocument(startPage, endPage);
}

/** wx will call this to know how many pages can be printed */
void CPatternDraftPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageSelFrom, int *pageSelTo)
{
    *minPage = 1;
    *maxPage = m_page_amount;

    *pageSelFrom = 1;
    *pageSelTo = m_page_amount;
}

/** called by wx to know what pages this document has */
bool CPatternDraftPrintout::HasPage(int pageNum)
{
    // wx will call this to know how many pages can be printed
    return pageNum >= 1 && pageNum <= m_page_amount;
}


/** called by wx everytime it's time to render a specific page onto the
 * printing device context */
bool CPatternDraftPrintout::OnPrintPage(int pageNum)
{
    // ---- setup DC with coordinate system ----
    FitThisSizeToPageMargins(wxSize(m_coord_system_width, m_coord_system_height), m_page_setup);

    wxRect fitRect = GetLogicalPageMarginsRect(m_page_setup);

    wxCoord xoff = (fitRect.width - m_coord_system_width) / 2;
    wxCoord yoff = (fitRect.height - m_coord_system_height) / 2 + m_coord_system_height;  // yoff so that origin is set to bottom left of page
    OffsetLogicalOrigin(xoff, yoff);
	

    wxDC* ptr = GetDC();
    if (ptr==NULL || !ptr->IsOk())
    {
        TGERROR("DC is not Ok, interrupting printing");
        return false;
    }
    wxDC& dc = *ptr;
	dc.SetAxisOrientation(true, true);  // Set axes to increase up and to right
	

    // ---- A couple helper variables to help us during draw within paper area -----
    const int x0 = 0;
    const int y0 = 0;
    const int width = m_coord_system_width;
    const int height = m_coord_system_height;

    // ---- Draw to the print DC ----
    dc.Clear();

    dc.SetPen(  wxPen( wxColour(0,0,0) ) );
    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    // draw a rectangle to show its bounds.
    dc.DrawRectangle(x0, y0, width, height);
	
	// Print pattern draft data
	wxRect Rect( 0, 0, width, height );
	m_PatternDraftDialog.PrintPatternDraft( dc, Rect );

    return true;
}  

void CPatternDraftPrintout::OnEndPrinting()
{
}