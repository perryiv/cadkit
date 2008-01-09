
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PLANET_COMPONENT_CLASS_H_
#define _PLANET_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IPlanetNode.h"

#include "ossimPlanet/ossimPlanet.h"

#include <string>
#include <map>

class PlanetComponent : public Usul::Base::Referenced,
                        public Usul::Interfaces::IPlugin,
                        public Usul::Interfaces::IPlanetNode
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced               BaseClass;
  typedef Usul::Interfaces::IUnknown           Unknown;
  typedef osg::ref_ptr < ossimPlanet >         PlanetPtr;
  typedef std::map < std::string, PlanetPtr >  Planets;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PlanetComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  PlanetComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Planet"; }

protected: 
  /// Return a node for a planet.
  virtual osg::Group* planetNode ( const std::string& key );

  // Do not copy.
  PlanetComponent ( const PlanetComponent & );
  PlanetComponent &operator = ( const PlanetComponent & );

  /// Use reference counting.
  virtual ~PlanetComponent();

private:
  Planets _planets;
};


#endif // _PLANET_COMPONENT_CLASS_H_
