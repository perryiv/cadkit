
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _EDGE_DETECT_COMPONENT_CLASS_H_
#define _EDGE_DETECT_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/ICommand.h"



class EdgeDetectComponent : public Usul::Base::Referenced,
                            public Usul::Interfaces::IPlugin,
                            public Usul::Interfaces::IMenuEntry,
                            public Usul::Interfaces::ICommand

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( EdgeDetectComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  EdgeDetectComponent();


  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Edge Detect"; }

  ///  Usul::Interfaces::IMenuEntry
  virtual std::string     menuText() const;
  virtual std::string     hotKeyText() const;
  virtual std::string     statusBarText() const;
  virtual std::string     menuGroup() const;

  ///  Usul::Interfaces::ICommand
  virtual void            execute ( Usul::Interfaces::IUnknown *caller );



protected:

  // Do not copy.
  EdgeDetectComponent ( const EdgeDetectComponent & );
  EdgeDetectComponent &operator = ( const EdgeDetectComponent & );

  /// Use reference counting.
  virtual ~EdgeDetectComponent();


};



#endif //  _EDGE_DETECT_COMPONENT_CLASS_H_
