
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Factory/Readers.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Split.h"

using namespace Minerva::Core::Factory;


///////////////////////////////////////////////////////////////////////////////
//
//  Static member initialization.
//
///////////////////////////////////////////////////////////////////////////////

Readers* Readers::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

Readers& Readers::instance()
{
  if ( 0x0 == _instance )
    _instance = new Readers;
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Readers::Readers() :
  _mutex    ( new Readers::Mutex ),
  _filters  (),
  _creators ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Readers::~Readers()
{
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a creator.
//
///////////////////////////////////////////////////////////////////////////////

void Readers::add ( const std::string &filter, const std::string& extension, BaseCreator *creator )
{
  Guard guard ( *_mutex );
  _filters.push_back ( Filter ( filter, extension ) );
  
  typedef std::vector<std::string> Strings;
  Strings strings;
  Usul::Strings::split ( extension, ",", false, strings );
  
  for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
    _creators.insert ( Creators::value_type ( Usul::File::extension ( (*iter) ), creator ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a creator.
//
///////////////////////////////////////////////////////////////////////////////

void Readers::remove ( const std::string &filter, const std::string& extension )
{
  Guard guard ( *_mutex );
  
  typedef std::vector<std::string> Strings;
  Strings strings;
  Usul::Strings::split ( extension, ",", false, strings );
  
  for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
    _creators.erase ( Usul::File::extension ( (*iter) ) );
  
  // Remove the filter.
#if 0
  _filters.erase ( 
                 std::remove_if ( _filters.begin(), 
                                  _filters.end(), 
                                  Filter ( filter, extension ) ),
                 _filters.end() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Readers::create ( const std::string &extension )
{
  Guard guard ( *_mutex );
  Creators::iterator iter = _creators.find ( extension );
  if ( _creators.end() != iter )
    return (*iter->second) ();
  
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all filters.
//
///////////////////////////////////////////////////////////////////////////////

Readers::Filters Readers::filters() const
{
  Guard guard ( *_mutex );
  return _filters;
}
