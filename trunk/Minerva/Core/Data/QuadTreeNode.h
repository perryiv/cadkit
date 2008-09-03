
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
#include "Usul/Math/Vector3.h"

#include "osg/Vec2d"

#include <vector>

namespace Minerva {
namespace Core {
namespace Data {

class MINERVA_EXPORT QuadTreeKey : public Usul::Base::Referenced
{
public:
  
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec3d Vertex;
  
  USUL_DECLARE_REF_POINTERS ( QuadTreeKey );
  
  QuadTreeKey ( Vertex* vertex, unsigned int index );
  
  const Vertex * const vertex() const { return _vertex; }
  unsigned int         index() const { return _index; }
  
protected:
  
  virtual ~QuadTreeKey();
  
private:
  
  QuadTreeKey();
  
  Vertex *_vertex;
  unsigned int _index;
};

class MINERVA_EXPORT QuadTreeNode : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  typedef QuadTreeKey::RefPtr KeyPtr;
  typedef std::vector<KeyPtr> Keys;

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
  
  /// Clear.
  void                clear();

  /// Get the extents.
  Extents             extents() const;
  
  /// Insert a key.
  void                insert ( KeyPtr key );

  /// Is this node a leaf?
  bool                isLeaf() const;
  
  // Get all the keys that are contained by the extents.
  Keys                query ( const Extents& e ) const;
  void                query ( const Extents& e, Keys& keys ) const;

protected:
  
  QuadTreeNode();
  
  virtual ~QuadTreeNode();
  
  /// Does this nood need to be split?
  bool                _needsSplit() const;
  
  /// Split into four nodes.
  void                _split();
  
private:
  
  typedef std::vector<QuadTreeNode::RefPtr> Children;
  
  Extents _extents;
  Children _children;
  Keys _keys;
};
 

}
}
}

#endif // __MINERVA_CORE_DATA_QUAD_TREE_NODE_H__
