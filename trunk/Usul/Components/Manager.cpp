
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Components/Manager.h"
#include "Usul/Components/Object.h"

#include "Usul/Interfaces/IPlugin.h"

using namespace Usul;
using namespace Usul::Components;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member
//
///////////////////////////////////////////////////////////////////////////////

Manager* Manager::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance
//
///////////////////////////////////////////////////////////////////////////////

Manager* Manager::instance()
{
  if( !_instance )
    _instance = new Manager();
  return _instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
_unknowns()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load plugins in given director with given extension.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( unsigned long iid, const std::list<std::string> &plugins )
{
  typedef std::list<std::string>::const_iterator Iterator;

  for ( Iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    // Load plugin file.
    const std::string name ( *i );
    Usul::Interfaces::IUnknown::QueryPtr unknown ( Usul::Components::Object::create ( iid, name, true, true ) );

    // Insert into set of plugins.
    if ( unknown.valid() )
      _unknowns.insert ( unknown.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear plugins and release libraries
//
///////////////////////////////////////////////////////////////////////////////

void Manager::clear () 
{ 
  _unknowns.clear(); 
  Usul::Components::Object::releaseLibraries(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find first IUnknown with given iid
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Manager::getInterface( unsigned long iid )
{
  for( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( (*i).get() );
    if( u->queryInterface( iid ) )
      return (*i).get();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all IUnknowns with given iid
//
///////////////////////////////////////////////////////////////////////////////

Manager::UnknownSet Manager::getInterfaces( unsigned long iid )
{
  UnknownSet set;

  for( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( (*i).get() );
    if( u->queryInterface( iid ) )
      set.insert( *i );
  }

  return set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all IUnknowns with given iids
//
///////////////////////////////////////////////////////////////////////////////

Manager::UnknownSet Manager::getInterfaces ( unsigned long iid1, unsigned long iid2 )
{
  UnknownSet set;

  for ( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( i->get() );
    if ( u->queryInterface ( iid1 ) && u->queryInterface ( iid2 )  )
      set.insert (  *i );
  }

  return set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return list of plugin names.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Strings Manager::names() const
{
  Manager::Strings names;
  UnknownSet unknowns ( _unknowns );
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    Usul::Interfaces::IPlugin::QueryPtr plugin ( *i );
    if ( plugin.valid() )
      names.push_back ( plugin->getPluginName() );
  }
  names.sort();
  return names;
}
