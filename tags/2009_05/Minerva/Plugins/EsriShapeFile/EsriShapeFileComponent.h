
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

#ifndef _ESRI_SHAPE_FILE_COMPONENT_CLASS_H_
#define _ESRI_SHAPE_FILE_COMPONENT_CLASS_H_

#include "Minerva/Plugins/EsriShapeFile/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>



class EsriShapeFileComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( EsriShapeFileComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  EsriShapeFileComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Esri Shape File Document"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

protected: 

  // Do not copy.
  EsriShapeFileComponent ( const EsriShapeFileComponent & );
  EsriShapeFileComponent &operator = ( const EsriShapeFileComponent & );

  /// Use reference counting.
  virtual ~EsriShapeFileComponent();


};


#endif // _ARC_GEN_READER_WRITER_COMPONENT_CLASS_H__
