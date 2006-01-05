
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component class to smooth triangles
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SMOOTH_TRIANGLES_COMPONENT_H__
#define __SMOOTH_TRIANGLES_COMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ISmoothTriangles.h"

class SmoothTrianglesComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::ISmoothTriangles,
                                 public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SmoothTrianglesComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  SmoothTrianglesComponent();

protected:    
  ///Destructor
  ~SmoothTrianglesComponent();

  ///Usul/Interfaces/IPlugin.h implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::ISmoothTriangles
  virtual void smoothTriangles ( OsgTools::Triangles::TriangleSet* );
};

#endif /* __SMOOTH_TRIANGLES_COMPONENT_H__  */		
	
