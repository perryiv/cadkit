/////////////////////////////////////////////////////////////////////////////
// Name:        CodeMakerDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     01/03/06 16:26:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "CodeMakerDialog.h"
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

#include "CodeMakerDialog.h"
#include "CodeMaker.h"

////@begin XPM images
////@end XPM images

#include "Usul/File/Find.h"
#include <vector>
#include <string>

/*!
 * CodeMakerDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CodeMakerDialog, wxDialog )

/*!
 * CodeMakerDialog event table definition
 */

BEGIN_EVENT_TABLE( CodeMakerDialog, wxDialog )

////@begin CodeMakerDialog event table entries
    EVT_CLOSE( CodeMakerDialog::OnCloseWindow )

    EVT_BUTTON( ID_PLUGIN_BROWSE, CodeMakerDialog::OnPluginBrowseClick )

    EVT_BUTTON( ID_CREATE_PLUGIN, CodeMakerDialog::OnCreatePluginClick )

    EVT_BUTTON( ID_INTERFACE_BROWSE, CodeMakerDialog::OnInterfaceBrowseClick )

    EVT_BUTTON( ID_CREATE_INTERFACE, CodeMakerDialog::OnCreateInterfaceClick )

    EVT_BUTTON( ID_CLOSE, CodeMakerDialog::OnCloseClick )

////@end CodeMakerDialog event table entries

END_EVENT_TABLE()

/*!
 * CodeMakerDialog constructors
 */

CodeMakerDialog::CodeMakerDialog( )
{
}

CodeMakerDialog::CodeMakerDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * CodeMakerDialog creator
 */

bool CodeMakerDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin CodeMakerDialog member initialisation
    _creator = NULL;
    _pluginName = NULL;
    _pluginDirectory = NULL;
    _interfaces = NULL;
    _interfaceName = NULL;
    _interfaceDirectory = NULL;
////@end CodeMakerDialog member initialisation

////@begin CodeMakerDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end CodeMakerDialog creation
    return TRUE;
}

/*!
 * Control creation for CodeMakerDialog
 */

