
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

  // Set/get the number of channels.
  virtual void                channels ( unsigned int c ) = 0;
  virtual unsigned int        channels() const = 0;

  // Return a clone of this image.
  virtual BaseImage *         clone() const = 0;

  // Get/set the number of columns.
  virtual void                columns ( unsigned int c ) = 0;
  virtual unsigned int        columns() const = 0;

  // Get extreme values in all the channels.
  virtual void                extremes ( ValueCount &low, ValueCount &high ) const = 0;

  // Convert to grayscale.
  virtual void                grayScale() = 0;

  // Are the scalar values integers?
  virtual bool                integer() const = 0;

  // Calculate the histogram.
  virtual void                histogram ( Histogram &h ) const = 0;
  virtual void                histogram ( unsigned int channel, OneChannelHistogram &h ) const = 0;

  // Get/set the number of layers. For a "regular" 2D image, this is 1.
  virtual void                layers ( unsigned int num ) = 0;
  virtual unsigned int        layers() const = 0;

  // Convert to rgba.
  virtual void                redGreenBlue() = 0;

  // Read the file.
  virtual void                read ( std::istream &in ) = 0;

  // Resize the image.
  virtual void                resize ( unsigned int rows, unsigned int columns, unsigned int layers, unsigned int channels ) = 0;

  // Get/set the number of rows.
  virtual void                rows ( unsigned int r ) = 0;
  virtual unsigned int        rows() const = 0;

  // Set the pixels of this image.
  virtual void                setPixels ( Unknown *pixels ) = 0;

  // Return the size of the image (total number of values).
  unsigned int                size() const;

  // Get the image values.
  virtual void                values ( DataUint8   & ) const = 0;
  virtual void                values ( DataUint16  & ) const = 0;
  virtual void                values ( DataUint32  & ) const = 0;
  virtual void                values ( DataFloat32 & ) const = 0;
  virtual void                values ( DataFloat64 & ) const = 0;

  // Write the dimensions and pixel data to the given stream.
  virtual void                write ( std::ostream &out ) const = 0;

protected:

  // Use reference counting.
  virtual ~BaseImage();

  void                        _set ( unsigned int rows, unsigned int columns, unsigned int layers );

private:

  // No assignment.
  BaseImage &operator = ( const BaseImage &image );

  unsigned int _rows;
  unsigned int _columns;
  unsigned int _layers;
  bool _alpha;
};


} // Images


#endif // _IMAGES_BASE_IMAGE_CLASS_H_
