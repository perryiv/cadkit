
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  I'm not sure if this class is needed, or if this is the correct location for it.
//  I'm currently thinking that this class will be used for branding.
//  TODO: Need a mutex.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_APP_APPLICATION_H__
#define __USUL_APP_APPLICATION_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace App {

class USUL_EXPORT Application
{
public:
	static Application& instance();
	
	/// Get the icon directory.
	const std::string                    iconDirectory() const;
	
	/// Get/Set the splash screen image.
	void                                 splashImage( const std::string& image );
	const std::string &                  splashImage( ) const;
	
	/// Get the path to the splash screen image.
	std::string                          splashImagePath() const;
	
private:
	Application();
	~Application();
	
	static Application *_instance;
	
	std::string         _splashImage;
};

}
}

#endif // __USUL_APP_APPLICATION_H__
