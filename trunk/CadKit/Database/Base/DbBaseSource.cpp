
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseSource: Base class for other source database classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbBasePrecompiled.h"
#include "DbBaseSource.h"

#include "Standard/SlPrint.h"
#include "Standard/SlPathname.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

using namespace CadKit;

SL_IMPLEMENT_CLASS ( DbBaseSource, DbBaseObject );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseSource::DbBaseSource() : DbBaseObject(),
  _target ( NULL ),
  _scale ( true ),
  _needToScale ( true )
{
  SL_PRINT2 ( "In DbBaseSource::DbBaseSource(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseSource::~DbBaseSource()
{
  SL_PRINT2 ( "In DbBaseSource::~DbBaseSource(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbBaseSource::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbBaseSource::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IDataSource::IID:
    return static_cast<IDataSource *>(this);
  case IScaleDouble::IID:
    return static_cast<IScaleDouble *>(this);
  case IScaleFloat::IID:
    return static_cast<IScaleFloat *>(this);
  default:
    return DbBaseObject::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data target.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseSource::setDataTarget ( IUnknown *target )
{
  SL_PRINT3 ( "In DbBaseSource::setDataTarget(), this = %X, target = %X\n", this, target );

  // Set the target, it may be null.
  _target = target;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the current scale.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseSource::scale ( double x, double y, double z )
{
  SL_PRINT5 ( "In DbBaseSource::scale(), this = %X, x = %f, y = %f, z = %f\n", this, x, y, z );

  // Multiply the scale.
  SlMatrix44d scale;
  scale.setScale ( SlVec3d ( x, y, z ) );
  _scale *= scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the current scale.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseSource::scale ( float x, float y, float z )
{
  SL_PRINT5 ( "In DbBaseSource::scale(), this = %X, x = %f, y = %f, z = %f\n", this, x, y, z );

  // Call the other one.
  this->scale ( static_cast < float > ( x ), static_cast < float > ( y ), static_cast < float > ( z ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the scale one time until the flag gets reset.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseSource::_applyScaleOnce ( SlMatrix44d &scale ) const
{
  if ( this->_doWeNeedToScale() )
  {
    scale *= _scale;
    _needToScale = false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the scale one time until the flag gets reset.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseSource::_applyScaleOnce ( SlMatrix44f &s ) const
{
  if ( this->_doWeNeedToScale() )
  {
    // Probably not the most efficient way...
    SlMatrix44d t ( 
      s(0,0), s(0,1), s(0,2), s(0,3),
      s(1,0), s(1,1), s(1,2), s(1,3),
      s(2,0), s(2,1), s(2,2), s(2,3),
      s(3,0), s(3,1), s(3,2), s(3,3) );
    this->_applyScaleOnce ( t );
    s.setValue ( 
      static_cast<float>(t(0,0)), static_cast<float>(t(0,1)), static_cast<float>(t(0,2)), static_cast<float>(t(0,3)),
      static_cast<float>(t(1,0)), static_cast<float>(t(1,1)), static_cast<float>(t(1,2)), static_cast<float>(t(1,3)),
      static_cast<float>(t(2,0)), static_cast<float>(t(2,1)), static_cast<float>(t(2,2)), static_cast<float>(t(2,3)),
      static_cast<float>(t(3,0)), static_cast<float>(t(3,1)), static_cast<float>(t(3,2)), static_cast<float>(t(3,3)) );
  }
}
