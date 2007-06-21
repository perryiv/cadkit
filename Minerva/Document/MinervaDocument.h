
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
#include "Usul/Interfaces/IDistributedVR.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/ILayer.h"

#include "Minerva/Core/Scene/SceneManager.h"
#include "Minerva/Core/GUI/Controller.h"
#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Magrathea/Planet.h"

#include "Serialize/XML/Macros.h"

class ossimPlanetTextureLayer;

namespace Usul { namespace Interfaces { struct ICommand; } }

namespace Minerva {
namespace Document {

class MINERVA_DOCUMENT_EXPORT MinervaDocument : public Usul::Documents::Document,
                                                public Usul::Interfaces::IBuildScene,
                                                public Usul::Interfaces::ISceneUpdate,
                                                public Usul::Interfaces::IDatabasePager,
                                                public Usul::Interfaces::IMatrixManipulator,
                                                public Usul::Interfaces::IDistributedVR, 
                                                public Usul::Interfaces::IGroup,
                                                public Minerva::Interfaces::IAnimationControl
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::vector< std::string > Names;
  typedef std::map < std::string, Usul::Interfaces::ILayer::QueryPtr > Favorites;
  typedef OsgTools::Animate::Settings Settings;
  typedef std::vector < Usul::Interfaces::ILayer::QueryPtr > Layers;

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

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  void                        removeLayer ( Usul::Interfaces::ILayer * layer );
  void                        hideLayer   ( Usul::Interfaces::ILayer * layer );
  void                        showLayer   ( Usul::Interfaces::ILayer * layer );
  void                        addLayer    ( Usul::Interfaces::ILayer * layer, Unknown *caller = 0x0 );
  void                        modifyLayer ( Usul::Interfaces::ILayer * layer, Unknown *caller = 0x0 );

  void                        viewLayerExtents ( Usul::Interfaces::IUnknown * layer );

  void                        setLayerOperation( const std::string&, int val, Usul::Interfaces::IUnknown * layer );

  /// Animation methods.
  void                        timestepType( Settings::TimestepType type );
  Settings::TimestepType      timestepType( ) const;

  void                        startAnimation( float speed, bool accumulate, bool timeWindow, int numDays );
  
  void                        startAnimationCommand();
  void                        stopAnimationCommand();
  void                        animationSpeedCommand ( double value );

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

  void                        percentScreenWidth ( float );
  float                       percentScreenWidth();

  /// For now
  void                              viewer( Usul::Interfaces::IUnknown* viewer );

  void                              addToFavorites( Usul::Interfaces::IUnknown* layer );
  Usul::Interfaces::IUnknown *      createFavorite( const std::string& name ) const;
  Names                             favorites() const;

  Layers&                           layers();
  const Layers&                     layers() const;

protected:
  virtual ~MinervaDocument();

  MinervaDocument( const MinervaDocument& );
  MinervaDocument& operator=( const MinervaDocument& );

  /// Usul::Interfaces::IMatrixManipulator
  virtual osgGA::MatrixManipulator *       getMatrixManipulator ();

  /// Usul::Interfaces::IDatabasePager
  virtual osgDB::DatabasePager *           getDatabasePager ();

  /// Usul::Interfaces::IUpdateScene
  virtual void                             sceneUpdate( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Distributed functions.
  void                                     _connectToDistributedSession();

  void                                     _addLayer    ( Usul::Interfaces::ILayer * layer, Unknown *caller = 0x0 );

  /// Remove layer with given id.
  void                                     _removeLayerDistributed( Usul::Interfaces::ILayer *Layer );

  /// Show layer.
  void                                     _showLayerDistributed ( Usul::Interfaces::ILayer *Layer );

  /// Modify polygon data.
  void                                     _modifyLayerDistributed ( Usul::Interfaces::ILayer *layer );

  /// Start animation.
  void                                     _startAnimationDistributed ( float speed, bool accumulate, bool dateTimeStep, bool timeWindow, unsigned int numDays );

  /// Execute a command.
  void                                     _executeCommand ( Usul::Interfaces::ICommand* command );

  /// Minerva::Interfaces::IAnimationControl
  /// Start the animation.
  virtual void                             startAnimation ();
  virtual void                             stopAnimation();

  /// Get/Set the animate speed.
  virtual void                             animateSpeed ( double speed );
  virtual double                           animateSpeed () const;

  /// Usul::Interfaces::IDistributedVR
  virtual void playMovie ( const osg::Vec3f& position, const osg::Vec3f& width, const osg::Vec3f& height, const std::string& path );
  virtual void pause     ( );

  /// Implementing IGroup interface. 
  virtual osg::Group*  getGroup    ( const std::string& );
  virtual void         removeGroup ( const std::string& );
  virtual bool         hasGroup    ( const std::string& );

private:
  typedef osg::ref_ptr< osg::Group >         GroupPtr;
  typedef std::map < std::string, GroupPtr > GroupMap;

  Layers _layers;
  Favorites _favorites;
  Minerva::Core::Scene::SceneManager::RefPtr _sceneManager;
  osg::ref_ptr < Magrathea::Planet > _planet;

  bool _useDistributed;
  std::string _sessionName;
  Minerva::Core::GUI::Controller::RefPtr _distributed;
  GroupMap _groupMap;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME(MinervaDocument);
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  virtual void deserialize ( const XmlTree::Node &node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


}
}

#endif // __MINERVA_DOCUMENT_H__
