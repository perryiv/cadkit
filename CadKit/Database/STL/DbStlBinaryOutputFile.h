
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

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_STL_BINARY_FILE_OUTPUT_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_STL_BINARY_FILE_OUTPUT_H_

#include "DbStlOutputFile.h"


namespace CadKit
{
class DB_STL_API DbStlBinaryOutputFile : public DbStlOutputFile
{
public:  

  DbStlBinaryOutputFile ( const std::string &name, bool overWrite = true  );
  DbStlBinaryOutputFile ( const std::string &name, const std::string &header, bool overWrite = true );
  ~DbStlBinaryOutputFile();

  virtual void writeFacet ( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 );
  virtual void writeFacet ( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, SlVec3f &normal );

protected:

  // TODO_ERIC, can't _header and _stdHeader live in DbStlOutputFile?
  std::string _header; //client supplied header information
  const std::string _stdHeader; // written in absence of client supplied header
  std::streampos _numFacetsPos;

  void _writeHeader();
  void _endFile();

};

  
}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_STL_BINARY_FILE_OUTPUT_H_
