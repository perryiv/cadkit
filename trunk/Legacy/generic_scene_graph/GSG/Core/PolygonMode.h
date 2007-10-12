
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A polygon-mode class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_POLYGON_MODE_CLASS_H_
#define _GENERIC_SCENE_GRAPH_CORE_POLYGON_MODE_CLASS_H_

#include "GSG/Core/Attribute.h"


namespace GSG {


class GSG_CORE_EXPORT PolygonMode : public Attribute
{
public:

  GSG_DECLARE_REFERENCED ( PolygonMode );
  GSG_DECLARE_LOCAL_TYPEDEFS ( PolygonMode, Attribute );

  // Possible modes.
  enum Mode { POINT, LINE, FILL };

  // Constructors
  explicit PolygonMode ( Mode m = FILL );
  PolygonMode ( const PolygonMode &m );

  // Get/set the mode.
  Mode                    mode() const { return _mode; }
  void                    mode ( Mode m );

  // Are they equal?
  bool                    equal ( const PolygonMode &m ) const { return m._mode == _mode; }

  // Assignment
  PolygonMode &           operator = ( const PolygonMode &rhs );

protected:

  virtual ~PolygonMode();

private:

  Mode _mode;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

inline bool operator == ( const PolygonMode &left, const PolygonMode &right )
{
  return left.equal ( right );
}
inline bool operator != ( const PolygonMode &left, const PolygonMode &right )
{
  return false == left.equal ( right );
}


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_POLYGON_MODE_CLASS_H_
