
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbItriDatabase: ITRI database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbItriPrecompiled.h"
#include "DbItriDatabase.h"

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

SL_IMPLEMENT_DYNAMIC_CLASS ( DbItriDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbItriDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbItriDatabase::DbItriDatabase() : DbBaseTarget()
{
  SL_PRINT2 ( "In DbItriDatabase::DbItriDatabase(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbItriDatabase::~DbItriDatabase()
{
  SL_PRINT2 ( "In DbItriDatabase::~DbItriDatabase(), this = %X\n", this );

//TODO make sure we clean up
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is about to receive data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbItriDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

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

bool DbItriDatabase::dataTransferEnd ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbItriDatabase::dataTransferEnd(), this = %X, caller = %X\n", this, caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbItriDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbItriDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
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

std::string DbItriDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbItriDatabase::getFileExtension(), this = %X\n", this );
  return "itri";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Store the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::storeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbItriDatabase::storeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Open a file.
  std::ofstream out ( filename.c_str() );
  if ( false == out.is_open() )
    return false;

  // Write the header.
  //time_t now ( time ( 0x0 ) );
  //out << "solid created: " << ::asctime ( ::localtime ( &now ) ) << "\n";
  out << _facets.size() << "\n";

  // Loop through the facets.
  for ( Facets::const_iterator i = _facets.begin(); i != _facets.end(); ++i )
  {
    const DbItriTriangle &facet = *i;
    //out << "facet normal " << facet.getNormal() << "\n";
    //out << "outer loop\n";
    out << facet.getVertex ( 0 ) << "\n" ;
    out << facet.getVertex ( 1 ) << "\n" ;
    out << facet.getVertex ( 2 ) << "\n";
    //out << "endloop\n";
    //out << "endfacet\n";
  }

  // Write the footer.
  //out << "endsolid\n";

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the triangle.
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::appendTriangle ( float t0v0, float t0v1, float t0v2, 
                                     float t1v0, float t1v1, float t1v2,
                                     float t2v0, float t2v1, float t2v2,
                                     IUnknown *caller )
{
  SL_PRINT3 ( "In DbItriDatabase::appendTriangle(), this = %X, caller = %X\n", this, caller );
  SL_ASSERT ( caller );

  // Append the triangle.
  _facets.push_back ( DbItriTriangle ( 
    t0v0, t0v1, t0v2,
    t1v0, t1v1, t1v2,
    t2v0, t2v1, t2v2 ) );

  // It worked.
  return true;
}
