
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

#include "Minerva/Config.h"
#include "Minerva/Document/Export.h"

#if USE_DISTRIBUTED == 1
#include "Minerva/Document/CommandSender.h"
#include "Minerva/Document/CommandReceiver.h"

#include "Minerva/DataSources/PG/Connection.h"
#endif

#include "Usul/Documents/Document.h"

#include "Minerva/Core/Animate/Settings.h"
#include "Minerva/Core/Data/TimeSpan.h"
#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/Utilities/Hud.h"
#include "Minerva/Core/Navigator.h"
#include "Minerva/Interfaces/IAnimationControl.h"
#include "Minerva/Interfaces/IDirtyScene.h"
#include "Minerva/Interfaces/ILookAtLayer.h"

#include "MenuKit/Menu.h"

#include "OsgTools/Widgets/Legend.h"

#include "Serialize/XML/Macros.h"

#include "Usul/File/Log.h"
#include "Usul/Interfaces/IAnimationNotify.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IBusyState.h"
#include "Usul/Interfaces/ICommandExecuteListener.h"
#include "Usul/Interfaces/IContextMenuAdd.h"
#include "Usul/Interfaces/IHandleSeek.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/IJobFinishedListener.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IMatrixManipulator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IMouseEventListener.h"
#include "Usul/Interfaces/IProjectionMatrix.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Policies/Update.h"

#include "osg/Camera"
#include "osgText/Text"

namespace Usul { namespace Interfaces { struct ICommand; } }

namespace Minerva { namespace Core { class Visitor; } }
namespace Minerva { namespace Core { namespace Data { class DataObject; } } }

