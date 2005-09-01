
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_POLYGON_MODE_H_
#define _USUL_INTERFACE_POLYGON_MODE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IPolygonMode : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPolygonMode );

  /// Id for this interface.
  enum { IID = 2837403757u };

  enum Mode 
  {
    POINT,
    LINE,
    FILL
  };

  enum Face 
  {
    FRONT_AND_BACK,
    FRONT,
    BACK
  };

  // Set/query/remove the polygon mode.
  virtual void                  setPolygonMode    ( Face face, Mode mode ) = 0;
  virtual void                  togglePolygonMode ( Face face, Mode mode ) = 0;
  virtual bool                  hasPolygonMode    ( Face face, Mode mode ) const = 0;
  virtual bool                  hasPolygonMode    ( Face face ) const = 0;
  virtual bool                  hasPolygonMode() const = 0;
  virtual void                  removePolygonMode() = 0;

  // Set/query/remove hidden lines.
  virtual void                  setHiddenLines() = 0;
  virtual void                  toggleHiddenLines() = 0;
  virtual bool                  hasHiddenLines() const = 0;
  virtual void                  removeHiddenLines() = 0;

}; // class IPolygonMode


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_POLYGON_MODE_H_
