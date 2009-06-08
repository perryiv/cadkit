
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

#ifndef _PROJECTION_MANAGER_COMPONENT_H_
#define _PROJECTION_MANAGER_COMPONENT_H_

#include "Minerva/Plugins/ProjectionManager/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IProjectCoordinates.h"


class ProjectionManagerComponent : public Usul::Base::Referenced,
                                   public Usul::Interfaces::IPlugin,
                                   public Usul::Interfaces::IProjectCoordinates
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ProjectionManagerComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  ProjectionManagerComponent();
  
protected:

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  /// Get a list of projection names.
  virtual Projections           projectionList() const;

  /// Project to lat/lon ( no elevation ) using given spatial reference id.
  virtual void                  projectToSpherical ( const Usul::Math::Vec3d& orginal, unsigned int srid, Usul::Math::Vec3d& latLonPoint ) const;

  // Do not copy.
  ProjectionManagerComponent ( const ProjectionManagerComponent & );
  ProjectionManagerComponent &operator = ( const ProjectionManagerComponent & );

  /// Use reference counting.
  virtual ~ProjectionManagerComponent();
};


#endif // _PROJECTION_MANAGER_COMPONENT_H_
