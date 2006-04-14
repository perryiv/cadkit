
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A class used to wrap the Triangle Code from the Quake project at CMU. This
//  code can be found at http://www.cs.cmu.edu/~quake/triangle.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGULATE_H_
#define _TRIANGULATE_H_

#include "Helios/Plugins/Triangulate/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ITriangulate.h"


class TriangulateComponent : public Usul::Base::Referenced,
                             public Usul::Interfaces::ITriangulate,
                             public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::ITriangulate::Vertices    Vertices;
  typedef Usul::Interfaces::ITriangulate::UIntArray   UIntArray;
  typedef Usul::Interfaces::ITriangulate::InnerLoops  InnerLoops;
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangulateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  TriangulateComponent();
  
  /// Generate triangles.
  virtual void                  triangulate ( const Vertices &in, InnerLoops& inner, Vertices &out, UIntArray& indices );

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
