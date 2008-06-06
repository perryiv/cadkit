
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CENTER_GEOMETRY_COMPONENT_CLASS_H_
#define _CENTER_GEOMETRY_COMPONENT_CLASS_H_

#include "Helios/Plugins/CenterGeometry/CompileGuard.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include <string>

namespace osg { class Transform; }


class CenterGeometryComponent : public Usul::Base::Referenced,
                                public Usul::Interfaces::IPlugin,
											          public Usul::Interfaces::IMenuAdd
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CenterGeometryComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  CenterGeometryComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Center Geometry"; }

	/// Add to the menu.
	virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

	/// Center the geometry.
	void                        centerGeometry();

	/// Enable the button?
	bool                        enableCenterGeometry() const;

  /// Flatten the geometry.
  void                        flattenGeometry();

	/// Enable the button?
	bool                        enableFlattenGeometry() const;

protected:

  // Do not copy.
  CenterGeometryComponent ( const CenterGeometryComponent & );
  CenterGeometryComponent &operator = ( const CenterGeometryComponent & );

  /// Use reference counting.
  virtual ~CenterGeometryComponent();

  // Make the matrix-transform static.
  static void                 _toStaticTransform ( osg::Transform * );

};


#endif // _CENTER_GEOMETRY_COMPONENT_CLASS_H_
