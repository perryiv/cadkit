
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
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
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>



class GDALReadImageComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GDALReadImageComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  GDALReadImageComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "GDAL Read Image"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

protected: 

  // Do not copy.
  GDALReadImageComponent ( const GDALReadImageComponent & );
  GDALReadImageComponent &operator = ( const GDALReadImageComponent & );

  /// Use reference counting.
  virtual ~GDALReadImageComponent();


};


#endif // _GDAL_READ_IMAGE_COMPONENT_CLASS_H_
