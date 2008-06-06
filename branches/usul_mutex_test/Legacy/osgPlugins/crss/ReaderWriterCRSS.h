
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
#include <map>

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

  virtual bool            acceptsExtension ( const std::string &extension ) const;
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options ) const;

protected:

  osg::Group *            _build() const;
  osg::Node *             _buildBounds() const;
  osg::Node *             _buildCubes() const;
  osg::Node *             _buildSpheres() const;
  osg::Node *             _buildTriangles() const;
  osg::Node *             _buildLines() const;

  void                    _init() const;

  osg::Node  *            _makeCube   ( float size ) const;
  osg::Node  *            _makeSphere ( float radius, const osg::Vec2 &detail ) const;
  osg::Node  *            _makeSphere ( float radius ) const;

  void                    _material ( bool actual, osg::Material *am, osg::Material *pm, osg::Node *node ) const;

  Result                  _read ( const std::string &, const Options * ) const;

  void                    _readCube        ( std::istream& in ) const;
  void                    _readSphere      ( std::istream& in ) const;
  void                    _readTriangle    ( std::istream& in ) const;
  void                    _readLineSegment ( std::istream& in ) const;

private:

  /// Struct to hold triangle information.
  struct Triangle
  {
    osg::Vec3 v0, v1, v2;
    std::string mode, color;
  };

  // Struct to hold line information.
  struct Line
  {
    osg::Vec3 v0, v1;
    float thickness;
    std::string color;
  };

  typedef std::list < Triangle > Triangles;
  typedef std::list < Line > Lines;
  typedef std::map < std::string, osg::Vec4 > ColorMap;

  mutable osg::Vec3 _min;
  mutable osg::Vec3 _max;
  mutable Spheres _spheres;
  mutable Cubes _cubes;
  mutable Triangles _triangles;
  mutable Lines _lines;
  mutable ShapeFactory::Ptr _factory;
  unsigned int _numLodKids;
  float _lodDistPower;
  Usul::Math::Vec2ui _segsLat;
  Usul::Math::Vec2ui _segsLong;
  ColorMap _colors;
};


#endif // __READER_WRITER_CRSS_H__
