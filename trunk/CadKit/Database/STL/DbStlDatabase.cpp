
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlDatabase: STL database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

//???????????
// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
#include <math.h>
//????????????

#include "DbStlPrecompiled.h"
#include "DbStlDatabase.h"


#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IShapeQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryTexCoords.h"
#include "Interfaces/IClientData.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

#define LAST_LOD_RANGE            1e7
#define MAX_LOD_DISTANCE_FACTOR   30

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbStlDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbStlDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlDatabase::DbStlDatabase() : DbBaseTarget()
{
  SL_PRINT2 ( "In DbStlDatabase::DbStlDatabase(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlDatabase::~DbStlDatabase()
{
  SL_PRINT2 ( "In DbStlDatabase::~DbStlDatabase(), this = %X\n", this );
\
//TODO make sure we clean up
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is about to receive data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbStlDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

  // init FacetManager
  _fmgr.init();

  // clear out part/lod flags
  _partLodCheck.clear();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is done receiving data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::dataTransferEnd ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbStlDatabase::dataTransferEnd(), this = %X, caller = %X\n", this, caller );

  // Should be true.
//  SL_ASSERT ( _fmgr._transforms.size() );
//  SL_ASSERT ( this->_getRoot()->referenceCount() >= 1 );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbStlDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbStlDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IAssemblyNotify::IID:
    return static_cast<IAssemblyNotify *>(this);
  case IPartNotify::IID:
    return static_cast<IPartNotify *>(this);
  case IInstanceNotify::IID:
    return static_cast<IInstanceNotify *>(this);
  case ILodNotify::IID:
    return static_cast<ILodNotify *>(this);
  case IShapeNotify::IID:
    return static_cast<IShapeNotify *>(this);
  case ISetNotify::IID:
    return static_cast<ISetNotify *>(this);
  default:
    return DbBaseTarget::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file extension.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbStlDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbStlDatabase::getFileExtension(), this = %X\n", this );
  return "stl";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Store the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::storeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbStlDatabase::storeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Write the root to file.
  return _fmgr.storeData( filename.c_str(), DbStlFacetManager::STL_ASCII_FILE_MODE );
  //TODO - flag for choosing binary or ascii
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Push the matrix if there is one.
  SlMatrix44f matrix;
  if ( true == query->getTransform ( assembly, matrix, false ) )
    _fmgr.pushTransform ( matrix );

  // It worked
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );

  // nothing to do
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Push the matrix if there is one.
  SlMatrix44f matrix;
  if ( true == query->getTransform ( part, matrix, false ) )
    _fmgr.pushTransform ( matrix );

  // It worked
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );

  // nothing to do
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Push the matrix if there is one.
  SlMatrix44f matrix;
  if ( true == query->getTransform ( instance, matrix, false ) )
    _fmgr.pushTransform ( matrix );

  // It worked
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );


  // Need to determine if we have processed a LOD for the associated part yet...
  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> shapeQuery ( caller );
  if ( shapeQuery.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> lodQuery ( caller );
  if ( lodQuery.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  PartHandle p( lodQuery->getParent( shapeQuery->getParent( shape ) ) );
  if ( p ) // check for null
    if ( _partLodCheck.find( p ) == _partLodCheck.end() ) // only procede if no entry for part
    {
      _partLodCheck.insert( p ); // create entry so we don't process another lod for this part

      // Add the vertices and normals. 
      if ( false == _fmgr.fetchVerticesPerShape( caller, shape ) )
        return ERROR ( "Failed to add shape sets for given shape.", FAILED );
    }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // We ignore this because we build the geometry in the shape function above.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


