
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
#include "Minerva/Core/Data/QuadTreeNode.h"

#include "Usul/Base/Object.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace Data {
      
class MINERVA_EXPORT QuadTree : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  typedef QuadTreeKey::RefPtr KeyPtr;
  typedef std::vector<KeyPtr> Keys;
  
  USUL_DECLARE_REF_POINTERS ( QuadTree );
  
  QuadTree ( const Extents& e );
  
  void                clear();
  
  /// Insert a key.
  void                insert ( KeyPtr key );
  
  // Get all the keys that are contained by the extents.
  Keys                query ( const Extents& e ) const;
  void                query ( const Extents& e, Keys& keys ) const;
  
protected:
  
  virtual ~QuadTree();
  
private:
  
  QuadTree();
  
  QuadTreeNode::RefPtr _root;
};
      
      
}
}
}


#endif // __MINERVA_CORE_DATA_QUAD_TREE_NODE_H__
