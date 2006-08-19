
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

#ifndef __ABOUTCOMPONENT_H__
#define __ABOUTCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IAboutString.h"


class AboutComponent : public Usul::Base::Referenced,
                       public Usul::Interfaces::IPlugin,
                       public Usul::Interfaces::IAboutString
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AboutComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  AboutComponent();

protected:    
  /// Destructor
  ~AboutComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::IAboutString
  virtual std::string       aboutString() const;
};

#endif /* __ABOUTCOMPONENT_H__  */		
	
