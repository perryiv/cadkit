
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IPOLYGON_LAYER_H__
#define __MINERVA_INTERFACES_IPOLYGON_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IPolygonLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPolygonLayer );

  /// Id for this interface.
  enum { IID = 4030984299u };

  /// Get/Set the show interior flag.
  virtual void                    showInterior( bool b ) = 0;
  virtual bool                    showInterior() const = 0;

  /// Get/Set the show border flag.
  virtual void                    showBorder( bool b ) = 0;
  virtual bool                    showBorder() const = 0;
  
    /// Get/Set the border width.
  virtual void                    borderWidth ( float width ) = 0;
  virtual float                   borderWidth () const = 0;

}; // struct IPolygonLayer


} // end namespace Interfaces
} // end namespace Usul


#endif /* __MINERVA_INTERFACES_IPOLYGON_LAYER_H__ */
