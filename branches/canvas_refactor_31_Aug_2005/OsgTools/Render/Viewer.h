
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer class that handles rendering and interaction.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_VIEWER_H_
#define _OPEN_SCENE_GRAPH_TOOLS_VIEWER_H_

#include "OsgTools/Export.h"
#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Render/Animation.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/ContextMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Types/Types.h"
#include "Usul/Math/Vector2.h"

#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Interfaces/ISelectionBox.h"
#include "Usul/Interfaces/IProjectionMatrix.h"
#include "Usul/Interfaces/IRender.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Interfaces/ITimerServer.h"
#include "Usul/Interfaces/IRenderNotify.h"
#include "Usul/Interfaces/ISwapBuffers.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/ISpin.h"
#include "Usul/Interfaces/IBoundingSphere.h"
#include "Usul/Interfaces/IBoundingBox.h"
#include "Usul/Interfaces/IWriteScene.h"
#include "Usul/Interfaces/IWriteImage.h"
#include "Usul/Interfaces/IStereo.h"

#include "osg/ref_ptr"
#include "osg/Vec4"
#include "osg/LOD"
#include "osg/Plane"
#include "osg/ClipNode"
#include "osg/ShadeModel"
#include "osg/PolygonMode"

#include "osgText/Text"

#include "osgUtil/SceneView"
#include "osgUtil/IntersectVisitor"

#include "osgGA/MatrixManipulator"

#include <utility>
#include <vector>
#include <stack>


namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT Viewer : private Usul::Base::Referenced,
                                public Usul::Interfaces::IFrameDump,
                                public Usul::Interfaces::ISelectionBox,
                                public Usul::Interfaces::IProjectionMatrix,
                                public Usul::Interfaces::IRender,
                                public Usul::Interfaces::IViewport,
                                public Usul::Interfaces::ITimerNotify,
                                public Usul::Interfaces::IGetBoundingBox,
                                public Usul::Interfaces::IPolygonMode,
                                public Usul::Interfaces::IShadeModel,
                                public Usul::Interfaces::ISpin,
                                public Usul::Interfaces::IBoundingSphere,
                                public Usul::Interfaces::IBoundingBox,
                                public Usul::Interfaces::IWriteScene,
                                public Usul::Interfaces::IWriteImage,
                                public Usul::Interfaces::IStereo

