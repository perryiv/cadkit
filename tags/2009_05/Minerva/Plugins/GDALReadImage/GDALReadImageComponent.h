
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GDAL_READ_IMAGE_COMPONENT_CLASS_H_
#define _GDAL_READ_IMAGE_COMPONENT_CLASS_H_

#include "Minerva/Plugins/GDALReadImage/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IReadImageFile.h"
#include "Usul/Interfaces/IWriteImageFile.h"

#include <string>

class GDALReadImageComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IReadImageFile,
                               public Usul::Interfaces::IWriteImageFile
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GDALReadImageComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  GDALReadImageComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "GDAL Read Image"; }

protected: 

  // Do not copy.
  GDALReadImageComponent ( const GDALReadImageComponent & );
  GDALReadImageComponent &operator = ( const GDALReadImageComponent & );

  /// Use reference counting.
  virtual ~GDALReadImageComponent();

  /// Can we read this file (IReadImageFile)?
  virtual bool             canRead   ( const std::string &file ) const;
  
  /// Read a file and return an image (IReadImageFile).
	virtual ImagePtr         readImageFile ( const std::string& file ) const;
  
  /// Write the file (IWriteImageFile).
  virtual void             writeImageFile ( const std::string& file, osg::Image & ) const;
};


#endif // _GDAL_READ_IMAGE_COMPONENT_CLASS_H_
