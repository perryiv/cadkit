
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
//  Triangulate component.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGULATE_H_
#define _TRIANGULATE_H_

#include "Minerva/Plugins/TriangulateCGAL/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ITriangulate.h"


class TriangulateComponent : public Usul::Base::Referenced,
                             public Usul::Interfaces::ITriangulate,
                             public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::ITriangulate::Vertices3D    Vertices3D;
  typedef Usul::Base::Referenced                        BaseClass;
  typedef Usul::Interfaces::IUnknown                    Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangulateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  TriangulateComponent();
  
  /// Generate triangles.
  virtual void triangulate ( const Vertices3D &in, Vertices3D &out );

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

protected:

  // Do not copy.
  TriangulateComponent ( const TriangulateComponent & );
  TriangulateComponent &operator = ( const TriangulateComponent & );

  /// Use reference counting.
  virtual ~TriangulateComponent();
};


#endif // _TRIANGULATE_H_
