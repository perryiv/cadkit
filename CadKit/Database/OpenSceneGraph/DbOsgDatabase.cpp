
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgDatabase: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbOsgPrecompiled.h"
#include "DbOsgDatabase.h"

#include "Interfaces/IAssemblyQuery.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlPathname.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "osg/MatrixTransform"
# include "osg/Geode"
# include "osg/Geometry"
# include "osgDB/WriteFile"
#endif

// To help shorted up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbOsgDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbOsgDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::DbOsgDatabase() : DbBaseTarget(),
  _root ( new osg::Group ),
  _groups ( new GroupStack )
{
  SL_PRINT2 ( "In DbOsgDatabase::DbOsgDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groups.get() );

  // Reference the group.
  _root->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::~DbOsgDatabase()
{
  SL_PRINT2 ( "In DbOsgDatabase::~DbOsgDatabase(), this = %X\n", this );

  // Unreference the group.
  _root->unref();

  // The stack should be empty or just holding the root.
  SL_ASSERT ( true == _groups->empty() || _root == _groups->top() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbOsgDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbOsgDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IAssemblyNotify::IID:
    return static_cast<IAssemblyNotify *>(this);
  case IPartNotify::IID:
    return static_cast<IPartNotify *>(this);
  case IInstanceNotify::IID:
    return static_cast<IInstanceNotify *>(this);
  default:
    return DbBaseTarget::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file extension.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbOsgDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbOsgDatabase::getFileExtension(), this = %X\n", this );
  return "osg";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Store the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::storeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbOsgDatabase::storeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Write the _root to file.
  return osgDB::writeNodeFile ( *_root, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( _root );

  // If our stack is empty (i.e., this is the first assembly encountered) 
  // then push the _root onto the stack.
  if ( true == _groups->empty() )
    this->_pushGroup ( _root );

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( IAssemblyQueryFloat::IID, caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group.
  SlRefPtr<osg::MatrixTransform> group = new osg::MatrixTransform;

  // Set the name.
  group->setName ( query->getName ( assembly ) );

  // Set the matrix if there is one.
  float matrix[16];
  if ( true == query->getTransform ( assembly, matrix ) )
    group->setMatrix ( osg::Matrix ( matrix ) );

  // Add this group to the scene.
  _groups->top()->addChild ( group );

  // Make it the new current group.
  this->_pushGroup ( group );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groups->empty() );

  // Done with this group.
  this->_popGroup();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( _root );

  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( _root );

  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( _root );

  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( _root );

  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::_pushGroup ( osg::Group *group )
{
  SL_PRINT3 ( "In DbOsgDatabase::_pushGroup(), this = %X, group = %X\n", this, group );
  SL_ASSERT ( group );

  // Push it onto our stack.
  _groups->push ( group );

  // Reference it.
  group->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::_popGroup()
{
  SL_PRINT2 ( "In DbOsgDatabase::_popGroup(), this = %X\n", this );
  SL_ASSERT ( false == _groups->empty() );

  // Unreference the top one.
  _groups->top()->unref();

  // Pop it off of our stack.
  _groups->pop();
}
