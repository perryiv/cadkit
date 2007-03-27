
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Image class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_IMAGE_CLASS_H_
#define _IMAGES_IMAGE_CLASS_H_

#include "Images/Core/Export.h"
#include "Images/Core/BaseImage.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"
#include "Usul/Interfaces/IGetImageProperties.h"
#include "Usul/Interfaces/IGetImageData.h"

#include <string>
#include <vector>
#include <map>
#include <iosfwd>


namespace Images {


class IMAGES_EXPORT Image : public Usul::Base::Referenced,
                            public Usul::Interfaces::IGetImageProperties,
                            public Usul::Interfaces::IGetImageDataUint8,
                            public Usul::Interfaces::IGetImageDataUint16,
                            public Usul::Interfaces::IGetImageDataUint32,
                            public Usul::Interfaces::IGetImageDataFloat32,
                            public Usul::Interfaces::IGetImageDataFloat64
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef std::map < double, unsigned int > OneChannelHistogram;
  typedef std::vector < OneChannelHistogram > Histogram;
  typedef std::pair < double, unsigned int > OneChannelValueCount;
  typedef std::vector < OneChannelValueCount > ValueCount;
  typedef std::pair < unsigned int, bool > TypeInfo; // bytes, integer
  typedef Usul::Interfaces::IGetImageDataUint8::Values DataUint8;
  typedef Usul::Interfaces::IGetImageDataUint16::Values DataUint16;
  typedef Usul::Interfaces::IGetImageDataUint32::Values DataUint32;
  typedef Usul::Interfaces::IGetImageDataFloat32::Values DataFloat32;
  typedef Usul::Interfaces::IGetImageDataFloat64::Values DataFloat64;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Image );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructors.
  Image ( unsigned int bytes = 1, bool integer = true );
  Image ( const Image & );

  // Set/get if this image has an alpha channel.
  void                        alpha ( bool );
  bool                        alpha() const;

  // Get the number of bytes per channel-value.
  unsigned int                bytes() const;

  // Set/get the number of channels.
  void                        channels ( unsigned int c );
  unsigned int                channels() const;

  // Set/get the number of columns.
  void                        columns ( unsigned int c );
  unsigned int                columns() const;

  // Get extreme values in all the channels.
  void                        extremes ( ValueCount &low, ValueCount &high ) const;

  // Get whether or not the pixel values are floating point.
  bool                        floating() const;

  // Get the image dimensions.
  virtual void                getImageDimensions ( unsigned int &rows, unsigned int &columns, unsigned int &layers, unsigned int &channels ) const;

  // Get the image values.
  virtual void                getImageValues ( DataUint8   & ) const;
  virtual void                getImageValues ( DataUint16  & ) const;
  virtual void                getImageValues ( DataUint32  & ) const;
  virtual void                getImageValues ( DataFloat32 & ) const;
  virtual void                getImageValues ( DataFloat64 & ) const;

  // Usul::Interfaces::IGetImageProperties
  // Get the number of bytes per value. IGetImageProperties
  virtual unsigned int        getNumBytesPerValue() const;

  // Convert to grayscale.
  void                        grayScale();

  // Calculate the histogram.
  void                        histogram ( Histogram &h ) const;

  // Set/get whether or not the pixel values are integers.
  void                        integer ( bool state );
  bool                        integer() const;

  // Usul::Interfaces::IGetImageProperties
  // See if the scalar values are integers.
  virtual bool                isValueInteger() const;

  // Set/get the number of layers. For a "regular" 2D image, this is 1.
  void                        layers ( unsigned int d );
  unsigned int                layers() const;

  // Assignment.
  Image &                     operator = ( const Image &image );

  // Read the file.
  void                        read ( const std::string &name );
  void                        read ( std::istream &in );

  // Convert to rgb.
  void                        redGreenBlue();

  // Set/get the number of rows.
  void                        rows ( unsigned int r );
  unsigned int                rows() const;

  // Set/get the type of image.
  void                        type ( unsigned int bytes, bool integer );
  void                        type ( const TypeInfo & );
  TypeInfo                    type() const;

  // Write the dimensions and pixel data to the given stream.
  void                        write ( std::ostream &out ) const;

protected:

  // Use reference counting.
  virtual ~Image();

private:

  BaseImage::ValidRefPtr _image;
};


} // Images


#endif // _IMAGES_IMAGE_CLASS_H_
