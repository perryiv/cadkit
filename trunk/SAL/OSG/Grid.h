
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Grid class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_SAL_GRID_CLASS_H_
#define _OSG_SAL_GRID_CLASS_H_

#include "Usul/Base/Referenced.h"

#include "SAL/Interfaces/IGrid.h"
#include "SAL/Interfaces/IColorRGBA.h"
#include "SAL/Interfaces/IBuilder.h"
#include "SAL/Interfaces/INode.h"


namespace SAL {
namespace OSG {


class Grid : public Usul::Base::Referenced,
             public SAL::Interfaces::IGridFloat,
             public SAL::Interfaces::IColorFloatRGBA,
             public SAL::Interfaces::IBuilder
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Grid );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Grid();

protected:

  typedef SAL::Interfaces::IColorFloatRGBA::Color Color;
  typedef SAL::Interfaces::IGridFloat::Vec2 Vec2;
  typedef SAL::Interfaces::IGridFloat::Vec3 Vec3;
  typedef SAL::Interfaces::IGridFloat::Vec2ui Vec2ui;
  typedef SAL::Interfaces::INode INode;

  /// Do not copy.
  Grid ( const Grid & );
  Grid &operator = ( const Grid & );

  /// Use reference counting.
  virtual ~Grid();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IGridFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the center of the grid.
  virtual void            setGridCenter ( const Vec3 &c );
  virtual void            getGridCenter ( Vec3 &c ) const;

  // Set/get the number of grid blocks.
  virtual void            setNumGridBlocks ( const Vec2ui &num );
  virtual void            getNumGridBlocks ( Vec2ui &num ) const;

  // Set/get the grid size.
  virtual void            setGridSize ( const Vec2 &s );
  virtual void            getGridSize ( Vec2 &s ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IColorFloatRGBA
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the color.
  virtual void            setColorRGBA ( const Color &rgba );
  virtual void            getColorRGBA ( Color &rgba ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IBuilder
  //
  /////////////////////////////////////////////////////////////////////////////

  // Build the scene.
  virtual INode *         buildScene() const;

private:

  Vec2ui _numBlocks;
  Vec2 _size;
  Vec3 _center;
  Color _color;
};


}; // namespace OSG
}; // namespace SAL


#endif // _OSG_SAL_GRID_CLASS_H_
