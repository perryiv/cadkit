
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


class FlowReaderComponent : public Usul::Base::Referenced,
                                   public Usul::Interfaces::IPlugin,
                                   public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlowReaderComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  FlowReaderComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Flow Reader"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

protected: 

  // Do not copy.
  FlowReaderComponent ( const FlowReaderComponent & );
  FlowReaderComponent &operator = ( const FlowReaderComponent & );

  /// Use reference counting.
  virtual ~FlowReaderComponent();


};


#endif // _FIELD_VIEW_READER_COMPONENT_CLASS_H_
