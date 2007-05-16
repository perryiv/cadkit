
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/Manager/Manager.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "XmlTree/Document.h"

using namespace CadKit::Helios::Plugins::Manager;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member.
//
///////////////////////////////////////////////////////////////////////////////

Manager* Manager::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Return the instance.
//
///////////////////////////////////////////////////////////////////////////////

Manager& Manager::instance()
{
	if( 0x0 == _instance )
		_instance = new Manager();
		
	return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() : 
	_filename(),
	_names()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filename to parse.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::filename ( const std::string& filename )
{
	_filename = filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename to parse.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Manager::filename() const
{
	return _filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::parse()
{
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

void Manager::_addPlugins ( XmlTree::Node &parent )
{
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
//  Add a node.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_addPlugin ( XmlTree::Node &node )
{
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
  
  if( false == file.empty() )
  	_names.insert ( Names::value_type ( file, load ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load( Usul::Interfaces::IUnknown *caller )
{
	// Query for interfaces.
	Usul::Interfaces::IStatusBar::UpdateStatusBar status ( caller );
	Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );
	
	// Set the size for the progress bar.
	if( progress.valid() )
		progress->setTotalProgressBar( _names.size() );
	
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
			status( "Loading " + name + "...", true );
		
			// Load the plugin.
			Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, name );
		}
		
		// Update progress.
		if( progress.valid() )
			progress->updateProgressBar( ++number );
	}
}

