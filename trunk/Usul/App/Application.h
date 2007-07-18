
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
	
  /// Get/Set the vender.
  void                                 vendor ( const std::string& );
  const std::string&                   vendor () const;

  /// Get/Set the program name.
  void                                 program ( const std::string& );
  const std::string&                   program () const;

  /// Build a full path for a config file.  Will have the extention .xml
  std::string                          configFile ( const std::string& name );

private:
	Application();
	~Application();
	
	static Application *_instance;
	
	std::string         _splashImage;
  std::string         _vendor;
  std::string         _program;
};

}
}

#endif // __USUL_APP_APPLICATION_H__
