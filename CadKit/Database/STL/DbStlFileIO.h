
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

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_STL_FILE_IO_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_STL_FILE_IO_H_

#include "DbStlApi.h"

#include "Standard/SlVec3.h"
#include "Standard/SlStack.h"
#include "Standard/SlMatrix44.h"
#include "Standard/SlMatrix44IO.h"
#include "Standard/SlTypedefs.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <iostream>
# include <fstream>
# include <string>
#endif


namespace CadKit
{
class DB_STL_API DbStlOutputFile : public std::ofstream
{
public:
  enum StlFileState
  {
    UNCHANGED=0,
    HEADER_DONE=1,
    FIRST_FACET_DONE=2,
    MARKED_FOR_CLOSE=4
  };

  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 ) = 0;
  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal ) = 0;
  void close( );
  
protected:
  
  SlUint32 _numFacets; // need this to be 32bit integer
  std::string _filename;
  bool _readyToClose;
  int _fileState;

  DbStlOutputFile( );
  DbStlOutputFile( const char* name );
  DbStlOutputFile( const char* name, int nMode );
  DbStlOutputFile( const char* name, int nMode, int nProt );

  virtual void _writeHeader( ) { }
  virtual void _endFile( ) { }

  const int _getStlFileState( ) { return _fileState; }

  // inline shorhand functions for testing and setting file state bits in a readable manner
  bool _testHeaderDone( ) { return (_fileState & DbStlOutputFile::HEADER_DONE) ? true : false; }
  bool _testFirstFacetDone( ) { return (_fileState & DbStlOutputFile::FIRST_FACET_DONE) ? true : false; }
  bool _testMarkedForClose( ) { return (_fileState & DbStlOutputFile::MARKED_FOR_CLOSE) ? true : false; }
  void _setHeaderDone( ) { _fileState = _fileState | DbStlOutputFile::HEADER_DONE; }
  void _setFirstFacetDone( ) { _fileState = _fileState | DbStlOutputFile::FIRST_FACET_DONE; }
  void _setMarkedForClose( ) { _fileState = _fileState | DbStlOutputFile::MARKED_FOR_CLOSE; }
};




class DB_STL_API DbStlAsciiOutputFile : public DbStlOutputFile
{
public:
  
  DbStlAsciiOutputFile( ) 
    : _stdHeader( "#CadKit generated ASCII STL file" ) { }
  DbStlAsciiOutputFile( const char* name, bool ovrwrt = true );
  DbStlAsciiOutputFile( const char* name, const std::string &header, bool ovrwrt = true );
  ~DbStlAsciiOutputFile( ); 
  
//  virtual void writeFacet( const SlVec3f vertices[3], const SlVec3f &normal );
  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 );
  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal );

protected:
  std::string _header; //client supplied header information
  const std::string _stdHeader; // written in absence of client supplied header
  void _writeHeader( );
  void _endFile();

};

class DB_STL_API DbStlBinaryOutputFile : public DbStlOutputFile
{
public:  
  DbStlBinaryOutputFile( ) { }
  DbStlBinaryOutputFile( const char* name, bool ovrwrt = true  );
  DbStlBinaryOutputFile( const char* name, const std::string &header, bool ovrwrt = true );
  ~DbStlBinaryOutputFile( );

  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 );
  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal );

protected:
  std::string _header; //client supplied header information
  const std::string _stdHeader; // written in absence of client supplied header
  std::streampos _numFacetsPos;
  void _writeHeader( );
  void _endFile();

};

  
}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_STL_FILE_IO_H_