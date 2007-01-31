
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

#include "vrjGA/Referenced.h"
#include "vrjGA/Messages.h"


namespace vrjGA {


class VRJ_GA_EXPORT Callback : public Referenced
{
public:

  // Typedefs and smart-pointers.
  typedef Referenced BaseClass;
  VRJGA_DECLARE_POINTER ( Callback );

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


}; // namespace vrjGA


#endif // _VRJGA_CALLBACK_H_