void CodeMakerDialog::CreateControls()
{    
////@begin CodeMakerDialog content construction
    CodeMakerDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, _("Code Maker"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Creator"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    _creator = new wxTextCtrl( itemDialog1, ID_TEXTCTRL3, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(_creator, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxNotebook* itemNotebook7 = new wxNotebook( itemDialog1, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

    wxPanel* itemPanel8 = new wxPanel( itemNotebook7, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemPanel8->SetSizer(itemBoxSizer9);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel8, wxID_STATIC, _("Plugin Generator"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxGridSizer* itemGridSizer11 = new wxGridSizer(2, 2, 0, 0);
    itemBoxSizer9->Add(itemGridSizer11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText12 = new wxStaticText( itemPanel8, wxID_STATIC, _("Plugin Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    _pluginName = new wxTextCtrl( itemPanel8, ID_TEXTCTRL4, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer11->Add(_pluginName, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemPanel8, wxID_STATIC, _("Directory"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer11->Add(itemStaticText14, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemGridSizer11->Add(itemBoxSizer15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    _pluginDirectory = new wxTextCtrl( itemPanel8, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(300, -1), 0 );
    itemBoxSizer15->Add(_pluginDirectory, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton17 = new wxButton( itemPanel8, ID_PLUGIN_BROWSE, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemButton17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText18 = new wxStaticText( itemPanel8, wxID_STATIC, _("Interfaces"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString* _interfacesStrings = NULL;
    _interfaces = new wxListBox( itemPanel8, ID_LISTBOX2, wxDefaultPosition, wxDefaultSize, 0, _interfacesStrings, wxLB_EXTENDED );
    itemBoxSizer9->Add(_interfaces, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemPanel8, wxID_STATIC, _("Libraries"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText20, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString* itemListBox21Strings = NULL;
    wxListBox* itemListBox21 = new wxListBox( itemPanel8, ID_LISTBOX3, wxDefaultPosition, wxDefaultSize, 0, itemListBox21Strings, wxLB_EXTENDED );
    itemBoxSizer9->Add(itemListBox21, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton22 = new wxButton( itemPanel8, ID_CREATE_PLUGIN, _("Create"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton22, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemNotebook7->AddPage(itemPanel8, _("Plugin"));

    wxPanel* itemPanel23 = new wxPanel( itemNotebook7, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxStaticBox* itemStaticBoxSizer24Static = new wxStaticBox(itemPanel23, wxID_ANY, _("Interface Generator"));
    wxStaticBoxSizer* itemStaticBoxSizer24 = new wxStaticBoxSizer(itemStaticBoxSizer24Static, wxVERTICAL);
    itemPanel23->SetSizer(itemStaticBoxSizer24);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer24->Add(itemBoxSizer25, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText26 = new wxStaticText( itemPanel23, wxID_STATIC, _("Interface Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    _interfaceName = new wxTextCtrl( itemPanel23, ID_TEXTCTRL6, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(_interfaceName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer24->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText29 = new wxStaticText( itemPanel23, wxID_STATIC, _("Directory"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    _interfaceDirectory = new wxTextCtrl( itemPanel23, ID_TEXTCTRL2, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(_interfaceDirectory, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton31 = new wxButton( itemPanel23, ID_INTERFACE_BROWSE, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemButton31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton32 = new wxButton( itemPanel23, ID_CREATE_INTERFACE, _("Create"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer24->Add(itemButton32, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemNotebook7->AddPage(itemPanel23, _("Interface"));

    wxPanel* itemPanel33 = new wxPanel( itemNotebook7, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel33->Show(FALSE);
    wxStaticBox* itemStaticBoxSizer34Static = new wxStaticBox(itemPanel33, wxID_ANY, _("Class Generator"));
    wxStaticBoxSizer* itemStaticBoxSizer34 = new wxStaticBoxSizer(itemStaticBoxSizer34Static, wxVERTICAL);
    itemPanel33->SetSizer(itemStaticBoxSizer34);

    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer34->Add(itemBoxSizer35, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText36 = new wxStaticText( itemPanel33, wxID_STATIC, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer35->Add(itemStaticText36, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl37 = new wxTextCtrl( itemPanel33, ID_TEXTCTRL7, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer35->Add(itemTextCtrl37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer34->Add(itemBoxSizer38, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText39 = new wxStaticText( itemPanel33, wxID_STATIC, _("Directory"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemStaticText39, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl40 = new wxTextCtrl( itemPanel33, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemTextCtrl40, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton41 = new wxButton( itemPanel33, ID_BUTTON, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemButton41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer42 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer34->Add(itemBoxSizer42, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxCheckBox* itemCheckBox43 = new wxCheckBox( itemPanel33, ID_CHECKBOX, _("Singleton"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBox43->SetValue(FALSE);
    itemBoxSizer42->Add(itemCheckBox43, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* itemCheckBox44 = new wxCheckBox( itemPanel33, ID_CHECKBOX1, _("Copy Ctor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBox44->SetValue(FALSE);
    itemBoxSizer42->Add(itemCheckBox44, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* itemCheckBox45 = new wxCheckBox( itemPanel33, ID_CHECKBOX2, _("Default Ctor"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBox45->SetValue(FALSE);
    itemBoxSizer42->Add(itemCheckBox45, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* itemCheckBox46 = new wxCheckBox( itemPanel33, ID_CHECKBOX3, _("Ref Counted"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemCheckBox46->SetValue(FALSE);
    itemBoxSizer42->Add(itemCheckBox46, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook7->AddPage(itemPanel33, _("Class"));

    itemBoxSizer2->Add(itemNotebook7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton47 = new wxButton( itemDialog1, ID_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemButton47, 0, wxALIGN_RIGHT|wxALL, 5);

////@end CodeMakerDialog content construction

    this->_initializeAvailableInterfaces();

    char *buf ( ::getenv ( "USUL_INC_DIR" ) );

    if( ::strlen ( buf ) != 0 )
    {
      std::string plugdir ( buf );
      plugdir += "/OsgFox/Plugins";

      _pluginDirectory->SetValue ( plugdir.c_str() );

      std::string idir ( buf );
      idir += "/Usul/Interfaces";

      _interfaceDirectory->SetValue ( idir.c_str() );
    }
}

/*!
 * Should we show tooltips?
 */

bool CodeMakerDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap CodeMakerDialog::GetBitmapResource( const wxString& )
{
    // Bitmap retrieval
////@begin CodeMakerDialog bitmap retrieval
    return wxNullBitmap;
////@end CodeMakerDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CodeMakerDialog::GetIconResource( const wxString& )
{
    // Icon retrieval
////@begin CodeMakerDialog icon retrieval
    return wxNullIcon;
////@end CodeMakerDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CREATE_INTERFACE
 */

void CodeMakerDialog::OnCreateInterfaceClick( wxCommandEvent& )
{
  std::string interfaceName ( _interfaceName->GetValue() );
  if ( !interfaceName.empty() )
  {
    std::string creator ( _creator->GetValue() );

    std::string errors;
    CodeMaker::instance().createInterface ( creator, interfaceName, errors );

    if ( !errors.empty() )
    {
      wxMessageDialog dialog ( this, errors.c_str(), "Error", wxOK );
      dialog.ShowModal();
    }

    // Re-populate the interface list.
    this->_initializeAvailableInterfaces();

    _interfaceName->SetValue( "" );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CREATE_PLUGIN
 */

void CodeMakerDialog::OnCreatePluginClick( wxCommandEvent& )
{
  std::string pluginName ( _pluginName->GetValue() );

  std::string directory ( _pluginDirectory->GetValue() );
  if ( !pluginName.empty() && !directory.empty() )
  {
    std::string creator ( _creator->GetValue() );

    CodeMaker::StringArray array;
    this->_selectedInterfaces ( array );

    std::string errors;
    CodeMaker::instance().createPlugin ( creator, pluginName, directory, array, errors );

    if ( !errors.empty() )
    {
      wxMessageDialog dialog ( this, errors.c_str(), "Error", wxOK );
      dialog.ShowModal();
    }
    
    _pluginName->SetValue( "" );
  }
}


/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
 */

void CodeMakerDialog::OnCloseWindow( wxCloseEvent& )
{
  wxApp::GetInstance()->Exit();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOSE
 */

void CodeMakerDialog::OnCloseClick( wxCommandEvent&  )
{
  wxApp::GetInstance()->Exit();
}


void CodeMakerDialog::_initializeAvailableInterfaces()
{
  wxArrayString selections;
  this->_selectedInterfaces ( selections );

  _interfaces->Clear();

  typedef std::vector < std::string > Strings;
  Strings interfaces;

  char *buf ( ::getenv ( "USUL_INC_DIR" ) );

  if( ::strlen ( buf ) == 0 )
    return;

  std::string usuldir ( buf );
  usuldir += "/Usul/Interfaces";

  Usul::File::find ( usuldir, "h", interfaces );

  wxArrayString array;
  for ( Strings::const_iterator iter = interfaces.begin(); iter != interfaces.end(); ++iter )
  {
    array.Add ( iter->c_str() );
  }

  _interfaces->InsertItems ( array, 0 );

  for ( wxArrayString::iterator iter = selections.begin(); iter != selections.end(); ++iter )
  {
    _interfaces->SetStringSelection( *iter );
  }
}


void CodeMakerDialog::_initializeAvailableLibraries()
{
}

template < class Strings > void CodeMakerDialog::_selectedInterfaces ( Strings& strings )
{
  wxArrayInt selections;
  _interfaces->GetSelections ( selections );

  for ( wxArrayInt::iterator i = selections.begin(); i != selections.end(); ++i )
  {
    strings.push_back ( _interfaces->GetString ( *i ).c_str() );
  }
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void CodeMakerDialog::OnPluginBrowseClick( wxCommandEvent& )
{
  wxDirDialog dialog ( this );

  dialog.SetPath ( _pluginDirectory->GetValue() );

  dialog.ShowModal();

  _pluginDirectory->SetValue ( dialog.GetPath() );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_INTERFACE_BROWSE
 */

void CodeMakerDialog::OnInterfaceBrowseClick( wxCommandEvent& )
{
  wxDirDialog dialog ( this );

  dialog.SetPath ( _interfaceDirectory->GetValue() );

  dialog.ShowModal();

  _interfaceDirectory->SetValue ( dialog.GetPath() );
}



