/////////////////////////////////////////////////////////////////////////////
// Name:        plugin.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     12/30/05 23:49:52
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "plugin.h"
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

#include "plugin.h"
#include "CodeMakerDialog.h"

////@begin XPM images

////@end XPM images

/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( PluginApp )
////@end implement app

/*!
 * PluginApp type definition
 */

IMPLEMENT_CLASS( PluginApp, wxApp )

/*!
 * PluginApp event table definition
 */

BEGIN_EVENT_TABLE( PluginApp, wxApp )

////@begin PluginApp event table entries
////@end PluginApp event table entries

END_EVENT_TABLE()

/*!
 * Constructor for PluginApp
 */

PluginApp::PluginApp()
{
////@begin PluginApp member initialisation
////@end PluginApp member initialisation
}

/*!
 * Initialisation for PluginApp
 */

bool PluginApp::OnInit()
{    
////@begin PluginApp initialisation
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.

   CodeMakerDialog *d = new CodeMakerDialog ( 0x0, wxID_ANY, "Code Maker" );

    d->Show( true );

#if wxUSE_XPM
    wxImage::AddHandler( new wxXPMHandler );
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_GIF
    wxImage::AddHandler( new wxGIFHandler );
#endif
////@end PluginApp initialisation

    return true;
}

/*!
 * Cleanup for PluginApp
 */
int PluginApp::OnExit()
{    
////@begin PluginApp cleanup
    return wxApp::OnExit();
////@end PluginApp cleanup
}

