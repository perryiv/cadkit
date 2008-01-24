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
#include "Minerva/Document/Planet.h"
#include "Minerva/Config.h"

#include "Usul/Documents/Document.h"
#include "Usul/Policies/Update.h"
#include "Usul/Jobs/Job.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ICommandExecuteListener.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ILayerList.h"
#include "Usul/Interfaces/IMatrixManipulator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "Minerva/Core/Animate/Settings.h"
#include "Minerva/Core/Animate/TimeSpan.h"
#include "Minerva/Core/Scene/SceneManager.h"
#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Interfaces/IAnimationControl.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IRemoveLayer.h"
#include "Minerva/Interfaces/IDirtyScene.h"

#include "MenuKit/Menu.h"

#include "OsgTools/Legend/Legend.h"

#include "Serialize/XML/Macros.h"

#include "osg/Camera"
#include "osgText/Text"

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
                                                public Usul::Interfaces::ILayerList,
                                                public Usul::Interfaces::IMenuAdd,
                                                public Usul::Interfaces::ICommandExecuteListener,
                                                public Usul::Interfaces::IPlanetCoordinates,
                                                public Usul::Interfaces::IElevationDatabase
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::vector < Usul::Interfaces::ILayer::QueryPtr > Layers;
  typedef Minerva::Core::Scene::SceneManager SceneManager;
  typedef Minerva::Core::Animate::Settings Settings;
  typedef Minerva::Core::Animate::TimeSpan TimeSpan;
  typedef std::vector < TimeSpan::RefPtr > TimeSpans;
  typedef Minerva::Interfaces::IAnimationControl IAnimationControl;

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
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  void                        viewLayerExtents ( Usul::Interfaces::IUnknown * layer );

  /// Animation methods.
  void                                     timestepType( IAnimationControl::TimestepType type );
  IAnimationControl::TimestepType          timestepType( ) const;

#if USE_STAR_SYSTEM == 0
  /// Get/Set the split metric.
  void                                     splitMetric ( double );
  bool                                     isSplitMetric ( double ) const;
#endif

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

  /// Start the animation (Minerva::Interfaces::IAnimationControl).
  virtual void                             startAnimation ();
  virtual void                             stopAnimation();
  virtual void                             pauseAnimation();

  /// Get/Set the animate speed (Minerva::Interfaces::IAnimationControl).
  virtual void                             animateSpeed ( double speed );
  virtual double                           animateSpeed () const;

  /// Get/Set the show past events flag (Minerva::Interfaces::IAnimationControl).
  virtual void                             showPastEvents ( bool );
  virtual bool                             showPastEvents () const;

  /// Have visitor visit all layes.
  void                                     accept ( Minerva::Core::Visitor& visitor );

  /// Set the current time span.
  void                                     currentTimeSpan ( TimeSpan::RefPtr span );
  bool                                     isCurrentTimeSpan ( TimeSpan::RefPtr span ) const;
  
  enum LegendPosition
  {
    LEGEND_TOP_LEFT,
    LEGEND_TOP_RIGHT,
    LEGEND_BOTTOM_RIGHT,
    LEGEND_BOTTOM_LEFT
  };
  
  /// Get/Set the legend position.
  void                                     legendPosition ( LegendPosition position );
  LegendPosition                           legendPosition ( ) const;
  
  /// Toggle the legend on and off.
  bool                                     isShowLegend() const;
  void                                     showLegend( bool b );
  
  /// Legend width in percentage of screen width.
  void                                     legendWidth( float p );
  float                                    legendWidth( ) const;
  
  /// Height of each item showing in the legend.
  void                                     legendHeightPerItem( unsigned int height );
  unsigned int                             legendHeightPerItem() const;
  
  /// Legend padding from the sides of the screen.  Value in pixels.
  void                                     legendPadding( const osg::Vec2& padding );
  const osg::Vec2&                         legendPadding() const;
  
  /// Get/Set the dirty flag.
  void                                     dirty( bool );
  bool                                     dirty() const;
  
protected:
  virtual ~MinervaDocument();

  MinervaDocument( const MinervaDocument& );
  MinervaDocument& operator=( const MinervaDocument& );

  /// Usul::Interfaces::IMatrixManipulator
  virtual osgGA::MatrixManipulator *       getMatrixManipulator ();

  /// Distributed functions.
  void                                     _connectToDistributedSession();

  /// Execute a command.
  void                                     _executeCommand ( Usul::Interfaces::ICommand* command );

  /// Animate.
  void                                     _animate ( Usul::Interfaces::IUnknown *caller );

  /// Build the scene.
  void                                     _buildScene ( Usul::Interfaces::IUnknown *caller );
  void                                     _buildLegend( Usul::Interfaces::IUnknown *caller );
  
  void                                     _setLegendPosition ( unsigned int legendWidth );

  /// Build menus.
  void                                     _buildTimeSpanMenu();
  void                                     _buildLayerMenu();

  /// Find first and last date.
  void                                     _findFirstLastDate();
  
  /// Make the planet.
  void                                     _makePlanet();

  /// Minerva::Interfaces::IAddLayer
  virtual void                             addLayer ( Usul::Interfaces::ILayer * layer );

  /// Minerva::Interfaces::IRemoveLayer
  virtual void                             removeLayer ( Usul::Interfaces::ILayer * layer );

  /// Dirty the scene ( Minerva::Interfaces::IDirtyScene ).
  virtual void                             dirtyScene ( Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Get the number of layers ( Usul::Interfaces::ILayerList ).
  virtual unsigned int                     numberLayers () const;

  /// Get the layer at position i ( Usul::Interfaces::ILayerList ).
  virtual Usul::Interfaces::ILayer*        layer ( unsigned int i );

  /// Add to the menu.
  virtual void                             menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  // Command is executed.
  virtual void                             commandExecuteNotify ( Usul::Commands::Command* command );

  /// Convert to planet coordinates.
  virtual void                             convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const;
  virtual void                             convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const;

  // Get the elevation at a lat, lon (IElevationDatabase).
  virtual double                           elevationAtLatLong ( double lat, double lon ) const;
  
private:
  
  bool _dirty;

  Layers _layers;
  MenuKit::Menu::RefPtr _layersMenu;

  Minerva::Core::Scene::SceneManager::RefPtr _sceneManager;
  
  osg::ref_ptr < osg::Group >      _root;
  osg::ref_ptr < osg::Camera >     _camera;
  osg::ref_ptr < osgText::Text >   _dateText;

  Planet::RefPtr _planet;

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
  Minerva::Core::Animate::Date _lastDate;
  TimeSpan::RefPtr _global;
  TimeSpan::RefPtr _current;
  TimeSpans _timeSpans;
  bool _datesDirty;
  double _lastTime;
  double _animationSpeed;
  MenuKit::Menu::RefPtr _timeSpanMenu;
  
  /// Legend members.
  OsgTools::Legend::Legend::RefPtr _legend;
  bool _showLegend;
  float _legendWidth;
  unsigned int _legendHeightPerItem;
  osg::Vec2 _legendPadding;
  LegendPosition _legendPosition;
  
  /// Screen size members.
  unsigned int _width;
  unsigned int _height;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME( MinervaDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  virtual void deserialize ( const XmlTree::Node &node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


}
}

#endif // __MINERVA_DOCUMENT_H__
