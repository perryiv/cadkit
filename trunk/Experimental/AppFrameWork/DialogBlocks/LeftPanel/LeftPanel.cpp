/////////////////////////////////////////////////////////////////////////////
// Name:        LeftPanel.cpp
// Purpose:     
// Author:      Perry Miller
// Modified by: 
// Created:     12/16/05 23:33:29
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 12/16/05 23:33:29

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "LeftPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "LeftPanel.h"

////@begin XPM images
////@end XPM images

/*!
 * LeftPanel type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LeftPanel, wxPanel )

/*!
 * LeftPanel event table definition
 */

BEGIN_EVENT_TABLE( LeftPanel, wxPanel )

////@begin LeftPanel event table entries
////@end LeftPanel event table entries

END_EVENT_TABLE()

/*!
 * LeftPanel constructors
 */

LeftPanel::LeftPanel( )
{
}

LeftPanel::LeftPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, pos, size, style);
}

/*!
 * LeftPanel creator
 */

bool LeftPanel::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LeftPanel member initialisation
////@end LeftPanel member initialisation

////@begin LeftPanel creation
    SetParent(parent);
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end LeftPanel creation
    return true;
}

/*!
 * Control creation for LeftPanel
 */

void LeftPanel::CreateControls()
{    
////@begin LeftPanel content construction
    // Generated by DialogBlocks, 12/17/05 02:21:25 (unregistered)

    wxXmlResource::Get()->LoadPanel(this, GetParent(), _T("ID_LEFT_PANEL"));
////@end LeftPanel content construction

    // Create custom windows not generated automatically here.

////@begin LeftPanel content initialisation

////@end LeftPanel content initialisation
}

/*!
 * Should we show tooltips?
 */

bool LeftPanel::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LeftPanel::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LeftPanel bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LeftPanel bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LeftPanel::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LeftPanel icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LeftPanel icon retrieval
}
