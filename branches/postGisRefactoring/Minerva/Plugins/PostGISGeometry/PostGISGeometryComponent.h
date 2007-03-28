
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

#ifndef _COMPUTE_NORMALS_H_
#define _COMPUTE_NORMALS_H_

#include "Minerva/Plugins/ComputeNormals/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IComputeTriangleNormals.h"


class ComputeNormalsComponent : public Usul::Base::Referenced,
                                public Usul::Interfaces::IComputeTriangleNormals,
                                public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IComputeTriangleNormals::Vertices      Vertices;
  typedef Usul::Interfaces::IComputeTriangleNormals::Normals       Normals;
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ComputeNormalsComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  ComputeNormalsComponent();
  
protected:

  /// Generate normals.
  void computeNormalsPerTriangle ( const Vertices& vertices, Normals& normals );

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  ComputeNormalsComponent ( const ComputeNormalsComponent & );
  ComputeNormalsComponent &operator = ( const ComputeNormalsComponent & );

  /// Use reference counting.
  virtual ~ComputeNormalsComponent();
};


#endif // _COMPUTE_NORMALS_H_
