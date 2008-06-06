
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callbacks class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_CALLBACK_H_
#define _VRJGA_CALLBACK_H_

#include "VRV/Export.h"
#include "VRV/Devices/Messages.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace VRV {
namespace Devices {

class VRV_EXPORT Callback : public Usul::Base::Referenced
{
public:

  // Typedefs and smart-pointers.
  typedef Usul::Base::Referenced Referenced;
  typedef Usul::Base::Referenced BaseClass;
  USUL_DECLARE_REF_POINTERS ( Callback );
  typedef RefPtr Ptr;

  // Execute the callback functionality.
  virtual void operator () ( Message m, Referenced *caller ) = 0;

protected:

  // Construction.
  Callback();
  Callback ( const Callback & );
  Callback& operator = ( const Callback & );

  // Use reference counting.
  virtual ~Callback();
};


} // namespace Devices
} // namespace VRV


#endif // _VRJGA_CALLBACK_H_
