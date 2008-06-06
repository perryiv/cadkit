
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtTraversalState: Contains the traversal state.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtTraversalState.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraversalState::DbJtTraversalState() :
  _level    ( UNSET_INDEX ),
  _part     ( NULL ),
  _instance ( NULL ),
  _lod      ( UNSET_INDEX ),
  _shape    ( UNSET_INDEX ),
  _prim     ( UNSET_INDEX ),
  _set      ( UNSET_INDEX )
{
  SL_PRINT2 ( "In DbJtTraversalState::DbJtTraversalState(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraversalState::DbJtTraversalState ( const DbJtTraversalState &state ) :
  _level    ( state._level ),
  _part     ( state._part ),
  _instance ( state._instance ),
  _lod      ( state._lod ),
  _shape    ( state._shape ),
  _prim     ( state._prim ),
  _set      ( state._set )
{
  SL_PRINT2 ( "In DbJtTraversalState::DbJtTraversalState(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraversalState::~DbJtTraversalState()
{
  SL_PRINT2 ( "In DbJtTraversalState::~DbJtTraversalState(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraversalState::isEqual ( const DbJtTraversalState &state ) const
{
  SL_PRINT2 ( "In DbJtTraversalState::isEqual(), this = %X\n", this );

  return (
    _level    == state._level    &&
    _part     == state._part     &&
    _instance == state._instance &&
    _lod      == state._lod      &&
    _shape    == state._shape    &&
    _prim     == state._prim     &&
    _set      == state._set
    );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtTraversalState::setValue ( const DbJtTraversalState &state )
{
  SL_PRINT2 ( "In DbJtTraversalState::setValue(), this = %X\n", this );

  _level    = state._level;
  _part     = state._part;
  _instance = state._instance;
  _lod      = state._lod;
  _shape    = state._shape;
  _prim     = state._prim;
  _set      = state._set;
}
