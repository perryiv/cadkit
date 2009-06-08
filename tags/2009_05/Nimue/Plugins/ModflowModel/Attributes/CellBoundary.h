
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cell boundary attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_ATTRIBUTES_CELL_BOUNDARY_CLASS_H_
#define _MODFLOW_ATTRIBUTES_CELL_BOUNDARY_CLASS_H_

#include "ModflowModel/Attributes/Quads.h"

#include <vector>

namespace Modflow {
namespace Attributes {


class CellBoundary : public Modflow::Attributes::Attribute
{
public:

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef std::vector<Quads::RefPtr> Sides;
  typedef Usul::Pointers::WeakPointer < CellBoundary > WeakPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( CellBoundary );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CellBoundary );

  // Construction.
  CellBoundary ( const std::string &name, Modflow::Geometry::ID topGeometry, IUnknown *parent, const RegistryPath & );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::ModflowDocument *, Modflow::Model::Layer *, IUnknown *caller );

  // Clear the attribute.
  virtual void                clear();

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int        getNumChildNodes() const;
  virtual ITreeNode::RefPtr   getChildNode ( unsigned int which );

  // Usul::Interfaces::IStringGridGet.
  virtual void                getStringGrid ( IStringGridGet::StringGrid & ) const;

  // Set the min/max color.
  virtual void                maxColor ( const Color &c );
  virtual void                minColor ( const Color &c );

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );

protected:

  // Use reference counting.
  virtual ~CellBoundary();

private:

  // Do not copy.
  CellBoundary ( const CellBoundary & );
  CellBoundary &operator = ( const CellBoundary & );

  void                        _destroy();

  Sides _sides;
  GroupPtr _group;
  Modflow::Geometry::ID _topGeometry;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_ATTRIBUTES_CELL_BOUNDARY_CLASS_H_
