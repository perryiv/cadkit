
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_SPDAT_H__
#define __READER_WRITER_SPDAT_H__

#include "osgDB/ReaderWriter"

#include <string>

namespace osg { class Group; class Geode; };


class ReaderWriterSPDAT : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter BaseClass;
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;

  ReaderWriterSPDAT();
  ~ReaderWriterSPDAT();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options );

protected:

  Result                  _read ( const std::string &, const Options * );
};


#endif // __READER_WRITER_SPDAT_H__
