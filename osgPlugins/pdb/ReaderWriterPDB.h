
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_PDB_H__
#define __READER_WRITER_PDB_H__

#include <osg/Geode>
#include <osg/Group>
#include <osg/Shape>
#include <osg/ShapeDrawable>

#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include <string>
#include <fstream>
#include <stdlib.h>

using namespace std;

class ReaderWriterPDB : public osgDB::ReaderWriter {
 public:
  virtual const char* className();
  virtual bool acceptsExtension(const string &);
  virtual ReadResult readNode(const string&, const osgDB::ReaderWriter::Options *);
};

#endif
