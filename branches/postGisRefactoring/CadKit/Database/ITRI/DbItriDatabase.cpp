
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
#include "DbItriAsciiWriter.h"

#include "Database/Base/DbBaseInline.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlVec3IO.h"
#include "Standard/SlFindExtreme.h"

#include <fstream>
#include <time.h>
#include <functional>
#include <iomanip>
#include <limits>

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString
#define SW       std::setw ( width )

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbItriDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbItriDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbItriDatabase::DbItriDatabase() : DbBaseTarget(),
  _triangles(),
  _numDecimals ( std::numeric_limits<float>::digits10 )
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

  // Clear the list of triangles.
  _triangles.clear();

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

IUnknown *DbItriDatabase::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbItriDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case ITriangleAppendFloat::IID:
    return static_cast<ITriangleAppendFloat *>(this);
  case IFileExtension::IID:
    return static_cast<IFileExtension *>(this);
  case IDataWrite::IID:
    return static_cast<IDataWrite *>(this);
  case IOutputAttribute::IID:
    return static_cast<IOutputAttribute *>(this);
  case IOutputPrecision::IID:
    return static_cast<IOutputPrecision *>(this);
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
//  Does the format have the attribute?
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::isAttributeSupported ( const FormatAttribute &attribute ) const
{
  SL_PRINT3 ( "In DbItriDatabase::isAttributeSupported(), this = %X, attribute = %d\n", this, attribute );

  switch ( attribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return false;
  case FORMAT_ATTRIBUTE_ASCII:
    return true;
  default:
    SL_ASSERT ( 0 ); // What format is this?
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the output attribute.
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::setOutputAttribute ( const FormatAttribute &attribute )
{
  SL_PRINT3 ( "In DbItriDatabase::setOutputAttribute(), this = %X, attribute = %d\n", this, attribute );

  // There is only ascii output.
  return ( FORMAT_ATTRIBUTE_ASCII == attribute );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of decimals to output.
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::setOutputNumDecimals ( unsigned int numDecimals )
{
  SL_PRINT3 ( "In DbItriDatabase::setOutputNumDecimals(), this = %X, numDecimals = %d\n", this, numDecimals );
  _numDecimals = numDecimals;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbItriDatabase::writeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbItriDatabase::writeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Open a file.
  std::ofstream out ( filename.c_str() );
  if ( false == out.is_open() )
    return false;

  // Write the number of triangles.
  out << _triangles.size() << '\n';

  // Calculate the column width.
  Utility::CalculateWidth<float> cw;
  unsigned int width ( cw ( _triangles ) + _numDecimals );

  // Loop through the triangles and write them.
  std::for_each ( _triangles.begin(), _triangles.end(), DbItriAsciiWriter ( out, _numDecimals, width ) );

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
  _triangles.push_back ( Triangle ( 
    SlVec3f ( t0v0, t0v1, t0v2 ),
    SlVec3f ( t1v0, t1v1, t1v2 ),
    SlVec3f ( t2v0, t2v1, t2v2 ) ) );

  // It worked.
  return true;
}
