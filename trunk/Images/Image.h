
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

#include "Images/Export.h"
#include "Images/Enum.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"

#include <string>
#include <vector>


namespace Images {

namespace ITK { class BaseImage; }

class IMAGES_EXPORT Image : public Usul::Base::Referenced
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector < unsigned int > Histogram;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Image );

  // Constructors.
  Image ( unsigned int channels = 4, const Images::ValueType &type = UINT_8 );
  Image ( const Image & );

  // Assignment.
  Image &               operator = ( const Image &image );

  // Get the number of channels.
  unsigned int          channels() const;

  // Get the number of columns.
  unsigned int          columns() const;

  // Get the value.
  void                  get ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint8   &value ) const;
  void                  get ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint16  &value ) const;
  void                  get ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint32  &value ) const;
  void                  get ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint64  &value ) const;
  void                  get ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Float32 &value ) const;
  void                  get ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Float64 &value ) const;

  // Calculate the histogram.
  void                  histogram ( Histogram &h, unsigned int numBins = 255 ) const;

  // Read the file.
  void                  read ( const std::string &name );

  // Get the number of rows.
  unsigned int          rows() const;

  // Set the value.
  void                  set ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint8   value );
  void                  set ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint16  value );
  void                  set ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint32  value );
  void                  set ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Uint64  value );
  void                  set ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Float32 value );
  void                  set ( unsigned int row, unsigned int column, unsigned int channel, Usul::Types::Float64 value );

  // Convert to grayscale.
  void                  toGrayScale();

  // Get/set the type.
  Images::ValueType     type() const;
  void                  type ( Images::ValueType type );

protected:

  // Use reference counting.
  virtual ~Image();

private:

  Images::ITK::BaseImage *_image;
};


} // Images


#endif // _IMAGES_IMAGE_CLASS_H_
