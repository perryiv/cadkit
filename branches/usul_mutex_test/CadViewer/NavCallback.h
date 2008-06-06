
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for navigation callbacks.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_NAVIGATION_CALLBACK_FUNCTIONS_H_
#define _CV_NAVIGATION_CALLBACK_FUNCTIONS_H_

#include "CadViewer/NavFactory.h"
#include "VRV/Functors/Navigate/Direction.h"

#include "Usul/Interfaces/IUnknown.h"

#include "MenuKit/Item.h"


namespace CV {


///////////////////////////////////////////////////////////////////////////////
//
//  Struct for navigation button callback.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Analog,
  class Mat,
  class Nav
>
struct NavCB
{
  typedef MenuKit::Item Item;
  typedef MenuKit::Message Message;
  typedef VRV::Functors::Direction Dir;
  typedef Dir::Vector Vec;
  typedef CV::NavFactory<Analog,Dir,Mat,Nav> Factory;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Execute the callback.
  //
  /////////////////////////////////////////////////////////////////////////////

  template
  <
    class Caller, 
    class Functor
  >
  static void execute ( unsigned int id, 
                        Message m, 
                        Item *item, 
                        Functor &nav, 
                        float speed, 
                        const Vec &dir, 
                        Caller *caller )
  {
    // Process the message.
    switch ( m )
    {
      case MenuKit::MESSAGE_UPDATE:
        item->checked ( nav.valid() && id == nav->id() );
        break;

      case MenuKit::MESSAGE_SELECTED:
      {
        // If the navigator matches the given id...
        if ( nav.valid() && id == nav->id() )
          nav = 0x0;

        // Otherwise, set the navigator.
        else
        {
          Usul::Interfaces::IUnknown::ValidQueryPtr unknown ( caller );
          nav = Factory::create ( dir, speed, unknown, id );
        }
        break;
      }
    }
  }
};


}; // namespace CV


#endif // _CV_NAVIGATION_CALLBACK_FUNCTIONS_H_
