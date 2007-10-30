
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Extents.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Extents::Extents() :
  _mutex ( new Mutex ),
  _min ( 0.0, 0.0 ),
  _max ( 0.0, 0.0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Extents::Extents( const Vertex& min, const Vertex& max ) :
  _mutex ( new Mutex ),
  _min ( min ),
  _max ( max )
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Extents::Extents( const Extents& rhs ) :
  _mutex ( new Mutex ),
  _min ( rhs._min ),
  _max ( rhs._max )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Extents& Extents::operator= ( const Extents& rhs )
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Extents::_destroy ), "4155437642" );  
  _mutex = new Mutex;
  _min = rhs._min;
  _max = rhs._max;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Extents::~Extents()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Extents::_destroy ), "2484159931" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destrory.
//
///////////////////////////////////////////////////////////////////////////////

void Extents::_destroy()
{
  if ( 0x0 != _mutex )
    delete _mutex;
  _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min.
//
///////////////////////////////////////////////////////////////////////////////

Extents::Vertex Extents::min() const
{
  Guard guard ( this );
  return _min;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max.
//
///////////////////////////////////////////////////////////////////////////////

Extents::Vertex Extents::max() const
{
  Guard guard ( this );
  return _max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Expand by the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Extents::expand ( const Extents& extents )
{
  Vertex zero ( 0.0, 0.0 );

  // If we are invalid.
  if ( zero == this->min() && zero == this->max () )
  {
    Guard guard ( this );
    _min = extents.min();
    _max = extents.max();
  }
  else
  {
    this->min()[0] = Usul::Math::minimum ( this->min()[0], extents.min()[0] );
    this->min()[1] = Usul::Math::minimum ( this->min()[1], extents.min()[1] );

    this->max()[0] = Usul::Math::maximum ( this->max()[0], extents.max()[0] );
    this->max()[1] = Usul::Math::maximum ( this->max()[1], extents.max()[1] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the extent intersect this extent.
//
///////////////////////////////////////////////////////////////////////////////

bool Extents::intersects ( const Extents& extents ) const
{
  Guard guard ( this );

#if 0
  double minLon ( Usul::Math::minimum ( _min[0], extents.min()[0] ) );
  double maxLon ( Usul::Math::maximum ( _max[0], extents.max()[0] ) );
  double minLat ( Usul::Math::minimum ( _min[1], extents.min()[1] ) );
  double maxLat ( Usul::Math::maximum ( _max[1], extents.max()[1] ) );

  return ( maxLon <= minLon ) && ( maxLat <= minLat );
#endif
  return Usul::Math::maximum ( _min[0], extents.min()[0] ) <= Usul::Math::minimum ( _max[0], extents.max()[0] ) &&
         Usul::Math::maximum ( _min[1], extents.min()[1] ) <= Usul::Math::minimum ( _max[1], extents.max()[1] );
}
