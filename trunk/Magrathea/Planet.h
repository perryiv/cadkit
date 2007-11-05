
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSSIM_OSSIM_PLANET_WRAPPER_H__
#define __OSSIM_OSSIM_PLANET_WRAPPER_H__

#include "Export.h"
#include "LatLongGrid.h"
#include "LayerManipulator.h"
#include "Manipulator.h"

#include "Usul/Interfaces/IUnknown.h"

#include "osg/Referenced"
#include "osgDB/DatabasePager"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"
#include "ossimPlanet/ossimPlanetTextureLayerGroup.h"
#include "ossim/init/ossimInit.h"

namespace Magrathea
{

class MAGRATHEA_EXPORT Planet : public osg::Referenced
{
public:
  typedef osg::Referenced BaseClass;
    
  enum ShaderType
  {
    OPACITY                 = ossimPlanetShaderProgramSetup::OPACITY,
    HORIZONTAL_SWIPE        = ossimPlanetShaderProgramSetup::HORIZONTAL_SWIPE, 
    VERTICAL_SWIPE          = ossimPlanetShaderProgramSetup::VERTICAL_SWIPE, 
    TOP                     = ossimPlanetShaderProgramSetup::TOP, 
    REFERENCE               = ossimPlanetShaderProgramSetup::REFERENCE,
    ABSOLUTE_DIFFERENCE     = ossimPlanetShaderProgramSetup::ABSOLUTE_DIFFERENCE,
    FALSE_COLOR_REPLACEMENT = ossimPlanetShaderProgramSetup::FALSE_COLOR_REPLACEMENT  
  };

  Planet();

  void                                            setDefaults();
  void                                            init();

  void                                            readKWL( const std::string& );

  int                                             addLayer( const std::string& filename );
  int                                             addLayer( ossimPlanetTextureLayer *layer );

  int                                             addLayerOperation ( const std::string& filename );
  int                                             addLayerOperation ( ossimPlanetTextureLayer *layer );
  
  void                                            removeLayer( int index );
  void                                            removeLayer( ossimPlanetTextureLayer *layer );

  void                                            removeLayerOperation ( int index );
  void                                            removeLayerOperation ( ossimPlanetTextureLayer *layer );

  bool                                            hasLayer( ossimPlanetTextureLayer* layer );
  bool                                            hasLayerOperation( ossimPlanetTextureLayer* layer );

  virtual void                                    reset();
  void                                            refreshLandTextures( ossimPlanetExtents* extents, ossimPlanetPagedLandLodRefreshType refreshType);

  osg::Group*                                     root() const;

  osgDB::DatabasePager*                           databasePager() const;

  osgGA::MatrixManipulator*                       manipulator() const;

  bool                                            elevationEnabled() const;
  void                                            elevationEnabled( bool );

  bool                                            hudEnabled() const;
  void                                            hudEnabled( bool );

  bool                                            ephemerisFlag() const;
  void                                            ephemerisFlag( bool );

  float                                           elevationExag() const;
  void                                            elevationExag( const float& );

  int                                             elevationPatchSize() const;
  void                                            elevationPatchSize( const int& );

  unsigned int                                    levelDetail() const;
  void                                            levelDetail( unsigned int );

  std::string                                     elevationCacheDir() const;
  void                                            elevationCacheDir( const std::string& directory );

  void                                            landType( ossimPlanetLandType );

  void                                            showLatLongGrid( bool b );
  bool                                            showLatLongGrid() const;

  /// Get/Set the split metric.
  void                                            splitMetric ( double );
  double                                          splitMetric () const;

  /// Go to given lat, long and height.
  void                                            gotoLocation( double lat, double lon, double height );

  /// Get/Set the viewer.
  void                                            viewer( Usul::Interfaces::IUnknown* viewer );
  Usul::Interfaces::IUnknown*                     viewer();

  /// We should have only two funtions
  /// setShaderType() and setShaderParam()

  void                                            opacity( const float& val );
  float                                           opacity() const;

  float                                           horizontalSwipe() const;
  void                                            horizontalSwipe( const float& val );      

  float                                           verticalSwipe() const;
  void                                            verticalSwipe( const float& val );      

  void                                            top();

  void                                            reference();

  void                                            absoluteDifference();

  void                                            falseColorReplacement();      
 
  // Pre- and post-render notifications.
  void                                            preRender  ( Usul::Interfaces::IUnknown *caller );
  void                                            postRender ( Usul::Interfaces::IUnknown *caller );

  // Initialize the cull and update visitors of the caller.
  void                      initVisitors ( Usul::Interfaces::IUnknown *caller );
protected:

  virtual ~Planet();

protected: 

  osg::ref_ptr< ossimPlanet >                     _planet;
  osg::ref_ptr< osgDB::DatabasePager >            _databasePager;
  osg::ref_ptr < ossimPlanetTextureLayerGroup >   _textureLayerGroup;
  osg::ref_ptr < ossimPlanetTextureLayerGroup >   _textureOperationLayerGroup;
  osg::ref_ptr< LayerManipulator >                _layerManipulator;

  osg::ref_ptr < Manipulator >                    _manipulator;
  osg::ref_ptr < LatLongGrid >                    _latLongGrid;
  Usul::Interfaces::IUnknown::RefPtr              _viewer;

  static bool                                     _ossimInitialized;
  static unsigned int                             _planetCount;
};

}

#endif // __OSSIM_OSSIM_PLANET_WRAPPER_H__