{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex RecursiveMutex;
  typedef Usul::Threads::ContextMutex ContextMutex;
  typedef Usul::Threads::Guard < RecursiveMutex > RecursiveGuard;
  typedef Usul::Threads::Guard < ContextMutex > ContextGuard;
  typedef osgUtil::SceneView SceneView;
  typedef osg::ref_ptr < SceneView > SceneViewPtr;
  typedef Usul::Interfaces::IDocument IDocument;
  typedef osg::ref_ptr < osg::Group > GroupPtr;
  typedef osg::ref_ptr < osg::ClipNode > ClipPtr;
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector < Filter > Filters;
  typedef osgGA::MatrixManipulator NavManip;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IPolygonMode IPolygonMode;
  typedef Usul::Interfaces::IShadeModel IShadeModel;
  typedef Usul::Interfaces::IShadeModel IStereo;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor
  Viewer ( ContextMutex *context, IDocument *doc, IUnknown *unknown );

  // Animate.
  void                            animate ( const osg::Vec3d &t1, const osg::Quat &r1, 
                                            const osg::Vec3d &t2, const osg::Quat &r2 );

  // Set/get the background color.
  void                            backgroundColor ( const osg::Vec4f &color );
  osg::Vec4f                      backgroundColor() const;

  // Bounding-box.
  virtual void                    boundingBoxVisible ( bool show );
  virtual bool                    boundingBoxVisible() const;
  virtual osg::BoundingBox        boundingBoxGet() const;

  // Set/get the bounding-sphere visible state.
  virtual void                    boundingSphereVisible ( bool show );
  virtual bool                    boundingSphereVisible() const;

  // Clipping planes.
  void                            clipPlaneAdd ( const osg::Plane &plane );
  void                            clipPlaneAddDefault();
  void                            clipPlanesAdd ( const osg::BoundingBox &box );
  void                            clipPlaneRemove ( unsigned int index );
  void                            clipPlanesRemoveAll();
  unsigned int                    clipPlanesGetNum() const;

  // Set the default background.
  void                            defaultBackground();

  // Display-list state.
  void                            displayListsUpdate();
  static void                     displayListsUsage ( bool );
  static bool                     displayListsUsage();

  // Get the filters.
  virtual Filters                 filtersWriteScene() const;
  virtual Filters                 filtersWriteImage() const;

  // Frame dumping.
  void                            frameDump() const;
  virtual void                    frameDumpState ( bool );
  virtual bool                    frameDumpState() const;
  virtual unsigned int            frameDumpCurrentFileNum() const;
  virtual void                    frameDumpResetCounter();
  virtual void                    frameDumpProperties ( const std::string &dir, const std::string &base, const std::string &ext, unsigned int start = 0, unsigned int digits = 10 );

  // Pass false for "justModel" to intersect with the entire scene.
  bool                            intersect ( const osg::Vec2d &mouse, bool justModel, osgUtil::Hit &hit ) const;
  void                            intersect ( const osg::Vec2d &mouse, bool justModel, osgUtil::IntersectVisitor::HitList &hits ) const;

  // Initialize state.
  void                            init();

  // Lighting.
  void                            lighting ( bool );
  bool                            lighting() const;
  void                            lightBothSides ( bool );
  bool                            lightBothSides() const;

  // Get the line segment into the scene that goes through the 2D coordinate.
  void                            lineSegment ( const osg::Vec2d &mouse, osg::Vec3d &pt0, osg::Vec3d &pt1 ) const;

  // Set the callback to be used when deciding which LOD child to traverse.
  void                            lodCullCallback ( osg::NodeCallback *cb );
  void                            lodCullCallbackLow();
  void                            lodCullCallbackHigh();

  // Set the model.
  void                            model ( osg::Node * );

  // Navigation history.
  void                            navHistoryAppend();
  bool                            navHistoryCanRedo() const;
  bool                            navHistoryCanUndo() const;
  NavManip *                      navHistoryCloneCurrent() const;
  void                            navHistoryRedo();
  void                            navHistoryUndo();

  // Push/pop the navigation manipulator.
  NavManip *                      navManipCloneCurrent() const;
  void                            navManipPop();
  void                            navManipPush ( NavManip * );

  // Get the clipping plane distances.
  void                            nearFar ( double &n, double &f ) const;

  // Convert the pixel coordinates to normalized coordinates in the range [-1,1].
  osg::Vec2d                      normalizedMouse ( const Usul::Math::Vec2ui &mouse ) const;

  // Set/get the number of rendering passes. Unavailable numbers have no effect.
  void                            numRenderPasses ( unsigned int );
  unsigned int                    numRenderPasses() const;

  // Render the scene.
  virtual void                    render();

  // Resize the window.
  void                            resize ( unsigned int width, unsigned int height );

  // Rotate.
  void                            rotate ( const osg::Vec3d &axis, double radians );

  // Set/get projection matrix.
  virtual void                    projectionMatrix ( double left, double right, double bottom, double top, double zNear, double zFar );
  virtual bool                    projectionMatrix ( double &fovy, double &aspect, double &zNear, double &zFar );

  // Set/get the polygon mode.
  virtual void                    polygonMode ( IPolygonMode::Mode mode );
  virtual IPolygonMode::Mode      polygonMode() const;

  // Usul::Interfaces::ISelectionBox.
  virtual void                    selectionBoxSet ( const osg::Vec3d &, const osg::Vec3d & );
  virtual void                    selectionBoxRemove();

  // Set/get the shade model.
  virtual void                    shadeModel ( IShadeModel::Mode mode );
  virtual IShadeModel::Mode       shadeModel() const;

  // Get/Set back to front sorting.
  bool                            sortBackToFront() const;
  void                            sortBackToFront ( bool );

  // Spinning.
  virtual void                    spin ( bool );
  static void                     spinAllowed ( bool );
  static bool                     spinAllowed();

  // Stereo.
  virtual void                    stereoMode ( IStereo::Mode );
  virtual IStereo::Mode           stereoMode() const;
  virtual void                    stereoEyeDistance ( float );
  virtual float                   stereoEyeDistance() const;

  // Text in the view.
  virtual void                    textCellSet      ( double x, double y, unsigned int row, unsigned int col, const std::string &text );
  virtual void                    textMatrixRemove ( double x, double y );
  virtual void                    textMatrixResize ( double x, double y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth );

  // Time functions.
  Usul::Types::Uint64             timeLast    ( const std::string &name ) const;
  Usul::Types::Uint64             timeAverage ( const std::string &name ) const;
  void                            timeInfo    ( std::string & ) const;
  void                            timeStart   ( const std::string &name );
  void                            timeStop    ( const std::string &name );

  // Notification of a timer event. Return false to cancel the timer.
  bool                            timerEventNotify ( unsigned int id, IUnknown *caller );

  // Update the scene.
  void                            update();

  // Set/get the viewport.
  virtual void                    viewport ( int  x, int  y, unsigned int  width, unsigned int  height );
  virtual void                    viewport ( int &x, int &y, unsigned int &width, unsigned int &height ) const;
  unsigned int                    viewportHeight() const;
  unsigned int                    viewportWidth() const;

  // Write a file.
  virtual bool                    writeImageFile ( const std::string &name ) const;
  virtual bool                    writeImageFile ( const std::string &name, double percent ) const;
  virtual bool                    writeImageFile ( const std::string &name, unsigned int height, unsigned int width ) const;
  virtual bool                    writeSceneFile ( const std::string &name ) const;

protected:

  // Do not copy.
  Viewer ( const Viewer & );
  Viewer &operator = ( const Viewer & );

  // Use reference counting.
  virtual ~Viewer();

  bool                            _animate();

  void                            _cullAndDraw();

  osg::Group *                    _getGroup ( const std::string &key );
  osg::Node *                     _getModel();
  const osg::Node *               _getModel() const;

  void                            _multiPassRender();

  const NavManip *                _navManip() const;
  NavManip *                      _navManip();

  void                            _removeGroup ( const std::string &key );

  void                            _setModel ( osg::Node * );
  void                            _singlePassRender();
  bool                            _spin();

  IUnknown *                      _unknown();

  bool                            _writeImageFile ( const std::string &name, unsigned int height, unsigned int width );

private:

  // Possible flags.
  enum
  {
    _HAS_ACCUM_BUFFER   = 0x00000001,
    _HIDDEN_LINES       = 0x00000002,
    _SORT_BACK_TO_FRONT = 0x00000004,
  };

  // Timer ids.
  enum 
  {
    ID_ANIMATION_TIMEOUT,
    ID_SPIN_TIMEOUT,
  };

  // Local integer constants.
  enum
  {
    _SPIN_TIMER_MILLISECONDS         = 15,
    _ANIMATION_TIMER_MILLISECONDS    = 15, // 15 ms will give about 60 FPS for OpenGL rendering.
    _ANIMATION_DURATION_MILLISECONDS = 500 // Half a second.
  };

  typedef std::pair < bool, osg::Matrixd > CameraBuffer;
  typedef std::map < std::string, GroupPtr > GroupMap;
  typedef osg::ref_ptr < osg::Geode > GeodePtr;
  typedef Usul::Interfaces::ITimerServer ITimerServer;
  typedef Usul::Interfaces::IRenderNotify IRenderNotify;
  typedef Usul::Interfaces::ISwapBuffers ISwapBuffers;

  typedef osg::ref_ptr < NavManip > NavManipPtr;
  typedef std::stack < NavManipPtr > NavManips;
  typedef std::vector < NavManipPtr > NavHistoryList;
  typedef std::pair < NavHistoryList, unsigned int > NavHistory;

  typedef std::pair < Usul::Types::Uint64, Usul::Types::Uint64 > TimePair;
  typedef std::vector < TimePair > TimeHistory;
  typedef std::map < std::string, TimeHistory > TimeHistories;

  typedef std::pair < unsigned int, unsigned int > TextMatrixProperties;
  typedef osg::ref_ptr < osgText::Text > TextPtr;
  typedef std::vector < std::vector < TextPtr > > TextMatrix;
  typedef std::pair < TextMatrix, GeodePtr > TextMatrixPair;
  typedef std::pair < TextMatrixPair, TextMatrixProperties > TextMatrixData;
  typedef std::pair < double, double > TextKey;
  typedef std::map < TextKey, TextMatrixData > TextMap;

  typedef osg::ref_ptr < osg::LOD > LodPtr;
  typedef std::vector < LodPtr > LodList;
  typedef std::pair < bool, LodList > Lods;

  mutable RecursiveMutex _members;
  mutable ContextMutex::ValidRefPtr _context;
  SceneViewPtr _sceneView;
  IDocument::RefPtr _document;
  unsigned int _numPasses;
  unsigned int _flags;
  GroupPtr _root;
  ClipPtr _clipped;
  GroupPtr _unclipped;
  GroupPtr _model;
  GroupPtr _decorator;
  GroupMap _groupMap;
  FrameDump _frameDump;
  TimeHistories _times;
  Animation _animation;
  TextMap _textMap;
  Lods _lods;
  NavManips _navManips;
  NavHistory _navHistory;
  ITimerServer::QueryPtr _timerServer;
  IRenderNotify::QueryPtr _renderNotify;
  ISwapBuffers::QueryPtr _swapBuffers;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_VIEWER_H_
