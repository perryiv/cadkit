
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
//  Class for node of a quad tree.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_QUAD_TREE_NODE_H__
#define __MINERVA_CORE_DATA_QUAD_TREE_NODE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "Usul/Base/Object.h"

#include "osg/Vec2d"

#include <vector>

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT QuadTreeNode : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  
  USUL_DECLARE_REF_POINTERS ( QuadTreeNode );
  
  // Indices for children
  enum Indices
  {
    LOWER_LEFT = 0,
    LOWER_RIGHT = 1,
    UPPER_LEFT = 2,
    UPPER_RIGHT = 3
  };
  
  QuadTreeNode ( const Extents& e );
  
  /// Get the extents.
  Extents             extents() const;
  
protected:
  
  QuadTreeNode();
  
  virtual ~QuadTreeNode();
  
private:
  
  typedef std::vector<QuadTreeNode::RefPtr> Children;
  
  Extents _extents;
  Children _children;
};
 

}
}
}

#endif // __MINERVA_CORE_DATA_QUAD_TREE_NODE_H__
