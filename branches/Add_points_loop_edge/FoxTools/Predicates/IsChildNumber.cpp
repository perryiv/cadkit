
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for checking the child's location in the parent's list.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Predicates/IsChildNumber.h"
#include "FoxTools/Headers/Label.h"

#include "Usul/Cast/Cast.h"

#include "boost/algorithm/string/find.hpp"

using namespace FoxTools;
using namespace FoxTools::Predicates;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

IsChildNumber::IsChildNumber ( unsigned int num ) : BaseClass(),
  _num   ( num ),
  _count (   0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor
//
///////////////////////////////////////////////////////////////////////////////

IsChildNumber::IsChildNumber ( const IsChildNumber &object ) : BaseClass ( object ),
  _num   ( object._num   ),
  _count ( object._count )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

IsChildNumber::~IsChildNumber()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment
//
///////////////////////////////////////////////////////////////////////////////

IsChildNumber &IsChildNumber::operator = ( const IsChildNumber &object )
{
  _num   = object._num;
  _count = object._count;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate operator.
//
///////////////////////////////////////////////////////////////////////////////

bool IsChildNumber::operator () ( const FX::FXObject *object ) const
{
  const bool match ( _num == _count );
  ++_count;
  return match;
}
