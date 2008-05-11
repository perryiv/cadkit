
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to notify of an elevation change.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IELEVATION_CHANGED_LISTENER_H__
#define __MINERVA_INTERFACES_IELEVATION_CHANGED_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Minerva/Core/Extents.h"

#include "osg/Image"
#include "osg/ref_ptr"
#include "osg/Vec2d"

namespace Minerva {
namespace Interfaces {


struct IElevationChangedListnerer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IElevationChangedListnerer );

  /// Id for this interface.
  enum { IID = 2421981627u };

  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  typedef osg::ref_ptr<osg::Image> ImagePtr;

  /// Elevation has changed within given extents.
  virtual bool elevationChangedNotify ( const Extents& extents, ImagePtr elevationData, Usul::Interfaces::IUnknown * caller = 0x0 ) = 0;

}; // struct IElevationChangedListnerer


} // End namespace Interfaces
} // End namespace Minerva

#endif // __MINERVA_INTERFACES_IELEVATION_CHANGED_LISTENER_H__
