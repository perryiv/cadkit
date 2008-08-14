
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IREAD_IMAGE_FILE_H__
#define __USUL_INTERFACES_IREAD_IMAGE_FILE_H__

#include "Usul/Interfaces/IUnknown.h"

#include "osg/Image"
#include "osg/ref_ptr"

#include <string>

namespace Usul {
namespace Interfaces {


struct IReadImageFile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReadImageFile );

  /// Id for this interface.
  enum { IID = 3269784933u };

  /// Typedefs.
	typedef osg::ref_ptr<osg::Image> ImagePtr;

  /// Can we read this file?
  virtual bool         canRead ( const std::string &filename ) const = 0;
  
	/// Read the file.
	virtual ImagePtr     readImageFile ( const std::string& filename ) const = 0;

}; // struct IReadImageFile


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IREAD_IMAGE_FILE_H__
