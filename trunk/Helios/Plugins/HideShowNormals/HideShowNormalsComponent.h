
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

#ifndef __HIDESHOWNORMALSCOMPONENT_H__
#define __HIDESHOWNORMALSCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IMenuAdd.h"

class HideShowNormalsComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::IMenuAdd,
                                 public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HideShowNormalsComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  HideShowNormalsComponent();

  void                       showNormals ( bool b);
  bool                       showNormals () const;
  bool                       canHideShowNormals () const;

protected:    
  /// Destructor
  ~HideShowNormalsComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string        getPluginName() const;
  
  virtual void               menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

};

#endif /* __HIDESHOWNORMALSCOMPONENT_H__  */		
	
