
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

#include "DbStlPrecompiled.h"
#include "DbStlDatabase.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlVec3IO.h"

#include <fstream>
#include <time.h>

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

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

  // Clear the list of facets.
  _facets.clear();

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

  // Nothing to do.
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
  case ITriangleAppendFloat::IID:
    return static_cast<ITriangleAppendFloat *>(this);
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

  // Open a file.
  std::ofstream out ( filename.c_str() );
  if ( false == out.is_open() )
    return false;

  // Write the header.
  time_t now ( time ( 0x0 ) );
  out << "solid created: " << ::asctime ( ::localtime ( &now ) ) << "\n";

  // Loop through the facets.
  for ( Facets::const_iterator i = _facets.begin(); i != _facets.end(); ++i )
  {
    const DbStlFacet &facet = *i;
    out << "facet normal " << facet.getNormal() << "\n";
    out << "outer loop\n";
    out << "vertex " << facet.getVertex ( 0 ) << "\n";
    out << "vertex " << facet.getVertex ( 1 ) << "\n";
    out << "vertex " << facet.getVertex ( 2 ) << "\n";
    out << "endloop\n";
    out << "endfacet\n";
  }

  // Write the footer.
  out << "endsolid\n";

  // It worked.
  return true;
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

  // Nothing to do.
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

  // Nothing to do.
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

  // Nothing to do.
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

  // Nothing to do.
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

  // Nothing to do.
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

  // Nothing to do.
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
//  Append the triangle.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::appendTriangle ( float t0v0, float t0v1, float t0v2, 
                                     float t1v0, float t1v1, float t1v2,
                                     float t2v0, float t2v1, float t2v2,
                                     IUnknown *caller )
{
  SL_PRINT3 ( "In DbStlDatabase::appendTriangle(), this = %X, caller = %X\n", this, caller );
  SL_ASSERT ( caller );

  // Append the triangle.
  _facets.push_back ( DbStlFacet ( 
    t0v0, t0v1, t0v2,
    t1v0, t1v1, t1v2,
    t2v0, t2v1, t2v2 ) );

  // It worked.
  return true;
}
