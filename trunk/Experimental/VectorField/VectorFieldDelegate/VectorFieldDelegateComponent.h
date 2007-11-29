
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

#ifndef _SCENE_MODEL_COMPONENT_CLASS_H_
#define _SCENE_MODEL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Helios/Qt/Views/OSG/Delegate.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include <string>


class VectorFieldDelegateComponent : public CadKit::Helios::Views::OSG::Delegate,
                               public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef CadKit::Helios::Views::OSG::Delegate BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VectorFieldDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  VectorFieldDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Scene Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;

protected: 

  // Do not copy.
  VectorFieldDelegateComponent ( const VectorFieldDelegateComponent & );
  VectorFieldDelegateComponent &operator = ( const VectorFieldDelegateComponent & );

  /// Use reference counting.
  virtual ~VectorFieldDelegateComponent();


};


#endif // _SCENE_MODEL_COMPONENT_CLASS_H_
