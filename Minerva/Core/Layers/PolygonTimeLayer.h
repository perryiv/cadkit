
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_POLYGON_TIME_LAYER_H__
#define __WNV_LAYERS_POLYGON_TIME_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"

#include "Minerva/Core/Animate/Date.h"

#include <string>
#include <map>
#include <vector>


namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT PolygonTimeLayer : public Minerva::Core::Layers::Layer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PolygonTimeLayer );

  PolygonTimeLayer();

  enum Mode
  {
    DATE,
    NUMBER
  };

  enum DrawMode
  {
    POLYGON,
    CYLINDER
  };

  /// Build the scene.
  virtual void            buildScene( osg::Group* parent );

  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get/Set the mode.
  void                    mode ( Mode mode );
  Mode                    mode() const;

  /// Get/Set time step column.
  void                    stepColumn( const std::string& column );
  const std::string&      stepColumn() const;

  void                    zScaleFactor( float f );
  float                   zScaleFactor() const;

  void                    xOffset( float f );
  float                   xOffset() const;

  void                    yOffset( float f );
  float                   yOffset() const;

  /// Is this layer temporal.
  virtual bool            isTemporal() const;

  /// Get the default query.
  virtual std::string     defaultQuery() const;

protected:
  virtual ~PolygonTimeLayer();

  PolygonTimeLayer( const PolygonTimeLayer& layer );

  void                    _buildSceneDateMode   ( osg::Group* parent );
  void                    _buildSceneNumberMode ( osg::Group* parent );

  void                    _registerMembers();

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*              clone() const;

private:

  typedef std::map < Minerva::Core::Animate::Date, DataObjects > TemporalMap;
  typedef std::map < int, DataObjects > NumberMap;

  Mode _mode;
  DrawMode _drawMode;
  std::string _stepColumn;
  float _zScaleFactor;
  float _xOffset;
  float _yOffset;
  TemporalMap _temporalMap;
  NumberMap _numberMap;

  SERIALIZE_XML_CLASS_NAME ( PolygonTimeLayer );
};


}
}
}

#endif // __WNV_LAYERS_POLYGON_TIME_LAYER_H__
