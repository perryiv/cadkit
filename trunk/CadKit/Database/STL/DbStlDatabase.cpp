
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
#include "DbStlBinaryWriter.h"
#include "DbStlAsciiWriter.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"

#include <fstream>
#include <string>
#include <time.h>

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbStlDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbStlDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlDatabase::DbStlDatabase() : DbBaseTarget(),
  _facets(),
  _ascii ( false )
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when this target it is about to receive data.
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
//  Called when this target it is done receiving data.
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

  // Store as either ascii or binary.
  if ( _ascii )
    return _writeAscii ( filename );
  else
    return _writeBinary ( filename );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the header.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbStlDatabase::_getHeader() const
{
  SL_PRINT2 ( "In DbStlDatabase::_getHeader(), this = %X\n", this );

  time_t now ( time ( 0x0 ) );
  std::string header ( std::string ( "solid created: " ) + ::asctime ( ::localtime ( &now ) ) );

  // Note: asctime() appends a '\n'
  header.erase ( header.end() );

  // Binary STL requires an 80 byte header. Since we append null characters
  // the ascii output won't write them (which is good).
  header.resize ( 80, '\0' );

  return header;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write as ascii data.
//  See http://www.sdsc.edu/tmf/Stl-specs/stl.html or
//  http://astronomy.swin.edu.au/~pbourke/geomformats/stl/
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::_writeAscii ( const std::string &filename )
{
  SL_PRINT3 ( "In DbStlDatabase::_writeAscii(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Open a file.
  std::ofstream out ( filename.c_str() );
  if ( false == out.is_open() )
    return false;

  // Write the header.
  out << "solid created: " << this->_getHeader().c_str() << '\n';

  // Loop through the facets and write them.
  std::for_each ( _facets.begin(), _facets.end(), DbStlAsciiWriter ( out ) );

  // Write the footer.
  out << "endsolid\n";

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write as binary data.
//  See http://astronomy.swin.edu.au/~pbourke/geomformats/stl/
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::_writeBinary ( const std::string &filename )
{
  SL_PRINT3 ( "In DbStlDatabase::_writeBinary(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Open a file.
  std::ofstream out ( filename.c_str(), std::ios::binary );
  if ( false == out.is_open() )
    return false;

  // Write the header.
  std::string header ( this->_getHeader() );
  out.write ( header.c_str(), header.size() );

  // Helper class.
  DbStlBinaryWriter writer ( out );

  // Write the number of facets.
  writer.write ( _facets.size() );

  // Loop through the facets and write them.
  std::for_each ( _facets.begin(), _facets.end(), writer );

  // It worked.
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
  _facets.push_back ( DbStlTriangle ( 
    t0v0, t0v1, t0v2,
    t1v0, t1v1, t1v2,
    t2v0, t2v1, t2v2 ) );

  // It worked.
  return true;
}
