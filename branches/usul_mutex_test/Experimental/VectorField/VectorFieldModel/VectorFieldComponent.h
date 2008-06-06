
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

#ifndef _VECTOR_FIELD_COMPONENT_CLASS_H_
#define _VECTOR_FIELD_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>

class VectorFieldDocument;


class VectorFieldComponent : public Usul::Base::Referenced,
                       public Usul::Interfaces::IPlugin,
                       public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VectorFieldComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  VectorFieldComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "VectorField Model"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *           createDocument ( Unknown *caller = 0x0 );

protected:

  // Do not copy.
  VectorFieldComponent ( const VectorFieldComponent & );
  VectorFieldComponent &operator = ( const VectorFieldComponent & );

  /// Use reference counting.
  virtual ~VectorFieldComponent();

};


#endif // _VECTOR_FIELD_COMPONENT_CLASS_H_
