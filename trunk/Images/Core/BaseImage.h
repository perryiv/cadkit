
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all image implementations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_BASE_IMAGE_CLASS_H_
#define _IMAGES_BASE_IMAGE_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>


namespace Images {


class BaseImage : public Usul::Base::Referenced
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::vector < unsigned int > Histogram;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseImage );

  // Constructors.
  BaseImage();
  BaseImage ( const BaseImage & );

  // Get the number of channels.
  unsigned int                channels() const { return _channels; }

  // Return a clone of this image.
  virtual BaseImage *         clone() const = 0;

  // Get the height.
  unsigned int                height() const { return _height; }

  // Calculate the histogram.
  virtual void                histogram ( Histogram &h ) const = 0;

  // Convert to grayscale.
  virtual void                toGrayScale() = 0;

  // Read the file.
  virtual void                read ( const std::string &name ) = 0;

  // Resize the image.
  virtual void                resize ( unsigned int width, unsigned int height, unsigned int channels ) = 0;

  // Set the pixels of this image.
  virtual void                setPixels ( Unknown *pixels ) = 0;

  // Get the width.
  unsigned int                width() const { return _width; }

protected:

  // Use reference counting.
  virtual ~BaseImage();

private:

  // No assignment.
  BaseImage &operator = ( const BaseImage &image );

  unsigned int _width;
  unsigned int _height;
  unsigned int _channels;
  bool _hasAlpha;
};


} // Images


#endif // _IMAGES_BASE_IMAGE_CLASS_H_
