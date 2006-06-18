
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

#ifndef __PHASEFIELDDELEGATECOMPONENT_H__
#define __PHASEFIELDDELEGATECOMPONENT_H__

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IHandleActivatingDocument.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include "FoxTools/Headers/Object.h"

namespace FX { class FXVerticalFrame; }

class PhaseFieldDelegateComponent : public Usul::Base::Referenced,
                                    public Usul::Interfaces::IPlugin,
                                    public Usul::Interfaces::IGUIDelegate,
                                    public Usul::Interfaces::IHandleActivatingDocument,
                                    public FX::FXObject
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PhaseFieldDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  PhaseFieldDelegateComponent();

  enum
  {
    ID_FORWARD = 1,
    ID_BACKWARD
  };

  /// Command message maps.
  long                      onCommandForward           ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandBackward          ( FX::FXObject *, FX::FXSelector, void * );

protected:    
  /// Destructor
  virtual ~PhaseFieldDelegateComponent();

  void _buildTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document );

  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  virtual void                refreshView      ( Usul::Documents::Document *document, Usul::Interfaces::IViewer  *viewer );

  /// Usul::Interfaces::IHandleActivatingDocument
  virtual void                noLongerActive ( Usul::Documents::Document* document, const std::string& activeType );
  virtual void                nowActive      ( Usul::Documents::Document* document, const std::string& oldType );

private:

  FX::FXVerticalFrame *_frame;

  FXDECLARE( PhaseFieldDelegateComponent )
};

#endif /* __PHASEFIELDDELEGATECOMPONENT_H__  */		
	
