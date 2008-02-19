
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FIELD_VIEW_READER_COMPONENT_CLASS_H_
#define _FIELD_VIEW_READER_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>

class SceneDocument;


class FieldViewReaderComponent : public Usul::Base::Referenced,
                                   public Usul::Interfaces::IPlugin,
                                   public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FieldViewReaderComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  FieldViewReaderComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Field View Reader"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

protected: 

  // Do not copy.
  FieldViewReaderComponent ( const FieldViewReaderComponent & );
  FieldViewReaderComponent &operator = ( const FieldViewReaderComponent & );

  /// Use reference counting.
  virtual ~FieldViewReaderComponent();


};


#endif // _FIELD_VIEW_READER_COMPONENT_CLASS_H_
