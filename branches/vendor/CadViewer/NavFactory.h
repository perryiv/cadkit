
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for making navigators.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_NAV_FACTORY_CLASS_H_
#define _CV_NAV_FACTORY_CLASS_H_


namespace CV {


template
<
  class Analog,
  class Dir,
  class Mat,
  class Nav
>
struct NavFactory
{
  // Typedefs.
  typedef typename Analog::ValidRefPtr AnalogPtr;
  typedef typename Dir::ValidRefPtr DirPtr;;
  typedef typename Dir::Vector Vec;
  typedef typename Nav::ValidRefPtr NavPtr;
  typedef typename Mat::ValidRefPtr MatPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Create the navigator.
  static NavPtr create ( const Vec &axis, float speed, Unknown *unknown, unsigned int id )
  {
    // The analog input functor.
    AnalogPtr analog ( new Analog ( unknown ) );

    // The matrix functor.
    MatPtr mat ( new Mat ( unknown ) );

    // For getting directions in the proper coordinate system.
    DirPtr dir ( new Dir ( unknown, axis, mat ) );

    // Make the navigation functor.
    NavPtr nav ( new Nav ( unknown, dir, analog, speed, id ) );

    // Return the navigation functor.
    return nav;
  }
};


}; // namespace CV;


#endif // _CV_NAV_FACTORY_CLASS_H_
