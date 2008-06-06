
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDLDNAVIGATOR_H__
#define __USUL_INTERFACES_IDLDNAVIGATOR_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct IDldNavigator : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDldNavigator );

  /// Id for this interface.
  enum { IID = 2919146659u };

  // decrement file position counter
  virtual bool              decrementFilePosition () = 0;

  // increment file position counter
  virtual bool              incrementFilePosition () = 0;

  // send the signal to load the file in the current file position
  virtual bool              loadCurrentFile( bool loadFile ) = 0;

  // return the current file position
  virtual unsigned int      currentFilePosition () = 0;

  // start/stop animation
  virtual void              animate( bool a ) = 0;

  // set the current file position
  virtual void              setCurrentFilePosition( unsigned int pos ) = 0;


}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_IDLDNAVIGATOR_H__
