
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Post-frame notify interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_NOTIFY_POST_FRAME_H_
#define _JSAL_INTERFACE_NOTIFY_POST_FRAME_H_

#include "Usul/Interfaces/IUnknown.h"


namespace JSAL {
namespace Interfaces {


struct IPostFrame : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPostFrame );

  /// Id for this interface.
  enum { IID = 4159531970u };

  // Called after the frames are drawn.
  virtual void              postFrame() = 0;
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_NOTIFY_POST_FRAME_H_