namespace Minerva {
namespace Document {

class MINERVA_DOCUMENT_EXPORT MinervaDocument : public Usul::Documents::Document,
                                                public Usul::Interfaces::IAnimationNotify,
                                                public Usul::Interfaces::IBuildScene,
                                                public Usul::Interfaces::IMatrixManipulator,
                                                public Usul::Interfaces::IUpdateListener,
                                                public Minerva::Interfaces::IAnimationControl,
                                                public Minerva::Interfaces::IDirtyScene,
                                                public Usul::Interfaces::IMenuAdd,
                                                public Usul::Interfaces::ICommandExecuteListener,
                                                public Usul::Interfaces::IIntersectListener,
                                                public Usul::Interfaces::ITreeNode,
                                                public Usul::Interfaces::IJobFinishedListener,
                                                public Usul::Interfaces::IMouseEventListener,
                                                public Minerva::Interfaces::ILookAtLayer,
                                                public Usul::Interfaces::IBusyState,
                                                public Usul::Interfaces::IContextMenuAdd,
                                                public Usul::Interfaces::IProjectionMatrix,
                                                public Usul::Interfaces::IHandleSeek
{
public:
  
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Minerva::Core::Animate::Settings Settings;
  typedef Minerva::Core::Data::TimeSpan TimeSpan;
  typedef Minerva::Core::Data::Container::ObjectID ObjectID;
  typedef std::vector < TimeSpan::RefPtr > TimeSpans;
  typedef Minerva::Interfaces::IAnimationControl IAnimationControl;
  typedef Minerva::Core::TileEngine::Body Body;
  typedef std::vector<Body::RefPtr> Bodies;
  typedef Usul::File::Log::RefPtr LogPtr;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( MinervaDocument );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MinervaDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  MinervaDocument ( LogPtr log = LogPtr ( 0x0 ) );

  /// Add the view to the document.
  virtual void                             addView ( Usul::Interfaces::IView *view );

  /// Usul::Interfaces::IRenderListener inherited from base class.
  virtual void                             postRenderNotify ( Unknown *caller );
  virtual void                             preRenderNotify ( Unknown *caller );

  /// Build the scene.
  osg::Node *                              buildScene ( Unknown *caller = 0x0 );
  virtual osg::Node *                      buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                             canExport ( const std::string &ext ) const;
  virtual bool                             canInsert ( const std::string &ext ) const;
  virtual bool                             canOpen   ( const std::string &ext ) const;
  virtual bool                             canSave   ( const std::string &ext ) const;

  /// Return the job to close this document. Default is null.
  virtual Usul::Jobs::Job *                closeJob();

  /// Get the filters that correspond to what this document can do.
  virtual Filters                          filtersExport() const;
  virtual Filters                          filtersInsert() const;
  virtual Filters                          filtersOpen()   const;
  virtual Filters                          filtersSave()   const;

  /// Read the file and add it to existing document's data.
  virtual void                             read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  virtual void                             removeView ( View *view );
  
  /// Write the document to given file name. Does not rename this document.
  virtual void                             write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Clear any existing data.
  virtual void                             clear ( Unknown *caller = 0x0 );

  virtual void                             lookAtLayer ( Usul::Interfaces::IUnknown * layer );
  virtual void                             lookAtPoint ( const Usul::Math::Vec2d& point );
  
  /// Animation methods.
  void                                     timestepType( IAnimationControl::TimestepType type );
  IAnimationControl::TimestepType          timestepType( ) const;

  /// Update (Usul::Interfaces::IUpdateListener).
  virtual void                             updateNotify ( Usul::Interfaces::IUnknown *caller );

#if USE_DISTRIBUTED == 1
  
  /// Get/Set the connection.
  void                                     connection ( Minerva::DataSources::PG::Connection* connection );
  Minerva::DataSources::PG::Connection*    connection();
  
  /// Connect to the session.
  void                                     connectToSession();
  
  /// Get/Set the session.
  void                                     session ( const std::string& session );
  const std::string &                      session () const;

  /// Get/Set the commands receive flag.
  void                                     commandsReceive ( bool b );
  bool                                     commandsReceive () const;

#endif
  
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

  /// Get the busy state. (Usul::Interfaces::IBusyState).
  virtual bool                             busyStateGet() const;

  /// Usul::Interfaces::IAnimationNotify. 
  virtual void                             animationStarted(){}
  virtual void                             animationStep ( unsigned int step, unsigned int totalSteps ){}
  virtual void                             animationStopped();

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

  /// Get/Set show compass state.
  bool                                     isShowCompass() const;
  void                                     showCompass( bool b );
  
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
  
  /// Toggle the skirts on and off.
  bool                                     isShowSkirts() const;
  void                                     showSkirts ( bool b );
  
  /// Toggle the borders on and off.
  bool                                     isShowBorders() const;
  void                                     showBorders ( bool b );
  
  /// Get/Set the active body.
  void                                     activeBody ( Body* );
  Body*                                    activeBody() const;
  
  /// Toggle the freeze split flag.
  bool                                     isAllowSplit() const;
  void                                     allowSplit( bool b );

  /// Toggle the keep detail.
  bool                                     isKeepDetail() const;
  void                                     keepDetail ( bool b );

  /// Toggle showing of lat lon text.
  bool                                     isShowLatLonText() const;
  void                                     showLatLonText ( bool b );

  /// Toggle showing of job feedback.
  bool                                     isShowJobFeedback() const;
  void                                     showJobFeedback ( bool b );

  /// Toggle showing of date feedback.
  bool                                     isShowDateFeedback() const;
  void                                     showDateFeedback ( bool b );
  
  /// Toggle showing of eye altitude feedback.
  bool                                     isShowEyeAltitude() const;
  void                                     showEyeAltitude ( bool b );
  
  /// Toggle showing of meta-string feedback.
  bool                                     isShowMetaString() const;
  void                                     showMetaString ( bool b );
  
protected:

  virtual ~MinervaDocument();

  MinervaDocument( const MinervaDocument& );
  MinervaDocument& operator=( const MinervaDocument& );

  /// Usul::Interfaces::IMatrixManipulator
  virtual osgGA::MatrixManipulator *       getMatrixManipulator();

  /// Clear.
  void                                     _clear();
  
  /// Clear the balloon
  void                                     _clearBalloon();

  /// Distributed functions.
  void                                     _connectToDistributedSession();

  /// Destroy.
  void                                     _destroy();

  /// Animate.
  void                                     _animate ( Usul::Interfaces::IUnknown *caller );

  /// Build the scene.
  void                                     _buildScene ( Usul::Interfaces::IUnknown *caller );
  void                                     _buildLegend( Usul::Interfaces::IUnknown *caller );
  
  void                                     _setLegendPosition ( unsigned int legendWidth );

  /// Build menus.
  void                                     _buildTimeSpanMenu();
  void                                     _buildLayerMenu();
  void                                     _buildLayerSubMenu ( MenuKit::Menu& menu, Usul::Interfaces::ITreeNode* node );
  
  bool                                     _displayInformationBalloon ( Minerva::Core::Data::DataObject& );
  
  /// Find object.
  Usul::Interfaces::IUnknown::RefPtr       _findObject ( const ObjectID& objectID );

  /// Find first and last date.
  void                                     _findFirstLastDate();
  
  /// Intersect functions.
  bool                                     _intersectBalloon ( osgGA::GUIEventAdapter&, Usul::Interfaces::IUnknown * );
  bool                                     _intersectScene ( osgGA::GUIEventAdapter&, Usul::Interfaces::IUnknown * );
  
  /// Make the planet.
  void                                     _makePlanet();

  /// Resize all points.
  void                                     _resizePoints ( double factor );
  
  /// Increase/Decrease split distance.
  void                                     _increaseSplitDistance();
  void                                     _decreaseSplitDistance();
  
  /// Get the job manager.
  Usul::Jobs::Manager *                    _getJobManager();

  /// Set all the children's log.
  void                                     _setLog();
  
  /// Add to the context menu.
  virtual void                             contextMenuAdd ( MenuKit::Menu& menu, const Usul::Math::Vec2ui& position, Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Dirty the scene ( Minerva::Interfaces::IDirtyScene ).
  virtual bool                             dirtyScene() const;
  virtual void                             dirtyScene( bool b, Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Add to the menu.
  virtual void                             menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  // Command is executed.
  virtual void                             commandExecuteNotify ( Usul::Commands::Command* command );

  // Notify the observer of the intersection (IIntersectListener).
  virtual void                             intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller );

  // Get the number of children (ITreeNode).
  virtual unsigned int                     getNumChildNodes() const;

  // Get the child node (ITreeNode).
  virtual ITreeNode::RefPtr                getChildNode ( unsigned int which );

  // Set/get the name (ITreeNode).
  virtual void                             setTreeNodeName ( const std::string & );
  virtual std::string                      getTreeNodeName() const;

  /// The job has finished (IJobFinishedListener).
  virtual void                             jobFinished ( Usul::Jobs::Job *job );

  // Called when mouse event occurs.
  virtual void                             mouseEventNotify ( osgGA::GUIEventAdapter&, Usul::Interfaces::IUnknown * );

  // Get the projection matrix for the active view.
  virtual osg::Matrixd                     getProjectionMatrix() const;

  /// Seek to intersection point.
  virtual void                             handleSeek ( const Usul::Math::Vec3d& intersectionPoint );

private:
  
  // Callback to get the eye position.  This is a bit of a hack and needs to be improved.
  class Callback : public osg::NodeCallback
  {
  public:
    typedef osg::NodeCallback BaseClass;
    
    Callback () : BaseClass(), _hpr(), _eye(), _eyePosition(), _body ( 0x0 )
    {
    }
    
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
    
    osg::Vec3d _hpr;
    osg::Vec3d _eye;
    Usul::Math::Vec3d _eyePosition;
    MinervaDocument::Body *_body;
  };

  // Nested class for closing this document.
  struct CloseJob : public Usul::Jobs::Job
  {
    typedef Usul::Jobs::Job BaseClass;
    
    // Type information.
    USUL_IMPLEMENT_INLINE_TYPE_ID ( CloseJob );

    // Smart-pointer definitions.
    USUL_DECLARE_REF_POINTERS ( CloseJob );

    CloseJob ( MinervaDocument::RefPtr doc ) : BaseClass(), _doc ( doc )
    {
    }

  protected:

    virtual ~CloseJob()
    {
    }

    virtual void _started()
    {
      if ( true == _doc.valid() )
      {
        _doc->closing ( 0x0 );
      }
      _doc = 0x0;
    }

  private:

    MinervaDocument::RefPtr _doc;
  };

  /// Command members.
#if USE_DISTRIBUTED == 1
  bool _commandsSend;
  bool _commandsReceive;
  std::string _sessionName;
  CommandSender::RefPtr   _sender;
  CommandReceiver::RefPtr _receiver;
  Minerva::DataSources::PG::Connection::RefPtr _connection;
  Usul::Policies::TimeBased  _commandUpdate;
  Usul::Jobs::Job::RefPtr    _commandJob;
#endif
  
  bool _dirty;

  MenuKit::Menu::RefPtr _layersMenu;

  osg::ref_ptr<osg::Group>      _root;
  osg::ref_ptr<osg::Camera>     _camera;
  osg::ref_ptr<osg::Node>       _balloon;

  Bodies                     _bodies;
  Body::RefPtr               _activeBody;
  Usul::Jobs::Manager *      _manager;
  Minerva::Core::Utilities::Hud _hud;
  osg::ref_ptr < Callback >  _callback;
  
  /// Animation members.
  Minerva::Core::Animate::Settings::RefPtr  _animateSettings;
  Minerva::Core::Animate::Date _lastDate;
  TimeSpan::RefPtr _global;
  TimeSpan::RefPtr _current;
  TimeSpans _timeSpans;
  double _lastTime;
  double _animationSpeed;
  MenuKit::Menu::RefPtr _timeSpanMenu;
  
  /// Legend members.
  OsgTools::Widgets::Legend::RefPtr _legend;
  bool _showLegend;
  float _legendWidth;
  unsigned int _legendHeightPerItem;
  osg::Vec2 _legendPadding;
  LegendPosition _legendPosition;
  
  /// Screen size members.
  unsigned int _width;
  unsigned int _height;

  bool _showCompass;
  bool _allowSplit;
  bool _keepDetail;
  LogPtr _log;
  Minerva::Core::Navigator::RefPtr _navigator;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME( MinervaDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  virtual void deserialize ( const XmlTree::Node &node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


}
}

#endif // __MINERVA_DOCUMENT_H__
