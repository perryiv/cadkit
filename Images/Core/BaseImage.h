
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
#include <map>


namespace Images {


class BaseImage : public Usul::Base::Referenced
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::map < double, unsigned int > OneChannelHistogram;
  typedef std::vector < OneChannelHistogram > Histogram;
  typedef std::pair < double, unsigned int > OneChannelValueCount;
  typedef std::vector < OneChannelValueCount > ValueCount;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseImage );

  // Constructors.
  BaseImage();
  BaseImage ( const BaseImage & );

  // Set/get the flag that says if last channel should be interpreted as alpha.
  bool                        alpha() const { return _alpha; }
  void                        alpha ( bool a ) { _alpha = a; }

  // Get the number of channels.
  unsigned int                channels() const { return _channels; }

  // Return a clone of this image.
  virtual BaseImage *         clone() const = 0;

  // Get extreme values in all the channels.
  virtual void                extremes ( ValueCount &low, ValueCount &high ) const = 0;

  // Get the height.
  unsigned int                height() const { return _height; }

  // Calculate the histogram.
  virtual void                histogram ( Histogram &h ) const = 0;

  // Convert to grayscale.
  virtual void                toGrayScale() = 0;

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
  bool _alpha;
};


} // Images


#endif // _IMAGES_BASE_IMAGE_CLASS_H_
