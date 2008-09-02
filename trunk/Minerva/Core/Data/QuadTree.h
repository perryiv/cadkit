
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for a quad tree.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_QUAD_TREE_H__
#define __MINERVA_CORE_DATA_QUAD_TREE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "Usul/Base/Object.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace Data {
      
  class QuadTreeNode;
      
class MINERVA_EXPORT QuadTree : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  
  USUL_DECLARE_REF_POINTERS ( QuadTree );
  
  QuadTree();
  
protected:
  
  virtual ~QuadTree();
  
private:
  
  QuadTreeNode *_root;
};
      
      
}
}
}


#endif // __MINERVA_CORE_DATA_QUAD_TREE_NODE_H__
