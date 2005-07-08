
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_IMAGE_FACTORY_CLASS_H_
#define _IMAGES_IMAGE_FACTORY_CLASS_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Images/Core/Export.h"


namespace Images {

class BaseImage;

class Factory
{
public:

  // Useful typedefs
  typedef Usul::Interfaces::IUnknown Unknown;

  // Create the image.
  static BaseImage *create ( unsigned int bytes, bool integer, Unknown *pixels = 0x0 );
};


} // Images


#endif // _IMAGES_IMAGE_FACTORY_CLASS_H_
