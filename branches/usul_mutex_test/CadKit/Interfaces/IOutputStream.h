
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IOutputStream: Interface to set an output stream.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_OUTPUT_STREAM_H_
#define _CADKIT_INTERFACE_OUTPUT_STREAM_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <iostream>
#endif


namespace CadKit
{
class IOutputStream : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032755120 };

  // Set the output stream.
  virtual void            setOutputStream ( std::ostream *out ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_OUTPUT_STREAM_H_
