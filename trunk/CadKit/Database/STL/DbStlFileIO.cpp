///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Eric W. Schmidt
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFileIO: STL file IO 
//
///////////////////////////////////////////////////////////////////////////////

#include "DbStlPrecompiled.h"
#include "DbStlFileIO.h"

#include "Standard/SlTypedefs.h"
#include "Standard/SlVec3IO.h"

#include <iostream>
#include <fstream>

using namespace CadKit;
#define true 1



///////////////////////////////////////////////////////////////////////////////
//
//  DbStlOutputFile class functions
//
///////////////////////////////////////////////////////////////////////////////

DbStlOutputFile::DbStlOutputFile( ) 
  : _numFacets( 0 ), _readyToClose( false ), _fileState( DbStlOutputFile::UNCHANGED )
{
    
}

DbStlOutputFile::DbStlOutputFile( const char* name ) : std::basic_ofstream<char>(name),
  _numFacets( 0 ), 
  _readyToClose( false ), 
  _fileState( DbStlOutputFile::UNCHANGED )
{ 
//  std::ofstream::open(name);
}

DbStlOutputFile::DbStlOutputFile( const char* name, int nMode ) : std::basic_ofstream<char>( name, nMode ),
  _numFacets( 0 ), 
  _readyToClose( false ), 
  _fileState( DbStlOutputFile::UNCHANGED )
{ 
//  std::ofstream::open( name, nMode );
}

DbStlOutputFile::DbStlOutputFile( const char* name, int nMode, int nProt ) : std::basic_ofstream<char>( name, nMode ),
  _numFacets( 0 ), 
  _readyToClose( false ), 
  _fileState( DbStlOutputFile::UNCHANGED )
{ 
//  std::ofstream::open( name, nMode);
}

void DbStlOutputFile::close( )
{
  if ( this->is_open() )
  {
    if (!_testMarkedForClose() )
      _endFile();

    std::basic_ofstream<char> &me = static_cast<std::basic_ofstream<char> &>(*this);
    me.close();
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  DbStlAsciiOutputFile class functions
//
///////////////////////////////////////////////////////////////////////////////

DbStlAsciiOutputFile::DbStlAsciiOutputFile( const char* name, bool ovrwrt /*= true*/ )
  : DbStlOutputFile( name, std::ios_base::out),
   // _filename( name ),
    _stdHeader( "#CadKit generated ASCII STL file" )
{
  //TODO add code to check for valid filename
  SL_ASSERT( true == this->is_open() );
  _writeHeader( );
}


DbStlAsciiOutputFile::DbStlAsciiOutputFile( const char* name, const std::string &header, bool ovrwrt/*= true*/ )
  : DbStlOutputFile( name, std::ios_base::out|((ovrwrt)?std::ios_base::trunc:0) ),
    //_filename( name ),
    _header( header ),
    _stdHeader( "#CadKit generated ASCII STL file" )
{
  //TODO add code to check for valid filename
  SL_ASSERT( true == this->is_open() );

  _writeHeader( );
}

DbStlAsciiOutputFile::~DbStlAsciiOutputFile( )
{
  this->close( );
}

/*
void DbStlAsciiOutputFile::writeFacet( const SlVec3<float> vertices[3], const SlVec3<float> &normal )
{
  SL_ASSERT( true == this->is_open() );
  SL_ASSERT( vertices );

//  if ( !_testFirstFacetDone() )
//    _setFirstFacetDone();
  
  *this << "  facet";

  if ( normal ) // only need this if normal is provided... normal not required for valid facet in ascii
    *this << " normal " << normal[0] << " " << normal[1] << " " << normal[2];
  
  *this << std::endl <<  "    outer loop" << std::endl;

  for (int i = 0; i < 3; i++)
    *this << "      vertex " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;

  *this << "    endloop" << std::endl << "  endfacet" << std::endl;

  _numFacets++; // not required for ascii... but good to have just in case
}
*/

void DbStlAsciiOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 )
{
  SL_ASSERT( true == this->is_open() );

  // Do the math to calculate the normal.
  // TODO.
}

void DbStlAsciiOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal )
{
  SL_ASSERT( true == this->is_open() );

//  if ( !_testFirstFacetDone() )
//    _setFirstFacetDone();

  // Can't hurt.
  normal.normalize();
  
  *this << "  facet";

  if ( normal ) // only need this if normal is provided... normal not required for valid facet in ascii
    *this << " normal " << normal[0] << " " << normal[1] << " " << normal[2];
  
  *this << std::endl <<  "    outer loop" << std::endl;

  *this << "      vertex " << vertex1 << std::endl;
  *this << "      vertex " << vertex2 << std::endl;
  *this << "      vertex " << vertex3 << std::endl;

  *this << "    endloop" << std::endl << "  endfacet" << std::endl;

  _numFacets++; // not required for ascii... but good to have just in case
}

