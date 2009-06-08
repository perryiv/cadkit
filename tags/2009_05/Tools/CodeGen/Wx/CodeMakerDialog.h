/////////////////////////////////////////////////////////////////////////////
// Name:        CodeMakerDialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     01/03/06 16:26:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CODEMAKERDIALOG_H_
#define _CODEMAKERDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "CodeMakerDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10009
#define SYMBOL_CODEMAKERDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_CODEMAKERDIALOG_TITLE _("Code Maker")
#define SYMBOL_CODEMAKERDIALOG_IDNAME ID_DIALOG
#define SYMBOL_CODEMAKERDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_CODEMAKERDIALOG_POSITION wxDefaultPosition
#define ID_TEXTCTRL3 10012
#define ID_NOTEBOOK 10010
#define ID_PANEL 10011
#define ID_TEXTCTRL4 10013
#define ID_TEXTCTRL 10000
#define ID_PLUGIN_BROWSE 10001
#define ID_LISTBOX2 10016
#define ID_LISTBOX3 10017
#define ID_CREATE_PLUGIN 10018
#define ID_PANEL1 10020
#define ID_TEXTCTRL6 10021
#define ID_TEXTCTRL2 10004
#define ID_INTERFACE_BROWSE 10005
#define ID_CREATE_INTERFACE 10022
#define ID_PANEL2 10023
#define ID_TEXTCTRL7 10024
#define ID_TEXTCTRL1 10002
#define ID_BUTTON 10003
#define ID_CHECKBOX 10027
#define ID_CHECKBOX1 10028
#define ID_CHECKBOX2 10029
#define ID_CHECKBOX3 10030
#define ID_CLOSE 10019
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * CodeMakerDialog class declaration
 */

class CodeMakerDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( CodeMakerDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CodeMakerDialog( );
    CodeMakerDialog( wxWindow* parent, wxWindowID id = SYMBOL_CODEMAKERDIALOG_IDNAME, const wxString& caption = SYMBOL_CODEMAKERDIALOG_TITLE, const wxPoint& pos = SYMBOL_CODEMAKERDIALOG_POSITION, const wxSize& size = SYMBOL_CODEMAKERDIALOG_SIZE, long style = SYMBOL_CODEMAKERDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CODEMAKERDIALOG_IDNAME, const wxString& caption = SYMBOL_CODEMAKERDIALOG_TITLE, const wxPoint& pos = SYMBOL_CODEMAKERDIALOG_POSITION, const wxSize& size = SYMBOL_CODEMAKERDIALOG_SIZE, long style = SYMBOL_CODEMAKERDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin CodeMakerDialog event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLUGIN_BROWSE
    void OnPluginBrowseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CREATE_PLUGIN
    void OnCreatePluginClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_INTERFACE_BROWSE
    void OnInterfaceBrowseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CREATE_INTERFACE
    void OnCreateInterfaceClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOSE
    void OnCloseClick( wxCommandEvent& event );

////@end CodeMakerDialog event handler declarations

////@begin CodeMakerDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end CodeMakerDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
  void _initializeAvailableInterfaces();
  void _initializeAvailableLibraries();

  template < class Strings > void _selectedInterfaces ( Strings& strings );

////@begin CodeMakerDialog member variables
    wxTextCtrl* _creator;
    wxTextCtrl* _pluginName;
    wxTextCtrl* _pluginDirectory;
    wxListBox* _interfaces;
    wxTextCtrl* _interfaceName;
    wxTextCtrl* _interfaceDirectory;
////@end CodeMakerDialog member variables
};

#endif
    // _CODEMAKERDIALOG_H_
