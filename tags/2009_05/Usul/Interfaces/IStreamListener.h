
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for listeners to streams.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_STREAM_LISTENER_H_
#define _USUL_INTERFACE_STREAM_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


template
<
  class ValueType, 
  unsigned long iid
>
struct IStreamListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IStreamListener );

  // Id for this interface.
  enum { IID = iid };

  // Notify the component.
  virtual void          notify ( Usul::Interfaces::IUnknown *caller, const ValueType *values, unsigned int numValues ) = 0;
};


typedef IStreamListener < char, 1260209607u > IStreamListenerChar;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_STREAM_LISTENER_H_
