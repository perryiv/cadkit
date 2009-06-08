
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
//  Base class for attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_

#include "ModflowModel/Base/BaseObject.h"

#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Math/Vector4.h"

#include "osg/Group"

#include <vector>

namespace Modflow { class ModflowDocument; namespace Model { class Layer; class Cell; } }


namespace Modflow {
namespace Attributes {


class Attribute : public Modflow::Base::BaseObject,
                  public Usul::Interfaces::ITreeNode,
                  public Usul::Interfaces::IBooleanState,
                  public Usul::Interfaces::IDirtyState,
                  public Usul::Interfaces::IStringGridGet
{
public:

  // Type information.
  USUL_DECLARE_TYPE_ID ( Attribute );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Attribute );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Modflow::Base::BaseObject BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef osg::ref_ptr < osg::Group > GroupPtr;
  typedef Usul::Pointers::WeakPointer < Attribute > WeakPtr;
  typedef BaseClass::IStringGridGet IStringGridGet;
  typedef BaseClass::StringRow StringRow;
  typedef BaseClass::StringGrid StringGrid;
  typedef Modflow::Base::BaseObject::WeakPtr CellWeakPtr;
  typedef std::vector < CellWeakPtr > Cells;
  typedef Usul::Math::Vec4f Color;
  typedef std::vector<std::string> RegistryPath;

  // Construction.
  Attribute ( const std::string &name, IUnknown *parent, const RegistryPath & );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::ModflowDocument *, Modflow::Model::Layer *, IUnknown *caller );

  // Clear the attribute.
  virtual void                clear();

  // Set the color.
  void                        color ( const std::string & );

  // Set/get the dirty flag.
  virtual void                dirtyState ( bool );
  virtual bool                dirtyState() const;

  // Initialize the visible state.
  void                        initVisibleState();

  // Update this attribute.
  virtual void                update ( Usul::Interfaces::IUnknown *caller );

  // Set/get the visible flag.
  void                        visible ( bool );
  bool                        visible() const;

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int        getNumChildNodes() const;
  virtual ITreeNode::RefPtr   getChildNode ( unsigned int which );
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );
  virtual bool                getBooleanState() const;

  // Usul::Interfaces::IStringGridGet.
  virtual void                getStringGrid ( IStringGridGet::StringGrid & ) const;

  // Get the cell from the vertex.
  Modflow::Model::Cell *      getCellAtVertex ( unsigned int which );

  // Set/get the min/max color.
  virtual void                maxColor ( const Color &c );
  Color                       maxColor() const;
  virtual void                minColor ( const Color &c );
  Color                       minColor() const;

  // Set the registry path.
  void                        registryPath ( const RegistryPath & );

protected:

  // Use reference counting.
  virtual ~Attribute();

  void                        _cellWeakPointersClear();
  void                        _cellWeakPointersReserve ( unsigned int );
  void                        _cellWeakPointersAppend ( Modflow::Model::Cell *, unsigned int num );
  void                        _cellWeakPointersTrim();

  IUnknown *                  _getParent();

  GroupPtr                    _getScene();
  void                        _setScene ( osg::Group * );

private:

  // Do not copy.
  Attribute ( const Attribute & );
  Attribute &operator = ( const Attribute & );

  void                        _destroy();

  unsigned int _flags;
  GroupPtr _scene;
  IUnknown *_parent;
  Cells _cells;
  Color _minColor;
  Color _maxColor;
  RegistryPath _registryPath;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
