
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Compile-time type checking.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_TYPE_CHECK_H_
#define _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_TYPE_CHECK_H_

#include "GN/Traits/TypeTags.h"


namespace GN {
namespace MPL {


///////////////////////////////////////////////////////////////////////////////
//
//  For compile-time checking to make sure the type can be a curve. 
//  A compile-error here typically means that this class is being 
//  instantiated with the wrong type.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct CanBeCurve;
template <> struct CanBeCurve < GN::Traits::Curve >{};
template <> struct CanBeCurve < GN::Traits::Spline >{};


///////////////////////////////////////////////////////////////////////////////
//
//  For compile-time checking to make sure the type can be a surface. 
//  A compile-error here typically means that this class is being 
//  instantiated with the wrong type.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct CanBeSurface;
template <> struct CanBeSurface < GN::Traits::Surface >{};
template <> struct CanBeSurface < GN::Traits::Spline >{};


///////////////////////////////////////////////////////////////////////////////
//
//  For compile-time checking to make sure the type is a curve. 
//  A compile-error here typically means that this class is being 
//  instantiated with the wrong type.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct IsCurve;
template <> struct IsCurve < GN::Traits::Curve >{};


///////////////////////////////////////////////////////////////////////////////
//
//  For compile-time checking to make sure the type is a surface. 
//  A compile-error here typically means that this class is being 
//  instantiated with the wrong type.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct IsSurface;
template <> struct IsSurface < GN::Traits::Surface >{};


}; // namespace MPL
}; // namespace GN


///////////////////////////////////////////////////////////////////////////////
//
//  Macros for convenient use of above classes. If you get a compile error 
//  with these macros then you are most likely using an incompatable types 
//  in a template.
//
///////////////////////////////////////////////////////////////////////////////

#define GN_CAN_BE_CURVE(spline_type) \
  typedef typename spline_type::TypeTag TypeTag_; \
  typedef GN::MPL::CanBeCurve<TypeTag_> CanBeCurve_; \
  CanBeCurve_()
#define GN_CAN_BE_SURFACE(spline_type) \
  typedef typename spline_type::TypeTag TypeTag_; \
  typedef GN::MPL::CanBeSurface<TypeTag_> CanBeSurface_; \
  CanBeSurface_()
#define GN_IS_CURVE(spline_type) \
  typedef typename spline_type::TypeTag TypeTag_; \
  typedef GN::MPL::IsCurve<TypeTag_> IsCurve_; \
  IsCurve_()
#define GN_IS_SURFACE(spline_type) \
  typedef typename spline_type::TypeTag TypeTag_; \
  typedef GN::MPL::IsSurface<TypeTag_> IsSurface_; \
  IsSurface_()


#endif // _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_TYPE_CHECK_H_
