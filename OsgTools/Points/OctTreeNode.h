
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
//
//  For use with CFDViz for spatial partitioning of Computational Fluid
//  Dynamics visualization.  Will generalize at a later date when 
//  proof of concept is working.
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EXPERIMENTAL_OCTTREENODE_H__
#define __EXPERIMENTAL_OCTTREENODE_H__

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Types/Types.h"

#include "osg/BoundingBox"
#include "osg/Vec3"
#include "osg/Node"
#include "osg/PrimitiveSet"

#include "boost/shared_ptr.hpp"

#include <vector>
#include <list>
#include <iosfwd>

#define UNDEFINED_NODE 0
#define NODE_HOLDER 1
#define POINT_HOLDER 2

class OctTreeNode : public Usul::Base::Object
{
public:

  USUL_DECLARE_REF_POINTERS ( OctTreeNode );

  typedef Usul::Interfaces::IUnknown Unknown;
 
  typedef osg::Vec3f Point;
  typedef osg::ref_ptr< osg::Vec3Array > Points;
  typedef Usul::Base::Object BaseClass;
  typedef OctTreeNode::RefPtr OctTreeNodePtr;
  typedef std::vector< OctTreeNodePtr > Children;
  typedef std::vector< osg::BoundingBox > BoundingBoxVec;
  typedef Usul::Math::Vec3d Vec3d;
  typedef std::list< osg::Vec3f > LinkedList;
  typedef osg::ref_ptr< osg::PrimitiveSet > PrimitiveSetPtr;
  typedef osg::DrawElementsUShort Elements;
  typedef osg::ref_ptr < Elements > ElementsPtr;
  typedef std::vector<char> StreamBuffer;
  typedef boost::shared_ptr<StreamBuffer> StreamBufferPtr;

  OctTreeNode ( StreamBufferPtr, const std::string &tempPath );
  virtual ~OctTreeNode();

  // Getters
  osg::BoundingBox                  boundingBox();
  Children                          children();
  unsigned int                      type();
  unsigned int                      capacity();
 
  // Setters
  void                              boundingBox( const osg::BoundingBox &bb );
  void                              type( unsigned int type );
  void                              capacity( unsigned int level );
  bool                              add( Point p );

  void                              buildVectors();

  osg::Node*                        buildScene( Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                              useLOD( bool value );

  void                              distance( float d );
  float                             getBoundingRadius();

  void                              closeInputStream();
  void                              closeOutputStream();

  

  bool                              split();

protected:

  bool                              _contains( OctTreeNode::Point p );
  void                              _insertOrCreateChildren( OctTreeNode::Point p );
  void                              _reorder();
  void                              _createChildren();
  void                              _split();
  bool                              _addCellToChild( OctTreeNode::Point p );
  bool                              _rayQuadIntersect( Vec3d a, Vec3d b, Vec3d c, Vec3d d, Vec3d p );
  
  float                             _distance;

  void                              _openTempFileForRead();
  void                              _openTempFileForWrite();

  std::ifstream&                    _getInputStream();
  std::ofstream&                    _getOutputStream();

  void                              _closeInputStream();
  void                              _closeOutputStream();

  void                              _closeChildrenStreams();

private:

  osg::BoundingBox                _bb;
  Children                        _children;
  Points                          _points;
  unsigned int                    _type;
  unsigned int                    _capacity;

  bool                            _useLOD;
  LinkedList*                     _list; 

  unsigned int                    _numLODs;

  std::ifstream*                  _infile;
  std::ofstream*                  _outfile;
  std::string                     _tempFilename;
  Usul::Types::Uint64             _numPoints;
  StreamBufferPtr                 _streamBuffer;
  std::string                     _tempPath;

  static long                     _streamCount;
};

#endif // __EXPERIMENTAL_OCTTREENODE_H__