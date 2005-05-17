
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the image list
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_IMAGE_LIST_H_
#define _USUL_INTERFACE_IMAGE_LIST_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace osg { template < class T > class ref_ptr; class Image; }

namespace Usul {
namespace Interfaces {


struct IImageList : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IImageList );

  /// Id for this interface.
  enum { IID = 1331711481u };

  typedef osg::ref_ptr< osg::Image > ImagePtr;
  typedef std::vector< ImagePtr > ImageList;

  virtual ImageList& getImageList() = 0;

}; // struct IImageList


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_IMAGE_LIST_H_
