
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcSurface.h: NURBS surface class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SURFACE_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SURFACE_CLASS_H_

#include "NcSpline.h"


namespace CadKit
{
template<NCSDTCD> class NcSurface : public NcSpline<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  NcSurface(){}
  NcSurface ( const NcSurface<NCSDCA> &sd ) : NcSpline<NCSDCA> ( sd ){}
  ~NcSurface(){}

  // Assignment operator.
  NcSurface &                   operator = ( const NcSurface &sd ) { this->setValue ( sd ); return *this; }
};


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SURFACE_CLASS_H_
