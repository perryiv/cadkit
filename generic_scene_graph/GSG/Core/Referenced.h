
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

  virtual Referenced *        clone() const = 0;
  static Referenced *         safeClone ( const Referenced *cloneMe );

  virtual void                setFrom ( const Referenced & ) = 0;

  virtual const type_info &   typeId() const = 0;

  void                        ref();
  void                        unref ( bool allowDeletion = true );

  static void                 safeRef   ( Referenced * );
  static void                 safeUnref ( Referenced *, bool allowDeletion = true );

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
//  Convenience macros so that classes can easily implement setFrom().
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_SET_FROM(class_name) \
  virtual void  setFrom ( const Referenced & ); \
  void          setFrom ( const class_name & )

#define GSG_IMPLEMENT_SET_FROM(class_name) \
  void class_name::setFrom ( const Referenced &r ) \
  { \
    const class_name *c = dynamic_cast < const class_name * > ( &r ); \
    if ( c ) \
      this->setFrom ( *c ); \
    else \
      BaseClass::setFrom ( r ); \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macros so that classes can easily implement typeId().
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_TYPE_ID(class_name) \
  virtual const type_info &typeId() const

#define GSG_IMPLEMENT_TYPE_ID(class_name) \
  const type_info &class_name::typeId() const \
  { \
    return typeid ( *this ); \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Put in classes derived from Referenced.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_REFERENCED(class_name) \
  GSG_DECLARE_CLONE(class_name);\
  GSG_DECLARE_SET_FROM(class_name);\
  GSG_DECLARE_TYPE_ID(class_name)

#define GSG_IMPLEMENT_REFERENCED(class_name) \
  GSG_IMPLEMENT_CLONE(class_name);\
  GSG_IMPLEMENT_SET_FROM(class_name);\
  GSG_IMPLEMENT_TYPE_ID(class_name)


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
