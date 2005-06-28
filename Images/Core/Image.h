
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


namespace Images {


class IMAGES_EXPORT Image : public Usul::Base::Referenced
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector < unsigned int > Histogram;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Image );

  // Constructors.
  Image ( unsigned int bytes = 1, bool floating = false );
  Image ( const Image & );

  // Assignment.
  Image &               operator = ( const Image &image );

  // Get the number of channels.
  unsigned int          channels() const;

  // Get the height.
  unsigned int          height() const;

  // Calculate the histogram.
  void                  histogram ( Histogram &h ) const;

  // Read the file.
  void                  read ( const std::string &name );

  // Get the width.
  unsigned int          width() const;

  // Convert to grayscale.
  void                  toGrayScale();

protected:

  // Use reference counting.
  virtual ~Image();

  bool                  _readTiff ( const std::string &name );

private:

  BaseImage::ValidRefPtr _image;
};


} // Images


#endif // _IMAGES_IMAGE_CLASS_H_
