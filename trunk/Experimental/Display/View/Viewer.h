
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_VIEWER_CLASS_H_
#define _DISPLAY_LIBRARY_VIEWER_CLASS_H_

#include "Display/View/Canvas.h"


namespace Display {
namespace View {


class DISPLAY_LIBRARY_EXPORT Viewer : public Canvas
{
public:

  // Typedefs.
  typedef Canvas BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Viewer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Construction.
  Viewer();

protected:

  // Use reference counting.
  virtual ~Viewer();

private:

  // No copying or assignment.
  Viewer ( const Viewer & );
  Viewer &operator = ( const Viewer & );

  void                      _destroy();
};


}; // namespace View
}; // namespace Display


#endif // _DISPLAY_LIBRARY_VIEWER_CLASS_H_
