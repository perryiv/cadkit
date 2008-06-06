
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

#ifndef __VIEWTABCOMPONENT_H__
#define __VIEWTABCOMPONENT_H__

#include "FrameTab.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "Usul/Interfaces/Fox/IFoxAddTab.h"

class ViewTabComponent : public Usul::Base::Referenced,
                         public Usul::Interfaces::IPlugin,
                         public Usul::Interfaces::IFoxAddTab
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ViewTabComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  ViewTabComponent();

protected:    
  /// Destructor
  ~ViewTabComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::FoxAddTab
  virtual void foxAddTab( Usul::Interfaces::IUnknown *caller );

private:
  FrameTab                          _frameTab;
};

#endif /* __VIEWTABCOMPONENT_H__  */		
	
