
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
#include "Usul/Interfaces/IGetImageData.h"

#include <string>
#include <vector>
#include <map>
#include <iosfwd>


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
  typedef Usul::Interfaces::IGetImageDataUint8::Values DataUint8;
  typedef Usul::Interfaces::IGetImageDataUint16::Values DataUint16;
  typedef Usul::Interfaces::IGetImageDataUint32::Values DataUint32;
  typedef Usul::Interfaces::IGetImageDataFloat32::Values DataFloat32;
  typedef Usul::Interfaces::IGetImageDataFloat64::Values DataFloat64;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseImage );

  // Constructors.
  BaseImage();
  BaseImage ( const BaseImage & );

  // Set/get the flag that says if last channel should be interpreted as alpha.
  bool                        alpha() const { return _alpha; }
  void                        alpha ( bool a );

  // Return the number of bytes per scalar value.
  virtual unsigned int        bytes() const = 0;

  // Get the number of channels.
  unsigned int                channels() const { return _channels; }

  // Return a clone of this image.
  virtual BaseImage *         clone() const = 0;

  // Get extreme values in all the channels.
  virtual void                extremes ( ValueCount &low, ValueCount &high ) const = 0;

  // Are the scalar values integers?
  virtual bool                integer() const = 0;

  // Get the height.
  unsigned int                height() const { return _height; }

  // Calculate the histogram.
  virtual void                histogram ( Histogram &h ) const = 0;

  // Convert to grayscale.
  virtual void                toGrayScale() = 0;

  // Convert to rgba.
  virtual void                toRedGreenBlue() = 0;

  // Read the file.
  virtual void                read ( std::istream &in ) = 0;

  // Resize the image.
  virtual void                resize ( unsigned int width, unsigned int height, unsigned int channels ) = 0;

  // Set the pixels of this image.
  virtual void                setPixels ( Unknown *pixels ) = 0;

  // Get the image values.
  virtual void                values ( DataUint8   & ) const = 0;
  virtual void                values ( DataUint16  & ) const = 0;
  virtual void                values ( DataUint32  & ) const = 0;
  virtual void                values ( DataFloat32 & ) const = 0;
  virtual void                values ( DataFloat64 & ) const = 0;

  // Get the width.
  unsigned int                width() const { return _width; }

  // Write the dimensions and pixel data to the given stream.
  virtual void                write ( std::ostream &out ) const = 0;

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
