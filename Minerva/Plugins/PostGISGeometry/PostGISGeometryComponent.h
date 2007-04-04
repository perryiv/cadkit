
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

#ifndef _POST_GIS_GEOMETRY_H_
#define _POST_GIS_GEOMETRY_H_

#include "Minerva/Plugins/PostGISGeometry/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ICreateGeometry.h"

class PostGISGeometryComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::IPlugin,
                                 public Usul::Interfaces::ICreateGeometry
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PostGISGeometryComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  PostGISGeometryComponent();
  
protected:

  Usul::Interfaces::IUnknown* createFromBinary ( unsigned int srid, unsigned char* buffer, unsigned int length );

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  PostGISGeometryComponent ( const PostGISGeometryComponent & );
  PostGISGeometryComponent &operator = ( const PostGISGeometryComponent & );

  /// Use reference counting.
  virtual ~PostGISGeometryComponent();
};


#endif // _POST_GIS_GEOMETRY_H_
