
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that handles image information.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOXEL_MODEL_IMAGE_INFO_H_
#define _VOXEL_MODEL_IMAGE_INFO_H_

#include "Images/Core/Image.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Headers/OpenGL.h"

#include <string>


class ImageInfo : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ImageInfo );

  // Construction.
  ImageInfo ( const std::string &name );

  // Set/get the dirty flag.
  void            dirty ( bool d ) { _dirty = d; }
  bool            dirty() const { return _dirty; }

  // Get the size.
  unsigned int    rows()    const { return _size[0]; }
  unsigned int    columns() const { return _size[1]; }
  unsigned int    layers()  const { return _size[2]; }

  // Get the name of the image.
  std::string     name() const { return _name; }

  // Set the image.
  void            set ( Images::Image &image ) const;
  void            set ( unsigned int size, unsigned char *image ) const;

protected:

  /// Do not copy.
  ImageInfo ( const ImageInfo & );
  ImageInfo &operator = ( const ImageInfo & );

  /// Use reference counting.
  virtual ~ImageInfo();

private:

  std::string _name;
  Usul::Math::Vec3ui _size;
  std::string _temp;
  bool _dirty;
};


#endif // _VOXEL_MODEL_IMAGE_INFO_H_
