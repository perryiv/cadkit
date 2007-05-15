
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_QT_CORE_SPLASH_SCREEN_H__
#define __HELIOS_QT_CORE_SPLASH_SCREEN_H__

#include "QtGui/QWidget"

namespace CadKit {
namespace Helios {
namespace Core {

class SplashScreen : public QWidget
{
	Q_OBJECT

public:
	typedef QWidget BaseClass;
	
	SplashScreen();
	
};

}
}
}

#endif // __HELIOS_QT_CORE_SPLASH_SCREEN_H__
