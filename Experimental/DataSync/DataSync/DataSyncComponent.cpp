
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
  for( SharedBoolMap::iterator iter = _sharedBoolMap.begin(); iter != _sharedBoolMap.end(); ++iter )
  {
   iter->second->data = false;
  }
}


void DataSyncComponent::initialize( Usul::Interfaces::IUnknown *caller )
{
  //Guard guard ( this->mutex() );

  // Read config file.
   XmlTree::Document::RefPtr document ( new XmlTree::Document );
   
   std::string file = Usul::App::Application::instance().configFile( "machines" );
   document->load ( file );

   this->_readConfigFile( *document, caller, 0x0 );

   for( unsigned int i = 0; i < _machines.size(); ++i )
   {
     vpr::GUID guid ( _machines.at( i ).second );
     _sharedBoolMap[_machines.at( i ).first].init( guid, _machines.at( i ).first );
   }
  
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

