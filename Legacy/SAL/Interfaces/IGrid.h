
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a grid.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_GRID_H_
#define _SAL_INTERFACE_GRID_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"


namespace SAL {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IGrid : public Usul::Interfaces::IUnknown
{
  typedef Usul::Math::Vector2<RealType> Vec2;
  typedef Usul::Math::Vector3<RealType> Vec3;
  typedef Usul::Math::Vec2ui Vec2ui;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGrid );

  /// Id for this interface.
  enum { IID = iid };

  // Set/get the center of the grid.
  virtual void            setGridCenter ( const Vec3 &c ) = 0;
  virtual void            getGridCenter ( Vec3 &c ) const = 0;

  // Set/get the number of grid blocks.
  virtual void            setNumGridBlocks ( const Vec2ui &num ) = 0;
  virtual void            getNumGridBlocks ( Vec2ui &num ) const = 0;

  // Set/get the grid size.
  virtual void            setGridSize ( const Vec2 &s ) = 0;
  virtual void            getGridSize ( Vec2 &s ) const = 0;
};


typedef IGrid<2819734136u,float>  IGridFloat;
typedef IGrid<2910313393u,double> IGridDouble;


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_GRID_H_
