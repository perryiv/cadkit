
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for tool callbacks.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_TOOL_CALLBACK_FUNCTIONS_H_
#define _CV_TOOL_CALLBACK_FUNCTIONS_H_

#include "CadViewer/ToolFactory.h"
#include "Usul/Functors/Interaction/Navigate/Direction.h"

#include "Usul/Interfaces/IUnknown.h"

#include "MenuKit/Item.h"


namespace CV {


///////////////////////////////////////////////////////////////////////////////
//
//  Struct for tool button callback.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Analog,
  class Tool
>
struct ScaleCB
{
  typedef MenuKit::Item Item;
  typedef MenuKit::Message Message;
  typedef typename Tool::ValidRefPtr ToolPtr;
  typedef typename Tool::Vector Vec;
  typedef CV::ScaleFactory<Analog,Tool> Factory;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Execute the callback.
  //
  /////////////////////////////////////////////////////////////////////////////

  template
  <
    class Caller, 
    class Functor,
    class Transforms
  >
  static void execute ( unsigned int id, 
                        Message m, 
                        Item *item, 
                        Functor &functor, 
                        Transforms &trans,
                        float speed, 
                        const Vec &vec, 
                        Caller *caller )
  {
    // Process the message.
    switch ( m )
    {
      case MenuKit::MESSAGE_UPDATE:
        item->checked ( functor.valid() && id == functor->id() );
        break;

      case MenuKit::MESSAGE_SELECTED:
      {
        // If the functor matches the given id...
        if ( functor.valid() && id == functor->id() )
          functor = 0x0;

        // Otherwise...
        else
        {
          // Get the caller's unknown pointer.
          Usul::Interfaces::IUnknown::ValidQueryPtr unknown ( caller );

          // Make the new tool.
          ToolPtr tool ( Factory::create ( vec, speed, unknown, id ) );

          // Populate the list of transforms.
          tool->transforms ( trans );

          // Set the functor.
          functor = tool;
        }
        break;
      }
    }
  }
};


}; // namespace CV


#endif // _CV_TOOL_CALLBACK_FUNCTIONS_H_
