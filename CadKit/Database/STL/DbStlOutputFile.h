
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Eric W. Schmidt
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlOutputFile: Base class for STL file output.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_BASE_STL_FILE_OUTPUT_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_BASE_STL_FILE_OUTPUT_H_

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
class DB_STL_API DbStlOutputFile
{
public:

  enum StlFileState
  {
    UNCHANGED=0,
    HEADER_DONE=1,
    FIRST_FACET_DONE=2,
    MARKED_FOR_CLOSE=4
  };

  void close();

  bool is_open() const { return _out.is_open(); }

  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 ) = 0;
  virtual void writeFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal ) = 0;
  
protected:
  
  SlUint32 _numFacets; // need this to be 32bit integer
  bool _readyToClose;
  int _fileState;
  std::ofstream _out;

  DbStlOutputFile ( const std::string &name, int nMode );
  ~DbStlOutputFile();

  bool _constructorCommon ( const std::string &name, int mode );

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

  
}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_BASE_STL_FILE_OUTPUT_H_
