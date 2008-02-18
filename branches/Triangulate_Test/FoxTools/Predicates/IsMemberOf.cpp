
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for checking class-type membership.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Predicates/IsMemberOf.h"
#include "FoxTools/Headers/Object.h"

using namespace FoxTools;
using namespace FoxTools::Predicates;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

IsMemberOf::IsMemberOf ( const FX::FXMetaClass *meta, bool doCount, unsigned int which ) : BaseClass(), 
  _meta    ( meta ),
  _doCount ( doCount ),
  _count   ( 0 ),
  _which   ( which )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor
//
///////////////////////////////////////////////////////////////////////////////

IsMemberOf::IsMemberOf ( const IsMemberOf &object ) : BaseClass ( object ),
  _meta    ( object._meta    ),
  _doCount ( object._doCount ),
  _count   ( object._count   ),
  _which   ( object._which   )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

IsMemberOf::~IsMemberOf()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment
//
///////////////////////////////////////////////////////////////////////////////

IsMemberOf &IsMemberOf::operator = ( const IsMemberOf &object )
{
  _meta    = object._meta;
  _doCount = object._doCount;
  _count   = object._count;
  _which   = object._which;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate operator. If we are supposed to count, then it returns true 
//  when it reached the "i'th object of type".
//
///////////////////////////////////////////////////////////////////////////////

bool IsMemberOf::operator () ( const FX::FXObject *object ) const
{
  if ( 0x0 == _meta )
    return false;

  bool result ( TRUE == object->isMemberOf ( _meta ) );

  if ( _doCount && result )
  {
    if ( _which != _count )
      result = false;
    ++_count;
  }

  return result;
}