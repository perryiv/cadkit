
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
#include "Cell.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Array"

#include <vector>


class Layer : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef std::vector < Cell::RefPtr > Row;
  typedef std::vector < Row > Rows;
  typedef Cell::Vector Data;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Layer );

  // Construction.
  Layer ( unsigned int numRows, unsigned int numColumns, const Vec2d &cellSize );

  // Set/get the layer above.
  void                    above ( Layer * );
  const Layer *           above() const;
  Layer *                 above();

  // Set/get the layer below.
  void                    below ( Layer * );
  const Layer *           below() const;
  Layer *                 below();

  // Build the scene.
  osg::Node *             buildScene ( unsigned int flags, double sizeFactor, Unknown *caller ) const;

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

  // Return the grid size.
  Vec2ui                  gridSize() const;

  // Set all the values for the given time step.
  void                    vector ( const std::string &name, unsigned int timeStep, const Data &heads );

  // Purge all the cells that are outside the bounds.
  void                    purge ( const Data &bounds );

  // Set cell's tops and bottoms.
  void                    zRange ( const Data &top, const Data &bottom );

protected:

  // Use reference counting.
  virtual ~Layer();

  osg::Node *             _buildCubes ( unsigned int flags, double sizeFactor, Unknown *caller ) const;

private:

  // Do not copy.
  Layer ( const Layer & );
  Layer &operator = ( const Layer & );

  Rows _rows;
  Vec2d _cellSize;
  Layer::RefPtr _above;
  Layer::RefPtr _below;
};


#endif // _MODFLOW_MODEL_LAYER_CLASS_H_
