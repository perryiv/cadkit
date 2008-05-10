
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Utilities/DeleteHandler.h"

#include "osg/Node"
#include "osg/Drawable"

#include <limits>
#include <iostream>

using namespace OsgTools::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DeleteHandler::DeleteHandler() : BaseClass()
{
  this->setNumFramesToRetainObjects ( std::numeric_limits<unsigned int>::max() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DeleteHandler::~DeleteHandler()
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock ( _mutex );
  if ( false == _objectsToDelete.empty() )
  {
    std::cout << _objectsToDelete.size() << " osg::Referenced items still remain." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all objects..
//
///////////////////////////////////////////////////////////////////////////////

void DeleteHandler::flush()
{
  this->flushAll();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all objects..
//
///////////////////////////////////////////////////////////////////////////////

void DeleteHandler::flushAll()
{
  ObjectsToDeleteList objects;
  {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock ( _mutex );
    objects = _objectsToDelete;
    _objectsToDelete.clear();
  }
  
  for( ObjectsToDeleteList::iterator itr = objects.begin(); itr != objects.end(); ++itr )
  {
    const osg::Referenced * object ( itr->second );

    if ( 0x0 != object )
    {
      if ( 0 == object->referenceCount() )
        this->doDelete( itr->second );
      else
      {
        std::string name ( DeleteHandler::_name ( object ) );
          
        std::cout << "Trying to delete object: " 
                  << name
                  << " with reference count of: " 
                  << object->referenceCount() << std::endl;

        //this->requestDelete ( object );
      }
    }
  }

  objects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all objects..
//
///////////////////////////////////////////////////////////////////////////////

std::string DeleteHandler::_name ( const osg::Referenced* object )
{
  std::string name;

  if ( const osg::Node* node = dynamic_cast < const osg::Node* > ( object ) )
    name = node->className();
  else if ( const osg::Object *o = dynamic_cast < const osg::Object* > ( object ) )
    name = o->className();
  else
    name = typeid(object).name();

  return name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of objects.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DeleteHandler::numObjects()
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock ( _mutex );
  return _objectsToDelete.size();
}
