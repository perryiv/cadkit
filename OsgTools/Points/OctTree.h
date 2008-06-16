
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

#ifndef __EXPERIMENTAL_OCTTREE_H__
#define __EXPERIMENTAL_OCTTREE_H__

#include "OctTreeNode.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"

#include "osg/BoundingBox"
#include "osg/Group"

#include <vector>

class OctTree : public Usul::Base::Object
{
public:
  
  typedef Usul::Base::Object BaseClass;
  typedef osg::Vec3 Point;
  typedef Usul::Interfaces::IUnknown Unknown;
  

  USUL_DECLARE_REF_POINTERS ( OctTree );

  OctTree();
  virtual ~OctTree();

  void                add( OctTreeNode * node );
  void                insert( Point p );
  
  osg::Node*          buildScene( Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                bounds( osg::BoundingBox bb );
  
  // Get/Set tolerance level
  void                tolerance( unsigned int level );
  unsigned int        tolerance(){ return _tolerance; };

protected:
  
  void                _partition();
  

private:
  
  OctTreeNode::RefPtr          _tree;
  unsigned int                 _tolerance;



};

#endif // __EXPERIMENTAL_OCTTREE_H__