
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Image class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_IMAGE_CLASS_H_
#define _WX_GUI_IMAGE_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"


struct WxImage
{
  // We return a copy because wxIcon uses reference counting.
  // See: http://www.wxwindows.org/manuals/2.6.2/wx_wximage.html
  static wxImage         load ( const std::string &file, bool reportFailure = true );
};


#endif // _WX_GUI_IMAGE_CLASS_H_
