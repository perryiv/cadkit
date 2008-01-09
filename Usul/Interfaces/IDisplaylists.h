
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDISPLAYLISTS_H__
#define __USUL_INTERFACES_IDISPLAYLISTS_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct IDisplaylists : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDisplaylists );

  /// Id for this interface.
  enum { IID = 3349582213u };

  // Get/Set use display lists
  virtual bool                    displayList () const = 0;
  virtual void                    displayList ( bool ) = 0;


}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_IDISPLAYLISTS_H__
