
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

#include "OsgTools/ShapeFactory.h"

#include "osg/Vec3"

#include "Usul/Math/Vector2.h"

#include <string>
#include <list>

namespace osg { class Group; class Node; class Material; };


class ReaderWriterCRSS : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter BaseClass;
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;
  typedef OsgTools::ShapeFactory ShapeFactory;
  typedef std::pair<float,bool> SizeAndType;
  typedef std::pair<osg::Vec3,SizeAndType> Sphere;
  typedef std::pair<osg::Vec3,SizeAndType> Cube;
  typedef std::list<Sphere> Spheres;
  typedef std::list<Cube> Cubes;

  ReaderWriterCRSS();
  ~ReaderWriterCRSS();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options );

protected:

  osg::Group *            _build() const;
  osg::Node *             _buildBounds() const;
  osg::Node *             _buildCubes() const;
  osg::Node *             _buildSpheres() const;

  void                    _init();

  osg::Node  *            _makeCube   ( float size ) const;
  osg::Node  *            _makeSphere ( float radius, const osg::Vec2 &detail ) const;
  osg::Node  *            _makeSphere ( float radius ) const;

  void                    _material ( bool actual, osg::Material *am, osg::Material *pm, osg::Node *node ) const;

  Result                  _read ( const std::string &, const Options * );

private:

  osg::Vec3 _min;
  osg::Vec3 _max;
  Spheres _spheres;
  Cubes _cubes;
  mutable ShapeFactory::Ptr _factory;
  unsigned int _numLodKids;
  float _lodDistPower;
  Usul::Math::Vec2ui _segsLat;
  Usul::Math::Vec2ui _segsLong;
};


#endif // __READER_WRITER_CRSS_H__
