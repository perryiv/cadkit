
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counting class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_REFERENCED_H_
#define _GENERIC_SCENE_GRAPH_CORE_REFERENCED_H_

#include "GSG/Core/Config.h"


namespace GSG {


class GSG_CORE_EXPORT Referenced : public RootClass
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( Referenced, RootClass );

  virtual Referenced *  clone() const = 0;
  static Referenced *   safeClone ( const Referenced *cloneMe );

  void                  ref();
  void                  unref ( bool allowDeletion = true );

protected:

  explicit Referenced();
  Referenced ( const Referenced &r );
  virtual ~Referenced();

  ReferenceCount _refCount;
};


}; // namespace GSG


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macros so that classes can easily implement clone().
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_CLONE(class_name) \
  virtual Referenced *clone() const; \
  class_name *clone##class_name() const; \
  static class_name *safeClone##class_name ( const class_name *cloneMe )

#define GSG_IMPLEMENT_CLONE(class_name) \
  Referenced *class_name::clone() const \
  { \
    return new class_name ( *this ); \
  } \
  class_name *class_name::clone##class_name() const \
  { \
    return dynamic_cast < class_name * > ( this->clone() ); \
  } \
  class_name *class_name::safeClone##class_name ( const class_name *cloneMe ) \
  { \
    return dynamic_cast < class_name * > ( Referenced::safeClone ( cloneMe ) ); \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  For use with smart pointer classes.
//
///////////////////////////////////////////////////////////////////////////////

namespace GSG
{
  namespace Detail
  {
    GSG_CORE_EXPORT void intrusive_ptr_add_ref ( GSG::Referenced *ptr );
    GSG_CORE_EXPORT void intrusive_ptr_release ( GSG::Referenced *ptr );
    GSG_CORE_EXPORT void ptr_release_no_delete ( GSG::Referenced *ptr );
  };
};


#endif // _GENERIC_SCENE_GRAPH_CORE_REFERENCED_H_
