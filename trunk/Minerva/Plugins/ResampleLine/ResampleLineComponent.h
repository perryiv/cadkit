
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

#ifndef _RESAMPLE_LINE_H_
#define _RESAMPLE_LINE_H_

#include "Minerva/Plugins/ResampleLine/CompileGuard.h"
#include "Minerva/Interfaces/IFitLineTerrain.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"


class ResampleLineComponent : public Usul::Base::Referenced,
                              public Minerva::Interfaces::IFitLineTerrain,
                              public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Minerva::Interfaces::IFitLineTerrain::Vertices   Vertices;
  typedef Usul::Base::Referenced                           BaseClass;
  typedef Usul::Interfaces::IUnknown                       Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ResampleLineComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  ResampleLineComponent();
  
protected:

  virtual void resample ( const Vertices& input, Vertices& output, unsigned int maximumDepth, Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  ResampleLineComponent ( const ResampleLineComponent & );
  ResampleLineComponent &operator = ( const ResampleLineComponent & );

  /// Use reference counting.
  virtual ~ResampleLineComponent();
};

#endif // _RESAMPLE_LINE_H_