void DbStlAsciiOutputFile::_endFile( )
{
  SL_ASSERT( true == this->is_open() );
  *this << "endsolid" << std::endl;
  _setMarkedForClose();
}

void DbStlAsciiOutputFile::_writeHeader( )
{
  SL_ASSERT( !_testHeaderDone() );
  SL_ASSERT( this->is_open() );

  // default to standard header if none was provided in constructor
  std::string header = _header.size() > 0 ? _header : _stdHeader;
  *this << "solid " << header.c_str() << std::endl;

  //TODO - clean up header stuff a little bit
  //TODO make sure no newline and/or funky characters in header
  //TODO maybe put filename in header
  _setHeaderDone();
}




///////////////////////////////////////////////////////////////////////////////
//
//  DbStlBinaryOutputFile class functions
//
///////////////////////////////////////////////////////////////////////////////

DbStlBinaryOutputFile::DbStlBinaryOutputFile( const char* name, bool ovrwrt /*= true*/ )
  : DbStlOutputFile( name, std::ios_base::out|std::ios_base::binary|((ovrwrt)?std::ios_base::trunc:0) ),
//  : DbStlOutputFile( name, /*std::ios::out | std::ios::binary | std::ios::trunc*/ ),
   // _filename( name ),
    _stdHeader( "#CadKit generated binary STL file" )
{
  int failtest = this->fail();
  int nMode = std::ios::out | std::ios::binary | std::ios::trunc;
  int nMout = std::ios::out;
  int nMbinary = std::ios::binary;
  int nMtrunc = std::ios::trunc;
  SL_ASSERT( true == this->is_open() );
  _writeHeader();
}

DbStlBinaryOutputFile::DbStlBinaryOutputFile( const char* name, const std::string &header, bool ovrwrt /*= true*/ )
  : DbStlOutputFile( name, std::ios_base::out|std::ios_base::binary|((ovrwrt)?std::ios_base::trunc:0) ),
//  : DbStlOutputFile( name, std::ios::out | std::ios::binary | std::ios::trunc ),
   // _filename( name ),
    _header( header ),
    _stdHeader( "#CadKit generated binary STL file" )
{
  SL_ASSERT( true == this->is_open() );
  _writeHeader();
}

DbStlBinaryOutputFile::~DbStlBinaryOutputFile( )
{
  this->close( );
}

void DbStlBinaryOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 )
{
  //TODO add normal calculations
}
void DbStlBinaryOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal )
{
  SL_ASSERT( true == this->is_open() );
  SlByte spacer = 0;
  
  if ( _testFirstFacetDone() ) // prepend 2 byte spacer if not first facet
  {
    this->write( reinterpret_cast<const char*>(&spacer), sizeof( SlByte ) );
    this->write( reinterpret_cast<const char*>(&spacer), sizeof( SlByte ) );
  }
  else
    _setFirstFacetDone();
    

  // TODO - need to add some kind of typecase conversion if system is not 32bit
  for (int i=0; i<3; i++) // normal is required for binary format
    this->write( (const char*)(&normal[i]), sizeof( SlFloat32 ) );
 
    this->write( (const char*)&vertex1[0], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex1[1], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex1[2], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex2[0], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex2[1], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex2[2], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex3[0], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex3[1], sizeof( SlFloat32 ) );
    this->write( (const char*)&vertex3[2], sizeof( SlFloat32 ) );

  _numFacets++;  // increment _numFacets - required to be inserted upon file closure
}

void DbStlBinaryOutputFile::_endFile( )
{
  SL_ASSERT( true == this->is_open() );
  SL_ASSERT( _testHeaderDone() && (_numFacets > 0) ); // make sure header and at least one facet exist

  // Jump back to placeholder so we can write final number of facets written...
  // this should be pointing to 4 empty bytes immediately following 80 byte header
  this->seekp( _numFacetsPos );
  this->write( (const char*)&_numFacets, sizeof( SlUint32 ) );
//  this->write( "####", sizeof( SlInt32 ) );

  _setMarkedForClose();
}

void DbStlBinaryOutputFile::_writeHeader( )
{
  SL_ASSERT( !_testHeaderDone() );
  SL_ASSERT( this->is_open() );
  char buf[80];
  SlInt32 spacer = 0;

  // default to standard header if none was provided in constructor
  std::string header = ( !_header.empty() ) ? _header : _stdHeader;

  // header is 80 bytes... pad with NULL if necessary
  if (int i = 80-header.size() > 0 )
    header.append( i, '\0' );
  
  header.copy( buf, 80 );
  this->write( buf, 80*sizeof( SlByte ) );
  
  //grab current position so we can come back to write num facets when done
  _numFacetsPos = this->tellp();

  // write out 4 byte null spacer for num facets
  this->write( (const char*)&spacer, sizeof( SlUint32 ) );

  _setHeaderDone();
}


