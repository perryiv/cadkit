
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that abstracts the system clipboard.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_SYSTEM_CLIPBOARD_H__
#define __USUL_SYSTEM_CLIPBOARD_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace System {


struct USUL_EXPORT ClipBoard
{
  /// Does the current platform have a clip board?
  static bool isClipBoardSupported();
  
  /// Add the given text to the clip board.
  static void addToClipboard ( const std::string& text );
  
};


}
}


#endif
