
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICOLORSPERVERTEX_H__
#define __USUL_INTERFACES_ICOLORSPERVERTEX_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct IColorsPerVertex : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IColorsPerVertex );

  /// Id for this interface.
  enum { IID = 1183216728u };

  // Get the color values per vertex
  virtual osg::Vec4Array *        getColorsV ( bool reserve = false ) = 0;
  
  // Set the dirty flag.
  virtual void                    dirtyColorsV ( bool ) = 0;

}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_ICOLORSPERVERTEX_H__
