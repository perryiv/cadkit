
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Eric W. Schmidt
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlAsciiOutputFile: Ascii STL file output.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbStlPrecompiled.h"
#include "DbStlAsciiOutputFile.h"

#include "Standard/SlVec3IO.h"

using namespace CadKit;

#define ASCII_HEADER "#CadKit generated ASCII STL file"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlAsciiOutputFile::DbStlAsciiOutputFile ( const std::string &name, bool overWrite ) :
  DbStlOutputFile ( name, std::ios_base::out ),
  _header ( "" ),
  _stdHeader( ASCII_HEADER )
{
  //TODO add code to check for valid filename
  SL_ASSERT( true == this->is_open() );
  this->_writeHeader( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlAsciiOutputFile::DbStlAsciiOutputFile ( const std::string &name, const std::string &header, bool overWrite ) :
  DbStlOutputFile ( name, std::ios_base::out | ( ( overWrite ) ? std::ios_base::trunc : 0 ) ),
  _header( header ),
  _stdHeader( ASCII_HEADER )
{
  //TODO add code to check for valid filename
  SL_ASSERT( true == this->is_open() );

  this->_writeHeader( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlAsciiOutputFile::~DbStlAsciiOutputFile( )
{
  // TODO_ERIC. Should this be in base class's destructor?
  this->close( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlAsciiOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 )
{
  SL_ASSERT( true == this->is_open() );

  // Do the math to calculate the normal.
  // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlAsciiOutputFile::writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal )
{
  SL_ASSERT( true == this->is_open() );

//  if ( !_testFirstFacetDone() )
//    _setFirstFacetDone();

  // Can't hurt.
  normal.normalize();
  
  _out << "  facet";

  if ( normal ) // only need this if normal is provided... normal not required for valid facet in ascii
    _out << " normal " << normal[0] << " " << normal[1] << " " << normal[2];
  
  _out << std::endl <<  "    outer loop" << std::endl;

  _out << "      vertex " << vertex1 << std::endl;
  _out << "      vertex " << vertex2 << std::endl;
  _out << "      vertex " << vertex3 << std::endl;

  _out << "    endloop" << std::endl << "  endfacet" << std::endl;

  _numFacets++; // not required for ascii... but good to have just in case
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the file.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlAsciiOutputFile::_endFile( )
{
  SL_ASSERT( true == this->is_open() );
  _out << "endsolid" << std::endl;
  _setMarkedForClose();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the header.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlAsciiOutputFile::_writeHeader( )
{
  SL_ASSERT( !_testHeaderDone() );
  SL_ASSERT( this->is_open() );

  // default to standard header if none was provided in constructor
  std::string header = _header.size() > 0 ? _header : _stdHeader;
  _out << "solid " << header.c_str() << std::endl;

  //TODO - clean up header stuff a little bit
  //TODO make sure no newline and/or funky characters in header
  //TODO maybe put filename in header
  _setHeaderDone();
}
