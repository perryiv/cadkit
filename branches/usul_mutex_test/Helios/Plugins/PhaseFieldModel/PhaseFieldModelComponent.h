
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PHASEFIELDMODELCOMPONENT_H__
#define __PHASEFIELDMODELCOMPONENT_H__

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"


class PhaseFieldModelComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::IPlugin,
                                 public Usul::Interfaces::IDocumentCreate
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PhaseFieldModelComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  PhaseFieldModelComponent();

protected:    
  /// Destructor
  virtual ~PhaseFieldModelComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::IDocumentCreate
  virtual Usul::Documents::Document *   createDocument ( Unknown *caller = 0x0 );
};

#endif /* __PHASEFIELDMODELCOMPONENT_H__  */		
	
