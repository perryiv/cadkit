
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The state attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTE_SET_H_
#define _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTE_SET_H_

#include "GSG/Core/Attribute.h"


namespace GSG {


class GSG_CORE_EXPORT AttributeSet : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( AttributeSet );
  GSG_DECLARE_LOCAL_TYPEDEFS ( AttributeSet, Referenced );
  typedef GSG_BIDIRECTIONAL_CONTAINER < Attribute::ValidPtr > Attributes;
  typedef Attributes::size_type size_type;
  typedef Attributes::iterator iterator;
  typedef Attributes::const_iterator const_iterator;
  typedef Attributes::reference reference;
  typedef Attributes::const_reference const_reference;

  explicit AttributeSet();
  AttributeSet ( const AttributeSet &a );

  // Iterators to the primitives.
  const_iterator          begin() const { return _attributes.begin(); }
  iterator                begin()       { return _attributes.begin(); }
  const_iterator          end() const   { return _attributes.end(); }
  iterator                end()         { return _attributes.end(); }

  // Find the attribute.
  iterator                find ( const type_info &t );
  const_iterator          find ( const type_info &t ) const;

  // Remove any attribute of the given type.
  void                    remove ( const type_info & );

  // Set the attribute. Replaces any existing attributes of the same type.
  void                    set ( Attribute * );

protected:

  virtual ~AttributeSet();

private:

  Attributes _attributes;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTE_SET_H_
