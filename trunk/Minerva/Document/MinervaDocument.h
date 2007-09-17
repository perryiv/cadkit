
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

#include "Minerva/Document/Export.h"
#include "Minerva/Document/CommandSender.h"
#include "Minerva/Document/CommandReceiver.h"

#include "Usul/Documents/Document.h"
#include "Usul/Policies/Update.h"
#include "Usul/Jobs/Job.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IMatrixManipulator.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/ILayerList.h"

#include "Minerva/Core/Animate/Settings.h"
#include "Minerva/Core/Scene/SceneManager.h"
#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Interfaces/IAnimationControl.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IRemoveLayer.h"
#include "Minerva/Interfaces/IDirtyScene.h"

#include "Magrathea/Planet.h"

#include "Serialize/XML/Macros.h"

class ossimPlanetTextureLayer;

namespace Usul { namespace Interfaces { struct ICommand; } }

namespace Minerva { namespace Core { class Visitor; } }

namespace Minerva {
namespace Document {

class MINERVA_DOCUMENT_EXPORT MinervaDocument : public Usul::Documents::Document,
                                                public Usul::Interfaces::IBuildScene,
                                                public Usul::Interfaces::IMatrixManipulator,
                                                public Usul::Interfaces::IUpdateListener,
                                                public Minerva::Interfaces::IAnimationControl,
                                                public Minerva::Interfaces::IAddLayer,
                                                public Minerva::Interfaces::IRemoveLayer,
                                                public Minerva::Interfaces::IDirtyScene,
                                                public Usul::Interfaces::ILayerList
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::vector< std::string > Names;
  typedef std::map < std::string, Usul::Interfaces::ILayer::QueryPtr > Favorites;
  typedef std::vector < Usul::Interfaces::ILayer::QueryPtr > Layers;
  typedef Minerva::Core::Scene::SceneManager SceneManager;
  typedef Minerva::Core::Animate::Settings Settings;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( MinervaDocument );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MinervaDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  MinervaDocument();

  /// Add the view to the document.
  virtual void                addView ( Usul::Interfaces::IView *view );

  /// Usul::Interfaces::IRenderListener inherited from base class.
  virtual void                postRenderNotify ( Unknown *caller );
  virtual void                preRenderNotify ( Unknown *caller );

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

  void                        viewLayerExtents ( Usul::Interfaces::IUnknown * layer );

  void                        setLayerOperation( const std::string&, int val, Usul::Interfaces::IUnknown * layer );

  /// Animation methods.
  void                        timestepType( Settings::TimestepType type );
  Settings::TimestepType      timestepType( ) const;
  
  void                        startAnimationCommand();
  void                        stopAnimationCommand();
  void                        pauseAnimationCommand();
  void                        animationSpeedCommand ( double value );

  void                        addLayerCommand ( Usul::Interfaces::ILayer * layer );
  void                        removeLayerCommand ( Usul::Interfaces::ILayer * layer );
  void                        modifyLayerCommand ( Usul::Interfaces::ILayer * layer );
  void                        showLayerCommand ( Usul::Interfaces::ILayer * layer );
  void                        hideLayerCommand ( Usul::Interfaces::ILayer * layer );

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

  /// Get/Set the percentage of screen the legend uses.
  void                                     percentScreenWidth ( float );
  float                                    percentScreenWidth();

  /// For now
  void                                     viewer ( Usul::Interfaces::IUnknown* viewer );

  void                                     addToFavorites( Usul::Interfaces::IUnknown* layer );
  Usul::Interfaces::IUnknown *             createFavorite( const std::string& name ) const;
  Names                                    favorites() const;

  Layers&                                  layers();
  const Layers&                            layers() const;

  /// Get/Set the connection.
  void                                     connection ( Minerva::Core::DB::Connection* connection );
  Minerva::Core::DB::Connection*           connection ();

  /// Get/Set the session.
  void                                     session ( const std::string& session );
  const std::string &                      session () const;

  /// Connect to the session.
  void                                     connectToSession ();

  /// Update (Usul::Interfaces::IUpdateListener).
  virtual void                             updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Get/Set the commands receive flag.
  void                                     commandsReceive ( bool b );
  bool                                     commandsReceive () const;

  /// Get the scene manager.
  SceneManager *                           sceneManager ();
  const SceneManager *                     sceneManager () const;

  /// Get the planet.
  Magrathea::Planet*                       planet ();

  /// Get the database pager.
  virtual osgDB::DatabasePager *           getDatabasePager ();

  /// Start the animation (Minerva::Interfaces::IAnimationControl).
  virtual void                             startAnimation ();
  virtual void                             stopAnimation();
  virtual void                             pauseAnimation();

  /// Get/Set the animate speed (Minerva::Interfaces::IAnimationControl).
  virtual void                             animateSpeed ( double speed );
  virtual double                           animateSpeed () const;

  /// Have visitor visit all layes.
  void                                     accept ( Minerva::Core::Visitor& visitor );
protected:
  virtual ~MinervaDocument();

  MinervaDocument( const MinervaDocument& );
  MinervaDocument& operator=( const MinervaDocument& );

  /// Usul::Interfaces::IMatrixManipulator
  virtual osgGA::MatrixManipulator *       getMatrixManipulator ();

  /// Distributed functions.
  void                                     _connectToDistributedSession();

  /// Add a layer.
  void                                     _addLayer ( Usul::Interfaces::ILayer * layer );

  /// Execute a command.
  void                                     _executeCommand ( Usul::Interfaces::ICommand* command );

  /// Animate.
  void                                     _animate ( Usul::Interfaces::IUnknown *caller );

  /// Minerva::Interfaces::IAddLayer
  virtual void                             addLayer ( Usul::Interfaces::ILayer * layer );

  /// Minerva::Interfaces::IRemoveLayer
  virtual void                             removeLayer ( Usul::Interfaces::ILayer * layer );

  /// Dirty the scene ( Minerva::Interfaces::IDirtyScene ).
  virtual void                             dirtyScene ();

  /// Get the number of layers ( Usul::Interfaces::ILayerList ).
  virtual unsigned int                  numberLayers () const;

  /// Get the layer at position i ( Usul::Interfaces::ILayerList ).
  virtual Usul::Interfaces::ILayer*     layer ( unsigned int i );

private:

  Layers _layers;
  Favorites _favorites;
  Minerva::Core::Scene::SceneManager::RefPtr _sceneManager;
  osg::ref_ptr < Magrathea::Planet > _planet;

  /// Command members.
  bool _commandsSend;
  bool _commandsReceive;
  std::string _sessionName;
  CommandSender::RefPtr   _sender;
  CommandReceiver::RefPtr _receiver;
  Minerva::Core::DB::Connection::RefPtr _connection;
  Usul::Policies::TimeBased  _commandUpdate;
  Usul::Jobs::Job::RefPtr    _commandJob;

  /// Animation members.
  Minerva::Core::Animate::Settings::RefPtr  _animateSettings;
  Minerva::Core::Animate::Date _minDate;
  Minerva::Core::Animate::Date _maxDate;
  Minerva::Core::Animate::Date _lastDate;
  bool _datesDirty;
  double _lastTime;
  double _animationSpeed;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME( MinervaDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  virtual void deserialize ( const XmlTree::Node &node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


}
}

#endif // __MINERVA_DOCUMENT_H__
