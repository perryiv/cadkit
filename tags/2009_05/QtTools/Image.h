
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for creating images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_QT_TOOLS_IMAGE_LOADER_CLASS_H_
#define _CADKIT_QT_TOOLS_IMAGE_LOADER_CLASS_H_

#include "QtTools/Export.h"

#include <string>

class QWidget;
class QAction;
class QLabel;
class QSplashScreen;
class QAbstractButton;


namespace QtTools {


struct QT_TOOLS_EXPORT Image
{
  static void icon   ( const std::string &file, QAbstractButton *button );
  static void icon   ( const std::string &file, QWidget *widget );
  static void icon   ( const std::string &file, QAction *action );
  static void pixmap ( const std::string &file, QSplashScreen *splash );
  static void pixmap ( const std::string &file, QLabel *label );
};


} // namespace QtTools


#endif //_CADKIT_QT_TOOLS_IMAGE_LOADER_CLASS_H_
