
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

#ifndef _GENERAL_SHADER_COMPONENT_CLASS_H_
#define _GENERAL_SHADER_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>

class SceneDocument;


class GeneralShaderComponent : public Usul::Base::Referenced,
                                   public Usul::Interfaces::IPlugin,
                                   public Usul::Interfaces::IDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GeneralShaderComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  GeneralShaderComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "General Shader"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *            createDocument ( Unknown *caller = 0x0 );

protected: 

  // Do not copy.
  GeneralShaderComponent ( const GeneralShaderComponent & );
  GeneralShaderComponent &operator = ( const GeneralShaderComponent & );

  /// Use reference counting.
  virtual ~GeneralShaderComponent();


};


#endif // _GENERAL_SHADER_COMPONENT_CLASS_H_
