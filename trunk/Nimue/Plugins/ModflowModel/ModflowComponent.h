
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_COMPONENT_CLASS_H_
#define _MODFLOW_MODEL_COMPONENT_CLASS_H_

#include "ModflowModel/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"

#include <string>

namespace Modflow { class ModflowDocument; }


class ModflowComponent : public Usul::Base::Referenced,
                         public Usul::Interfaces::IPlugin,
                         public Usul::Interfaces::IDocumentCreate
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  // Type information.
  USUL_DECLARE_TYPE_ID ( ModflowComponent );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModflowComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  ModflowComponent();

  // Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Modflow Model"; }

  // Usul::Interfaces::IDocumentCreate
  virtual Document *           createDocument ( Unknown *caller = 0x0 );

protected: 

  // Do not copy.
  ModflowComponent ( const ModflowComponent & );
  ModflowComponent &operator = ( const ModflowComponent & );

  // Use reference counting.
  virtual ~ModflowComponent();


};


#endif // _MODFLOW_MODEL_COMPONENT_CLASS_H_
