
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single modflow cell.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Model/Cell.h"
#include "Helios/Plugins/ModflowModel/Constants.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

using namespace Modflow::Model;

USUL_IMPLEMENT_TYPE_ID ( Cell );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Cell::Cell ( double x, double y ) : BaseClass ( "" ),
  _x       ( x ),
  _y       ( y ),
  _bottom  ( 0 ),
  _top     ( 0 ),
  _vectors (),
  _values  ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Cell::~Cell()
{
  USUL_TRACE_SCOPE;
  _vectors.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set cell top
//
///////////////////////////////////////////////////////////////////////////////

void Cell::top ( double t )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _top = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get cell top.
//
///////////////////////////////////////////////////////////////////////////////

double Cell::top() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _top;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set cell bottom.
//
///////////////////////////////////////////////////////////////////////////////

void Cell::bottom ( double b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _bottom = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bottom's z coordinate.
//
///////////////////////////////////////////////////////////////////////////////

double Cell::bottom() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _bottom;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the coordinate.
//
///////////////////////////////////////////////////////////////////////////////

double Cell::x() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _x;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the coordinate.
//
///////////////////////////////////////////////////////////////////////////////

void Cell::x ( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _x = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the coordinate.
//
///////////////////////////////////////////////////////////////////////////////

double Cell::y() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _y;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the coordinate.
//
///////////////////////////////////////////////////////////////////////////////

void Cell::y ( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _y = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

Cell::Vec3d Cell::center() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Vec3d ( _x, _y, 0.5 * ( _top - _bottom ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the named value.
//
///////////////////////////////////////////////////////////////////////////////

double Cell::value ( const std::string &name ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Cell &me ( const_cast < Cell & > ( *this ) );
  return me._values[name];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the named value.
//
///////////////////////////////////////////////////////////////////////////////

void Cell::value ( const std::string &name, double v )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _values[name] = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the named vector.
//
///////////////////////////////////////////////////////////////////////////////

const Cell::Vector &Cell::vector ( const std::string &name ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Cell &me ( const_cast < Cell & > ( *this ) );
  return me._vectors[name];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the named vector.
//
///////////////////////////////////////////////////////////////////////////////

Cell::Vector &Cell::vector ( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _vectors[name];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the named vector.
//
///////////////////////////////////////////////////////////////////////////////

void Cell::vector ( const std::string &name, const Vector &v )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _vectors[name] = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the cell.
//
///////////////////////////////////////////////////////////////////////////////

void Cell::clear()
{
  USUL_TRACE_SCOPE;
  // Nothing to clear, just plumbing.
}
