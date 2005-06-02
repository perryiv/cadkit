
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the image
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_IMAGE_H_
#define _USUL_INTERFACE_IMAGE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Image; }

namespace Usul {
namespace Interfaces {


struct IImage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IImage );

  /// Id for this interface.
  enum { IID = 1708090928u };


  virtual osg::Image* getImage() = 0;

}; // struct IImage


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_IMAGE_H_
