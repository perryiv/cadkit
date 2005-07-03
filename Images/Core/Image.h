
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

#include <string>
#include <vector>
#include <map>


namespace Images {


class IMAGES_EXPORT Image : public Usul::Base::Referenced
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef std::map < double, unsigned int > OneChannelHistogram;
  typedef std::vector < OneChannelHistogram > Histogram;
  typedef std::pair < double, unsigned int > OneChannelValueCount;
  typedef std::vector < OneChannelValueCount > ValueCount;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Image );

  // Constructors.
  Image ( unsigned int bytes = 1, bool floating = false );
  Image ( const Image & );

  // Get the number of channels.
  unsigned int          channels() const;

  // Get extreme values in all the channels.
  void                  extremes ( ValueCount &low, ValueCount &high ) const;

  // Get the height.
  unsigned int          height() const;

  // Calculate the histogram.
  void                  histogram ( Histogram &h ) const;

  // Assignment.
  Image &               operator = ( const Image &image );

  // Read the file.
  void                  read ( const std::string &name );

  // Get the width.
  unsigned int          width() const;

  // Convert to grayscale.
  void                  toGrayScale();

protected:

  // Use reference counting.
  virtual ~Image();

private:

  BaseImage::ValidRefPtr _image;
};


} // Images


#endif // _IMAGES_IMAGE_CLASS_H_
