
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Eric W. Schmidt
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlBinaryOutputFile: Binary STL file output.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbStlPrecompiled.h"
#include "DbStlBinaryOutputFile.h"

using namespace CadKit;

#define BINARY_HEADER "#CadKit generated binary STL file"
#define HEADER_SIZE   80


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlBinaryOutputFile::DbStlBinaryOutputFile ( const std::string &name, bool overWrite ) :
  DbStlOutputFile ( name, std::ios_base::out | std::ios_base::binary | ( ( overWrite ) ? std::ios_base::trunc : 0 ) ),
  _header ( "" ),
  _stdHeader( BINARY_HEADER )
{
  int failtest = _out.fail();
  int nMode = std::ios::out | std::ios::binary | std::ios::trunc;
  int nMout = std::ios::out;
  int nMbinary = std::ios::binary;
  int nMtrunc = std::ios::trunc;
  SL_ASSERT( true == this->is_open() );
  this->_writeHeader();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlBinaryOutputFile::DbStlBinaryOutputFile ( const std::string &name, const std::string &header, bool overWrite ) :
  DbStlOutputFile ( name, std::ios_base::out | std::ios_base::binary | ( ( overWrite ) ? std::ios_base::trunc : 0 ) ),
  _header( header ),
  _stdHeader( BINARY_HEADER )
{
  SL_ASSERT ( true == this->is_open() );
  this->_writeHeader();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlBinaryOutputFile::~DbStlBinaryOutputFile( )
{
  // TODO_ERIC. Should this be in base class's destructor?
  this->close( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlBinaryOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 )
{
  //TODO add normal calculations
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlBinaryOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal )
{
  SL_ASSERT( true == this->is_open() );
  SlByte spacer = 0;
  
  if ( this->_testFirstFacetDone() ) // prepend 2 byte spacer if not first facet
  {
    _out.write ( reinterpret_cast<const char*>(&spacer), sizeof( SlByte ) );
    _out.write ( reinterpret_cast<const char*>(&spacer), sizeof( SlByte ) );
  }
  else
    this->_setFirstFacetDone();
    

  // TODO - need to add some kind of typecase conversion if system is not 32bit
  for (int i=0; i<3; i++) // normal is required for binary format
    _out.write ( (const char*)(&normal[i]), sizeof( SlFloat32 ) );
 
  _out.write ( (const char*)&vertex1[0], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex1[1], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex1[2], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex2[0], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex2[1], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex2[2], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex3[0], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex3[1], sizeof( SlFloat32 ) );
  _out.write ( (const char*)&vertex3[2], sizeof( SlFloat32 ) );

  ++_numFacets;  // increment _numFacets - required to be inserted upon file closure
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the file.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlBinaryOutputFile::_endFile( )
{
  SL_ASSERT( true == this->is_open() );
  SL_ASSERT( _testHeaderDone() && (_numFacets > 0) ); // make sure header and at least one facet exist

  // Jump back to placeholder so we can write final number of facets written...
  // this should be pointing to 4 empty bytes immediately following 
  // HEADER_SIZE byte header.
  _out.seekp( _numFacetsPos );
  _out.write ( (const char*)&_numFacets, sizeof( SlUint32 ) );
//  _out.write ( "####", sizeof( SlInt32 ) );

  _setMarkedForClose();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the header.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlBinaryOutputFile::_writeHeader( )
{
  SL_ASSERT( !_testHeaderDone() );
  SL_ASSERT( this->is_open() );
  char buf[HEADER_SIZE];
  SlInt32 spacer = 0;

  // default to standard header if none was provided in constructor
  std::string header = ( !_header.empty() ) ? _header : _stdHeader;

  // header is HEADER_SIZE bytes... pad with NULL if necessary
  int i = HEADER_SIZE - header.size();
  if ( i > 0 )
    header.append ( i, '\0' );
  
  header.copy ( buf, HEADER_SIZE );
  _out.write ( buf, HEADER_SIZE * sizeof ( SlByte ) );
  
  //grab current position so we can come back to write num facets when done
  _numFacetsPos = _out.tellp();

  // write out 4 byte null spacer for num facets
  _out.write ( (const char*)&spacer, sizeof( SlUint32 ) );

  _setHeaderDone();
}
