
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

#ifndef __PARADISMODELCOMPONENT_H__
#define __PARADISMODELCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"


class ParadisModelComponent : public Usul::Base::Referenced,
                              public Usul::Interfaces::IDocumentCreate,
                              public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ParadisModelComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  ParadisModelComponent();

protected:    
  /// Destructor
  ~ParadisModelComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  /// Create the document.
  virtual Usul::Documents::Document *    createDocument ( Usul::Interfaces::IUnknown *caller = 0x0 );
};

#endif /* __PARADISMODELCOMPONENT_H__  */		
	
