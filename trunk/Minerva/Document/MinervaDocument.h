
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DOCUMENT_H__
#define __MINERVA_DOCUMENT_H__

#include "Export.h"

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ISceneUpdate.h"
#include "Usul/Interfaces/IDatabasePager.h"
#include "Usul/Interfaces/IMatrixManipulator.h"

#include "Minerva/Core/Scene/SceneManager.h"

#include "Magrathea/Planet.h"

class ossimPlanetTextureLayer;

namespace Minerva {
namespace Document {

class MINERVA_DOCUMENT_EXPORT MinervaDocument : public Usul::Documents::Document,
                                                public Usul::Interfaces::IBuildScene,
                                                public Usul::Interfaces::ISceneUpdate,
                                                public Usul::Interfaces::IDatabasePager,
                                                public Usul::Interfaces::IMatrixManipulator
{
public:
  typedef Usul::Documents::Document BaseClass;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( MinervaDocument );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MinervaDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  MinervaDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Get the filters that correspond to what this document can do.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Open the file. Clears any data this document already has.
  virtual void                open ( const std::string &filename, Unknown *caller = 0x0 );

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  void                        removeLayer ( Minerva::Core::Layers::Layer * layer );
  void                        hideLayer   ( Minerva::Core::Layers::Layer * layer );
  void                        showLayer   ( Minerva::Core::Layers::Layer * layer );
  void                        addLayer    ( Minerva::Core::Layers::Layer * layer, Unknown *caller = 0x0 );
  void                        modifyLayer ( Minerva::Core::Layers::Layer * layer, Unknown *caller = 0x0 );

  void                        viewLayerExtents ( ossimPlanetTextureLayer * layer );

  void                        addKeyWordList ( const std::string& kwl );
  void                        addLayer       ( ossimPlanetTextureLayer * layer );
  void                        removeLayer    ( ossimPlanetTextureLayer * layer );

  void                        setLayerOperation( const std::string&, int val, ossimPlanetTextureLayer * layer );

  typedef OsgTools::Animate::Settings Settings;

  void                        timestepType( Settings::TimestepType type );
  Settings::TimestepType      timestepType( ) const;

  void                        startAnimation( float speed, bool accumulate, bool timeWindow, int numDays );
  void                        stopAnimation();

  void                        resize ( unsigned int width, unsigned int height );

  bool                        elevationEnabled() const;
  void                        elevationEnabled( bool val );

  bool                        hudEnabled() const;
  void                        hudEnabled( bool val );

  bool                        ephemerisFlag() const;
  void                        ephemerisFlag( bool val );

  float                       elevationExag() const;
  void                        elevationExag( float elevExagVal );

  int                         elevationPatchSize() const;
  void                        elevationPatchSize( float elevEstimateVal );

  int                         levelDetail() const;
  void                        levelDetail( float levelDetailVal );

  std::string                 elevationCacheDir() const;
  void                        elevationCacheDir( const std::string& directory );

  bool                        latLongGrid() const;
  void                        latLongGrid( bool b );

  bool                        showLegend() const;
  void                        showLegend( bool b );

  /// For now
  void                        viewer( Usul::Interfaces::IUnknown* viewer );

protected:
  virtual ~MinervaDocument();

  MinervaDocument( const MinervaDocument& );
  MinervaDocument& operator=( const MinervaDocument& );

  /// Usul::Interfaces::IMatrixManipulator
  virtual osgGA::MatrixManipulator *       getMatrixManipulator ();

  /// Usul::Interfaces::IDatabasePager
  virtual osgDB::DatabasePager *           getDatabasePager ();

  /// Usul::Interfaces::IUpdateScene
  virtual void                             sceneUpdate( );
private:
  Minerva::Core::Scene::SceneManager::RefPtr _sceneManager;
  osg::ref_ptr < Magrathea::Planet > _planet;
};


}
}

#endif // __MINERVA_DOCUMENT_H__
