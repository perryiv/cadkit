
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

#include "CompileGuard.h"
#include "Attribute.h"
#include "Cell.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ILayerList.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Array"
#include "osg/Group"

#include <vector>


class Layer : public Usul::Base::Object,
              public Usul::Interfaces::ILayer,
              public Usul::Interfaces::ILayerList
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef std::vector < Cell::RefPtr > Row;
  typedef std::vector < Row > Rows;
  typedef Cell::Vector Data;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::IDocument IDocument;
  typedef Usul::Interfaces::ILayer ILayer;
  typedef Usul::Interfaces::ILayerList ILayerList;
  typedef std::vector < BaseAttribute::RefPtr > Attributes;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Layer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Layer ( const std::string &name, unsigned int numRows, unsigned int numColumns, const Vec2d &cellSize );

  // Add the attribute.
  void                    addAttribute ( BaseAttribute * );

  // Set/get the layer above.
  void                    above ( Layer * );
  const Layer *           above() const;
  Layer *                 above();

  // Set/get the layer below.
  void                    below ( Layer * );
  const Layer *           below() const;
  Layer *                 below();

  // Build the scene.
  osg::Node *             buildScene ( Unknown *caller );

  // Get the cell below.
  const Cell *            cell ( unsigned int row, unsigned int col ) const;
  Cell *                  cell ( unsigned int row, unsigned int col );

  // Get the cell below.
  const Cell *            cellBelow ( unsigned int row, unsigned int col ) const;
  Cell *                  cellBelow ( unsigned int row, unsigned int col );

  // Get the cell size.
  Vec2d                   cellSize() const;

  // Clear the layer.
  void                    clear();

  // Set/get the dirty flag.
  void                    dirty ( bool );
  bool                    dirty() const;

  // Set the document.
  void                    document ( Usul::Interfaces::IUnknown * );

  // Set/get the flag.
  void                    flags ( unsigned int );
  unsigned int            flags() const;

  // Return the grid size.
  Vec2ui                  gridSize() const;

  // Set/get the margin.
  void                    margin ( double x, double y, double z );
  Vec3d                   margin() const;

  // Set modification flag.
  void                    modified ( bool );

  // Get the number of layers.
  virtual unsigned int    numberLayers() const;

  /// Get the layer at position i.
  virtual ILayer *        layer ( unsigned int i );

  // Purge all the cells that are outside the bounds.
  void                    purge ( const Data &bounds );

  // Set all the values for the given time step.
  void                    vector ( const std::string &name, unsigned int timeStep, const Data &heads );

  // Set/get the visible state for the names attribute.
  // Pass empty string fo entire layer.
  bool                    visible ( const std::string &name = "" ) const;
  void                    visible ( bool state, const std::string &name = "" );

  // Set cell's tops and bottoms.
  void                    zRange ( const Data &top, const Data &bottom );

  // Usul::Interfaces::ILayer
  virtual std::string     guid() const;
  virtual std::string     name() const;
  virtual void            showLayer ( bool );
  virtual bool            showLayer() const;

protected:

  // Use reference counting.
  virtual ~Layer();

  osg::Node *             _buildCubes ( Unknown *caller ) const;
  void                    _buildScene ( Unknown *caller );

private:

  // Do not copy.
  Layer ( const Layer & );
  Layer &operator = ( const Layer & );

  Rows _rows;
  Vec2d _cellSize;
  Layer::RefPtr _above;
  Layer::RefPtr _below;
  std::string _guid;
  osg::ref_ptr<osg::Group> _root;
  unsigned int _flags;
  IDocument::QueryPtr _document;
  Vec3d _margin;
  Attributes _attributes;
};


#endif // _MODFLOW_MODEL_LAYER_CLASS_H_
