/////////////////////////////////////////////////////////////////////////////
// Name:        plugin.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     01/03/06 17:59:44
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "plugin.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
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
////@end control identifiers

/*!
 * PluginApp class declaration
 */

class PluginApp: public wxApp
{    
    DECLARE_CLASS( PluginApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    PluginApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin PluginApp event handler declarations
////@end PluginApp event handler declarations

////@begin PluginApp member function declarations
////@end PluginApp member function declarations

////@begin PluginApp member variables
////@end PluginApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(PluginApp)
////@end declare app

#endif
    // _PLUGIN_H_
