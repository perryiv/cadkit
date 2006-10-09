
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Factory.h"

using namespace CadKit::OSG::Glue;

Factory*  Factory::_instance( 0x0 );


Factory& Factory::instance()
{
  if( 0x0 == _instance )
    _instance = new Factory();

  return *_instance;
}


Factory::Factory() : _map()
{
}

void Factory::create ( unsigned int key )
{
  _map[key] = new osg::Node;
}

osg::Node* Factory::node ( unsigned int key )
{
  NodeMap::const_iterator iter = _map.find( key );
  if( iter != _map.end() )
    return iter->second.get();

  return 0x0;
}

void Factory::node ( unsigned int key, osg::Node* node )
{
  _map[key] = node;
}

void Factory::remove ( unsigned int key )
{
  NodeMap::iterator iter = _map.find( key );
  if( iter != _map.end() )
    _map.erase( iter );
}
