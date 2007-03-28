
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _POSTGRES_CONNECTION_COMPONENT_H_
#define _POSTGRES_CONNECTION_COMPONENT_H_

#include "Minerva/Plugins/PostgresConnection/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"


class PostgresConnectionComponent : public Usul::Base::Referenced,
                                    public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PostgresConnectionComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  PostgresConnectionComponent();
  
protected:

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  PostgresConnectionComponent ( const PostgresConnectionComponent & );
  PostgresConnectionComponent &operator = ( const PostgresConnectionComponent & );

  /// Use reference counting.
  virtual ~PostgresConnectionComponent();
};


#endif // _POSTGRES_CONNECTION_COMPONENT_H_
