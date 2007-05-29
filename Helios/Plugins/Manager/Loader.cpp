
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Loader for plugin files.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/Manager/Loader.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Trace/Trace.h"

#include "XmlTree/Document.h"

using namespace CadKit::Helios::Plugins::Manager;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Loader::Loader() : 
	_filename(),
	_names(),
  _mutex()
{
  USUL_TRACE_SCOPE;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Loader::~Loader()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filename to parse.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::filename ( const std::string& filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
	_filename = filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename to parse.
//
///////////////////////////////////////////////////////////////////////////////

std::string Loader::filename() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
	return _filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::parse()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  XmlTree::Document::RefPtr doc = new XmlTree::Document ( );
	doc->load ( _filename );
	
	typedef XmlTree::Document::Children Children;
	
	Children& children ( doc->children() );
	
	for( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
	{
		XmlTree::Node::RefPtr node ( (*iter) );
		if( "plugins" == node->name() )
		{
			this->_addPlugins( *node );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a node.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::_addPlugins ( XmlTree::Node &parent )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typedef XmlTree::Document::Children Children;
	Children& children ( parent.children() );
	
	for( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
	{
		XmlTree::Node::RefPtr node ( (*iter) );
		if( "plugin" == node->name() )
		{
			this->_addPlugin ( *node );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::_addPlugin ( XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typedef XmlTree::Document::Attributes Attributes;
	Attributes& attributes ( node.attributes() );
	
	std::string file ( "" );
  bool load ( true );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
  	if ( "file" == iter->first )
    {
  	  file = iter->second;
    }
    else if ( "load" == iter->first )
    {
    	std::istringstream in ( iter->second );
    	in >> load;
   	}
  }
  
  this->addPlugin ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::addPlugin ( const std::string &file )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
 
  if ( false == file.empty() )
  {
  	_names.insert ( Names::value_type ( file, true ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::load ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Query for interfaces.
	Usul::Interfaces::IStatusBar::UpdateStatusBar status ( caller );
	Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );
	
	// Set the size for the progress bar.
	if ( progress.valid() )
		progress->setTotalProgressBar ( _names.size() );

	// Number we are on.
	unsigned int number ( 0 );
	
	// Load the plugins.
	for ( Names::iterator iter = _names.begin(); iter != _names.end(); ++iter )
	{
		// Load the plugin if we are suppose to.
		if( iter->second )
		{
			// The name.
			std::string name ( iter->first );
			
			// Let the user know what plugin we are loading.
			status ( "Loading " + name + "...", true );
		
			// Load the plugin.
			Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, name );
		}
		
		// Update progress.
		if ( progress.valid() )
			progress->updateProgressBar ( ++number );
	}
}
