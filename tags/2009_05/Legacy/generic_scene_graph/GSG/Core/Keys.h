
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Keeps track of the keyboard state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_KEY_STATES_H_
#define _GENERIC_SCENE_GRAPH_CORE_KEY_STATES_H_

#include "GSG/Core/Referenced.h"

#include <map>


namespace GSG {

class Node;


class GSG_CORE_EXPORT Keys : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Keys );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Keys, Referenced );

  enum State { UP, DOWN, UNKNOWN };

  explicit Keys();
  Keys ( const Keys &v );

  // Set this key as down/up.
  void            down  ( UnsignedInteger c );
  void            up    ( UnsignedInteger c );

  // Get the state of the key.
  State           state ( UnsignedInteger c ) const;

protected:

  virtual ~Keys();

  typedef std::map < UnsignedInteger, State > States;
  States _s;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_KEY_STATES_H_
