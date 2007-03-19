
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

#include "OsgTools/Animate/Date.h"

#include <string>
#include <map>
#include <vector>

namespace boost { namespace serialization { class access; } }

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

  /// Clone the this layer.
  virtual Layer*              clone() const;

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

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );

  typedef std::map < OsgTools::Animate::Date, DataObjects > TemporalMap;
  typedef std::map < int, DataObjects > NumberMap;

  Mode _mode;
  DrawMode _drawMode;
  std::string _stepColumn;
  float _zScaleFactor;
  float _xOffset;
  float _yOffset;
  TemporalMap _temporalMap;
  NumberMap _numberMap;
};


template < class Archive >
void PolygonTimeLayer::serialize( Archive &ar, const unsigned int version )
{
  ar & boost::serialization::make_nvp( "BaseLayer", boost::serialization::base_object< BaseClass >(*this) );
  ar & boost::serialization::make_nvp( "Mode", _mode );
  ar & boost::serialization::make_nvp( "DrawMode", _drawMode );
  ar & boost::serialization::make_nvp( "StepColumn", _stepColumn );
  ar & boost::serialization::make_nvp( "ZScaleFactor", _zScaleFactor );
  ar & boost::serialization::make_nvp( "XOffset", _xOffset );
  ar & boost::serialization::make_nvp( "YOffset", _yOffset );
}

}
}
}

#endif // __WNV_LAYERS_POLYGON_TIME_LAYER_H__
