
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

#ifndef _TRIANGLE_MODEL_COMPONENT_CLASS_H_
#define _TRIANGLE_MODEL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/ICreateTriangleDocument.h"

#include <string>


class TriangleComponent : public Usul::Base::Referenced,
                          public Usul::Interfaces::IPlugin,
                          public Usul::Interfaces::IDocumentCreate,
                          public Usul::Interfaces::ICreateTriangleDocument
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( Triangle );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  TriangleComponent();

protected: 

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Triangle Model"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

  /// Usul::Interfaces::ICreateTriangleDocument
  virtual void                  createTriangleDocument ( OsgTools::Triangles::TriangleSet*, IUnknown *caller = 0x0 );

  // Do not copy.
  TriangleComponent ( const TriangleComponent & );
  TriangleComponent &operator = ( const TriangleComponent & );

  /// Use reference counting.
  virtual ~TriangleComponent();
};


#endif // _TRIANGLE_MODEL_COMPONENT_CLASS_H_
