
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

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlPathname.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

// To help shorted up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbOsgDatabase, DbBaseTarget );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::DbOsgDatabase() : DbBaseTarget()
{
  SL_PRINT2 ( "In DbOsgDatabase::DbOsgDatabase(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::~DbOsgDatabase()
{
  SL_PRINT2 ( "In DbOsgDatabase::~DbOsgDatabase(), this = %X\n", this );
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
  default:
    return DbBaseTarget::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the reference count.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long DbOsgDatabase::getRefCount() const
{
  SL_PRINT2 ( "In DbOsgDatabase::getRefCount(), this = %X\n", this );
  return SlRefBase::getRefCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::ref()
{
  SL_PRINT2 ( "In DbOsgDatabase::ref(), this = %X\n", this );
  this->_incrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::unref()
{
  SL_PRINT2 ( "In DbOsgDatabase::unref(), this = %X\n", this );
  this->_decrementReferenceCount();
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
  return false; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startAssembly ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOsgDatabase::startAssembly(), this = %X, caller = %X\n", this, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IHierarchyQuery> query ( IHierarchyQuery::IID, caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller." );

  // Create a group and set some properties.
  osg::ref_ptr<osg::MatrixTransform> group = new osg::MatrixTransform;

  // pseudo code...
  group->setName ( query->getHierarchyName() );
  group->setMatrix ( query->getHierarchyTransform() );
  // Get material when you need it.

  return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endAssembly ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOsgDatabase::endAssembly(), this = %X, caller = %X\n", this, caller );
  SL_ASSERT ( caller );
  return true; // TODO.
}
