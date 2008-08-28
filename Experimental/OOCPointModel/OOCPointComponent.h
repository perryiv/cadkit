
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

#ifndef _POINT_MODEL_COMPONENT_CLASS_H_
#define _POINT_MODEL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>

class OOCPointComponent : public Usul::Base::Referenced,
                       public Usul::Interfaces::IPlugin,
                       public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( Point );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( OOCPointComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  OOCPointComponent();

protected: 

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Point Model"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

  // Do not copy.
  OOCPointComponent ( const OOCPointComponent & );
  OOCPointComponent &operator = ( const OOCPointComponent & );

  /// Use reference counting.
  virtual ~OOCPointComponent();

};


#endif // _POINT_MODEL_COMPONENT_CLASS_H_
