
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

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSAL_STATE_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSAL_STATE_H_

#include "DbJtApi.h"
#include "DbJtVisApiHeaders.h"

#include "Standard/SlRefPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif


namespace CadKit
{
const int UNSET_INDEX = -1;

class DB_JT_API DbJtTraversalState
{
public:

  DbJtTraversalState();
  DbJtTraversalState ( const DbJtTraversalState &state );
  ~DbJtTraversalState();

  // Get the values.
  const eaiPart *         getPart() const     { return _part; }
  eaiPart *               getPart()           { return _part; }
  const eaiInstance *     getInstance() const { return _instance; }
  eaiInstance *           getInstance()       { return _instance; }
  int                     getLevel() const    { return _level; }
  int                     getLod() const      { return _lod; }
  int                     getShape() const    { return _shape; }
  int                     getPrim() const     { return _prim; }
  int                     getSet() const      { return _set; }
 
  // Equality.
  bool                    isEqual    ( const DbJtTraversalState &state ) const;
  bool                    isNotEqual ( const DbJtTraversalState &state ) const { return false == this->isEqual ( state ); }

  // Assignment.
  DbJtTraversalState &    operator = ( const DbJtTraversalState &rhs ) { this->setValue ( rhs ); return *this; }

  // Equality.
  friend bool             operator == ( const DbJtTraversalState &lhs, const DbJtTraversalState &rhs ) { return lhs.isEqual ( rhs ); }
  friend bool             operator != ( const DbJtTraversalState &lhs, const DbJtTraversalState &rhs ) { return lhs.isNotEqual ( rhs ); }

  // Set the values.
  void                    setPart     ( eaiPart *part )         { _part = part; }
  void                    setInstance ( eaiInstance *instance ) { _instance = instance; }
  void                    setLevel    ( const int &level )      { _level = level; }
  void                    setLod      ( const int &lod )        { _lod   = lod; }
  void                    setShape    ( const int &shape )      { _shape = shape; }
  void                    setPrim     ( const int &prim )       { _prim = prim; }
  void                    setSet      ( const int &set )        { _set   = set; }
  void                    setValue    ( const DbJtTraversalState &state );

protected:

  int _level;
  SlRefPtr<eaiPart> _part;
  SlRefPtr<eaiInstance> _instance;
  int _lod;
  int _shape;
  int _prim;
  int _set;
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSAL_STATE_H_
