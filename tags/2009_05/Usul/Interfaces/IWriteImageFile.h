
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IWRITE_IMAGE_FILE_H__
#define __USUL_INTERFACES_IWRITE_IMAGE_FILE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace osg { class Image; }


namespace Usul {
namespace Interfaces {


struct IWriteImageFile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWriteImageFile );

  /// Id for this interface.
  enum { IID = 1658460096u };

	/// Write the file.
	virtual void         writeImageFile ( const std::string& filename, osg::Image & ) const = 0;

}; // struct IWriteImageFile


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IWRITE_IMAGE_FILE_H__
