
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

#include "osg/BoundingBox"
#include "osg/Vec3"
#include "osg/Node"
#include <vector>

#define UNDEFINED_NODE 0
#define NODE_HOLDER 1
#define POINT_HOLDER 2



class OctTreeNode : public Usul::Base::Object
{
public:

  USUL_DECLARE_REF_POINTERS ( OctTreeNode );

  typedef Usul::Interfaces::IUnknown Unknown;
 
  typedef osg::Vec3 Point;
  typedef osg::ref_ptr< osg::Vec3Array > Points;
  typedef Usul::Base::Object BaseClass;
  typedef OctTreeNode::RefPtr OctTreeNodePtr;
  typedef std::vector< OctTreeNodePtr > Children;
  typedef std::vector< osg::BoundingBox > BoundingBoxVec;
  typedef Usul::Math::Vec3d Vec3d;

  OctTreeNode();
  virtual ~OctTreeNode();

  // Getters
  osg::BoundingBox                  boundingBox();
  Children                          children();
  unsigned int                      type();
  unsigned int                      tolerance();
 
  // Setters
  void                              boundingBox( const osg::BoundingBox &bb );
  void                              type( unsigned int type );
  void                              tolerance( unsigned int level );
  bool                              add( Point p );

  osg::Node*                        buildScene( Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                              useLOD( bool value );

  void                              distance( float d );
  float                             getBoundingRadius();

protected:
  bool                              _contains( Point p );
  void                              _insertOrCreateChildren( Point p );
  void                              _reorder();
  void                              _createChildren();
  BoundingBoxVec                    _split();
  bool                              _addCellToChild( Point p );
  bool                              _rayQuadIntersect( Vec3d a, Vec3d b, Vec3d c, Vec3d d, Vec3d p );
  
  float                             _distance;
  

private:
    osg::BoundingBox                _bb;
    Children                        _children;
    Points                          _points;
    unsigned int                    _type;
    unsigned int                    _tolerance;

    bool                            _useLOD;
};

#endif // __EXPERIMENTAL_OCTTREENODE_H__