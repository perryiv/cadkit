
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
//  Triangulate Grid component.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_PLUGINS_TRIANGULATE_GRID_H_
#define _HELIOS_PLUGINS_TRIANGULATE_GRID_H_

#include "Helios/Plugins/TriangulateGrid/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ITriangulateGrid.h"


class TriangulateGridComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::ITriangulateGrid,
                                 public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                        BaseClass;
  typedef Usul::Interfaces::IUnknown                    Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangulateGridComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  TriangulateGridComponent();
  
  // Build a triangle set from a grid of data.  GridResolution is the size of a grid cell in the x and y directions.
  virtual TriangleSet*  triangulateGrid ( const Grid &grid, 
                                          const GridResolution& resolution, 
                                          const Usul::Math::Vec3d& offset, 
                                          ValueType noDataValue,
                                          Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

protected:

  // Do not copy.
  TriangulateGridComponent ( const TriangulateGridComponent & );
  TriangulateGridComponent &operator = ( const TriangulateGridComponent & );

  /// Use reference counting.
  virtual ~TriangulateGridComponent();
};


#endif // _HELIOS_PLUGINS_TRIANGULATE_GRID_H_
