
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DataSyncComponent.h"

#include "Usul/Components/Factory.h"
#include "Usul/App/Application.h"
#include "Usul/Strings/Format.h"

#include <algorithm>

USUL_DECLARE_COMPONENT_FACTORY ( DataSyncComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DataSyncComponent, DataSyncComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DataSyncComponent::DataSyncComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DataSyncComponent::~DataSyncComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DataSyncComponent::queryInterface ( unsigned long iid )
{
  std::cout << Usul::Strings::format ("Calling queryInterface in DataSyncComponent for iid# ", iid ) << std::endl;
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IDataSync::IID:
    return static_cast < Usul::Interfaces::IDataSync * > ( this );
  case Usul::Interfaces::IPluginInitialize::IID:
    return static_cast < Usul::Interfaces::IPluginInitialize * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// set a data flag.
//
///////////////////////////////////////////////////////////////////////////////

void DataSyncComponent::setDataFlag( const std::string &machine, bool value )
{
  //USUL_TRACE_SCOPE;
  std::cout << Usul::Strings::format ("Setting ", machine, " to ", value ) << std::endl;
  //Guard guard ( this->mutex() );
  SharedBoolMap::iterator iter ( _sharedBoolMap.find ( machine ) );
  if ( iter != _sharedBoolMap.end() )
  {
    if(  (*iter).second.isLocal () )
      (*iter).second->data = value;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// query data state
//
///////////////////////////////////////////////////////////////////////////////

bool DataSyncComponent::queryDataState()
{
  //USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  std::cout << Usul::Strings::format ( "Querying data state in DataSync Plugin..." ) << std::endl;
  for( SharedBoolMap::iterator iter = _sharedBoolMap.begin(); iter != _sharedBoolMap.end(); ++iter )
  {
    if( false == iter->second->data )
    {
      return false;
    }
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
// reset data
//
///////////////////////////////////////////////////////////////////////////////

void DataSyncComponent::resetData()
{
  //USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  std::cout << Usul::Strings::format ( "Resetting all values to false in DataSync Plugin" ) << std::endl;
  for( SharedBoolMap::iterator iter = _sharedBoolMap.begin(); iter != _sharedBoolMap.end(); ++iter )
  {
   iter->second->data = false;
  }
}


void DataSyncComponent::initialize( Usul::Interfaces::IUnknown *caller )
{
  //Guard guard ( this->mutex() );

  // Debug information
  std::cout << "Initializing DataSync Plugin..." << std::endl;

  // Read config file.
   XmlTree::Document::RefPtr document ( new XmlTree::Document );
   
   std::string file = Usul::App::Application::instance().configFile( "machines" );
   document->load ( file );

   // Debug information
   std::cout << "Reading DataSync Plugin config file..." << std::endl;

   this->_readConfigFile( *document, caller, 0x0 );

   for( unsigned int i = 0; i < _machines.size(); ++i )
   {
     // Debug information
     std::cout << "Adding machine " << _machines.at( i ).first << "..." << std::endl;

     vpr::GUID guid ( _machines.at( i ).second );
     
     _sharedBoolMap[_machines.at( i ).first].init( guid, _machines.at( i ).first );
   }
   std::cout << Usul::Strings::format ( "Initialization complete in DataSync Plugin" ) << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the parameter file and parse elements.
//
///////////////////////////////////////////////////////////////////////////////

void DataSyncComponent::_readConfigFile( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  //Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    
  }

  Children& machines ( node.children() );
  for ( Children::iterator iter = machines.begin(); iter != machines.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "machine" == node->name() )
    {
      this->_parseMachine( *node, caller, progress );
    }
   
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a time line set.
//
///////////////////////////////////////////////////////////////////////////////

void DataSyncComponent::_parseMachine( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{ 
  //Guard guard ( this->mutex() );

  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );

  Machine m;
  m.first = "";
  m.second = "";
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    
    if ( "name" == iter->first )
    {
      m.first = iter->second;
    }
    if ( "guid" == iter->first )
    {
      m.second = iter->second;
    }
       
  }
  if( m.first != "" && m.second != "" )
    _machines.push_back( m );
 
}

