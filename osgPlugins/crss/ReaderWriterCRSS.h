
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_CRSS_H__
#define __READER_WRITER_CRSS_H__

#include "osgDB/ReaderWriter"

#include <string>
#include <list>

namespace osg { class Group; };


class ReaderWriterCRSS : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;

  ReaderWriterCRSS();
  ~ReaderWriterCRSS();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options );

protected:

  osg::Group *            _build() const;
  void                    _init();
  Result                  _read ( const std::string &, const Options * );

private:

  typedef std::pair<float,bool> SizeAndType;
  typedef std::pair<osg::Vec3,SizeAndType> Sphere;
  typedef std::pair<osg::Vec3,SizeAndType> Cube;
  typedef std::list<Sphere> Spheres;
  typedef std::list<Cube> Cubes;

  osg::Vec3 _min;
  osg::Vec3 _max;
  Spheres _spheres;
  Cubes _cubes;
};


#endif // __READER_WRITER_CRSS_H__
