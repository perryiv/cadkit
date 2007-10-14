
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A layer of modflow cells.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_LAYER_CLASS_H_
#define _MODFLOW_MODEL_LAYER_CLASS_H_

#include "Helios/Plugins/ModflowModel/Base/BaseObject.h"
#include "Helios/Plugins/ModflowModel/Attributes/Attribute.h"
#include "Helios/Plugins/ModflowModel/Model/Cell.h"

#include "Usul/Base/Observed.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Array"
#include "osg/Group"

#include <map>
#include <vector>


namespace Modflow {
namespace Model {


class Layer : public Modflow::Base::BaseObject,
              public Usul::Interfaces::ITreeNode,
              public Usul::Interfaces::IBooleanState,
              public Usul::Interfaces::IDirtyState,
              public Usul::Interfaces::IStringGridGet
{
public:

  // Useful typedefs.
  typedef Modflow::Base::BaseObject BaseClass;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef std::vector < Cell::RefPtr > Row;
  typedef std::vector < Row > Rows;
  typedef Cell::Vector Data;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::IDocument IDocument;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef Modflow::Attributes::Attribute Attribute;
  typedef std::vector < Attribute::RefPtr > Attributes;
  typedef osg::ref_ptr < osg::Group > GroupPtr;
  typedef Usul::Pointers::WeakPointer < Layer > WeakPtr;
  typedef BaseClass::IStringGridGet IStringGridGet;
  typedef BaseClass::StringRow StringRow;
  typedef BaseClass::StringGrid StringGrid;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Layer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Layer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Layer ( const std::string &name, unsigned int numRows, unsigned int numColumns, const Vec2d &cellSize );

  // Add the attribute.
  void                        addAttribute ( Attribute * );

  // Set/get the layer above.
  void                        above ( Layer * );
  const Layer *               above() const;
  Layer *                     above();

  // Set/get the layer below.
  void                        below ( Layer * );
  const Layer *               below() const;
  Layer *                     below();

  // Build the scene.
  osg::Node *                 buildScene ( Unknown *caller );

  // Get the cell.
  const Cell *                cell ( unsigned int row, unsigned int col ) const;
  Cell *                      cell ( unsigned int row, unsigned int col );

  // Get the cell below.
  const Cell *                cellBelow ( unsigned int row, unsigned int col ) const;
  Cell *                      cellBelow ( unsigned int row, unsigned int col );

  // Get the cell size.
  Vec2d                       cellSize() const;

  // Clear the layer.
  void                        clear();

  // Set/get the dirty flag.
  void                        dirty ( bool state ) { this->dirtyState ( state ); }
  bool                        dirty() const { return this->dirtyState(); }

  // Set/get the dirty flag.
  virtual void                dirtyState ( bool );
  virtual bool                dirtyState() const;

  // Set the document.
  void                        document ( Usul::Interfaces::IUnknown * );

  // Set/get the flag.
  void                        flags ( unsigned int );
  unsigned int                flags() const;

  // Return the grid size.
  Vec2ui                      gridSize() const;

  // Set/get the margin.
  void                        margin ( double x, double y, double z );
  Vec3d                       margin() const;

  // Set modification flag.
  void                        modified ( bool );

  // Purge all the cells that are outside the bounds.
  void                        purge ( const Data &bounds );

  // Set all the values for the given time step.
  void                        vector ( const std::string &name, unsigned int timeStep, const Data &heads );

  // Set/get the visible state for the layer.
  bool                        visible() const;
  void                        visible ( bool state );

  // Set cell's tops and bottoms.
  void                        zRange ( const Data &top, const Data &bottom );

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int        getNumChildNodes() const;
  virtual const ITreeNode *   getChildNode ( unsigned int which ) const;
  virtual ITreeNode *         getChildNode ( unsigned int which );
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );
  virtual bool                getBooleanState() const;

  // Usul::Interfaces::IStringGridGet.
  virtual void                getStringGrid ( StringGrid & ) const;

protected:

  // Use reference counting.
  virtual ~Layer();

  osg::Node *                 _buildCubes ( Unknown *caller ) const;
  void                        _buildScene ( Unknown *caller );

private:

  // Do not copy.
  Layer ( const Layer & );
  Layer &operator = ( const Layer & );

  Rows _rows;
  Vec2d _cellSize;
  Layer::RefPtr _above;
  Layer::RefPtr _below;
  GroupPtr _root;
  unsigned int _flags;
  IDocument::QueryPtr _document;
  Vec3d _margin;
  Attributes _attributes;
};


} // namespace Model
} // namespace Modflow


#endif // _MODFLOW_MODEL_LAYER_CLASS_H_
