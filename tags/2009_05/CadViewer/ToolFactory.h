
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for making tool functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_TOOL_FACTORY_CLASS_H_
#define _CV_TOOL_FACTORY_CLASS_H_


namespace CV {


template
<
  class Analog,
  class Tool
>
struct ScaleFactory
{
  // Typedefs.
  typedef typename Analog::ValidRefPtr AnalogPtr;
  typedef typename Tool::Vector Vec;
  typedef typename Tool::ValidRefPtr ToolPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Create the tool.
  static ToolPtr create ( const Vec &scale, float speed, Unknown *unknown, unsigned int id )
  {
    // The analog input functor.
    AnalogPtr analog ( new Analog ( unknown ) );

    // Make the tool functor.
    ToolPtr tool ( new Tool ( unknown, 0x0, analog, scale, speed, id ) );

    // Return the tool functor.
    return tool;
  }
};


}; // namespace CV;


#endif // _CV_TOOL_FACTORY_CLASS_H_
