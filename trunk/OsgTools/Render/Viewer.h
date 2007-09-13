
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OSG viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_VIEWER_H__
#define __OSG_TOOLS_RENDER_VIEWER_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ICamera.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IExport.h"
#include "Usul/Interfaces/IBoundingSphere.h"
#include "Usul/Interfaces/IBoundingBox.h"
#include "Usul/Interfaces/IOpenGLState.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/ISelectionBox.h"
#include "Usul/Interfaces/IStereo.h"
#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IClippingPlanes.h"
#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IAxes.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IMode.h"
#include "Usul/Interfaces/ITool.h"
#include "Usul/Interfaces/ISetTool.h"
#include "Usul/Interfaces/ISpin.h"
#include "Usul/Interfaces/IBackground.h"
#include "Usul/Interfaces/IHeliosView.h"
#include "Usul/Interfaces/ILights.h"
#include "Usul/Interfaces/IOpenGLContext.h"
#include "Usul/Interfaces/ITimeoutAnimate.h"
#include "Usul/Interfaces/GUI/ISetCursorType.h"
#include "Usul/Interfaces/ITimeoutSpin.h"
#include "Usul/Interfaces/ICenterOfRotation.h"
#include "Usul/Interfaces/IScreenCapture.h"
#include "Usul/Interfaces/ISnapShot.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IRenderListener.h"
#include "Usul/Interfaces/IRenderNotify.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Render/Animation.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"

#include "OsgTools/Draggers/Dragger.h"
#include "OsgTools/Builders/GradientBackground.h"

#include "osgUtil/SceneView"

#include "osg/ref_ptr"
#include "osg/ShadeModel"
#include "osg/PolygonMode"
#include "osg/TexEnv"
#include "osg/Geode"

#include "osgText/Text"

#include "osgGA/MatrixManipulator"

#include "osgDB/DatabasePager"

#include <list>
#include <string>
#include <vector>

namespace osg { class ClipPlane; class NodeVisitor; }
namespace osgUtil { class Hit; class CullVisitor; }
namespace OsgTools { namespace Render { class Trackball; struct ActionAdapter; class EventAdapter; } }


namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT Viewer : public Usul::Base::Object,
                                public Usul::Interfaces::IViewMatrix,
                                public Usul::Interfaces::IShadeModel,
                                public Usul::Interfaces::IPolygonMode,
                                public Usul::Interfaces::IExport,
                                public Usul::Interfaces::IBoundingBox,
                                public Usul::Interfaces::IBoundingSphere,
                                public Usul::Interfaces::IOpenGLState,
                                public Usul::Interfaces::IOpenSceneGraph,
                                public Usul::Interfaces::ISelectionBox,
                                public Usul::Interfaces::IStereo,
                                public Usul::Interfaces::IFrameDump,
                                public Usul::Interfaces::ITextMatrix,
                                public Usul::Interfaces::IGetDocument,
                                public Usul::Interfaces::IGroup,
                                public Usul::Interfaces::IClippingPlanes,
                                public Usul::Interfaces::IViewer,
                                public Usul::Interfaces::IGetBoundingBox,
                                public Usul::Interfaces::IAxes,
                                public Usul::Interfaces::ICamera,
                                public Usul::Interfaces::ITrackball,
                                public Usul::Interfaces::ISceneIntersect,
                                public Usul::Interfaces::IRedraw,
                                public Usul::Interfaces::IMode,
                                public Usul::Interfaces::ISetTool,
                                public Usul::Interfaces::ISpin,
                                public Usul::Interfaces::IBackground,
                                public Usul::Interfaces::IHeliosView,
                                public Usul::Interfaces::ILights,
                                public Usul::Interfaces::ICenterOfRotation,
                                public Usul::Interfaces::IScreenCapture,
                                public Usul::Interfaces::ISnapShot,
                                public Usul::Interfaces::IView,
                                public Usul::Interfaces::IRenderNotify,
                                public Usul::Interfaces::IFrameStamp,
                                public Usul::Interfaces::IUpdateSceneVisitor,
                                public Usul::Interfaces::ICullSceneVisitor,
                                public Usul::Interfaces::IUpdateSubject
{
public:

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef osgUtil::SceneView SceneView;
  typedef osg::Node Node;
  typedef osg::Group Group;
  typedef osg::Viewport Viewport;
  typedef osg::PolygonMode PolygonMode;
  typedef osg::ShadeModel ShadeModel;
  typedef osg::TexEnv TexEnv;
  typedef Usul::Interfaces::IDocument Document;
  typedef std::pair<std::string,std::string> Filter;
  typedef std::vector<Filter> Filters;
  typedef Usul::Interfaces::ICamera::CameraOption CameraOption;
  typedef Usul::Interfaces::IShadeModel IShadeModel;
  typedef Usul::Interfaces::IPolygonMode IPolygonMode;
  typedef osgGA::MatrixManipulator MatrixManip;
  typedef osg::ref_ptr<MatrixManip> MatrixManipPtr;
  typedef OsgTools::Draggers::Dragger Dragger;
  typedef Usul::Interfaces::ITool ITool;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IOpenGLContext IContext;
  typedef Usul::Interfaces::ISetCursorType ISetCursorType;
  typedef Usul::Interfaces::ITimeoutSpin ITimeoutSpin;
  typedef FrameDump::Names Filenames;
  typedef OsgTools::Builders::GradientBackground GradientBackground;
  typedef GradientBackground::Corners Corners;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef Usul::Interfaces::IRenderListener IRenderListener;
  typedef std::vector<IRenderListener::RefPtr> RenderListeners;
  typedef osgUtil::CullVisitor CullVisitor;
  typedef osg::NodeVisitor NodeVisitor;
  typedef Usul::Interfaces::IUpdateListener IUpdateListener;
  typedef std::vector<IUpdateListener::RefPtr> UpdateListeners;

  enum ViewMode
  {
    NAVIGATION,
    PICK,
    SEEK
  };

  // Construction/destruction.
  Viewer ( Document *doc, IUnknown* context, IUnknown *caller );
  virtual ~Viewer();

  // Add a chore.
  void                  addChoreRedraw();

  // Edit the background color.
  void                  editBackground();

  // Edit the background color.
  void                  setBackground ( const osg::Vec4 & );

  // Get/Set show axes state.
  void                  axes ( bool );
  bool                  axes() const;

  // Set/get the background color. Throws if getting color from a null viewer.
  void                  backgroundColor ( const osg::Vec4 &color );
  osg::Vec4             backgroundColor() const;

  // Set/get the background corners.
  void                  backgroundCorners ( unsigned int corners );
  unsigned int          backgroundCorners() const;

  // Set/get the bounding-box state.
  void                  boundingBox ( bool show );
  bool                  boundingBox() const;

  // Set/get the bounding-sphere state.
  void                  boundingSphere ( bool show );
  bool                  boundingSphere() const;

  // Button Press/Release
  void                  buttonPress      ( float x, float y, bool left, bool middle, bool right );
  void                  buttonRelease    ( float x, float y, bool left, bool middle, bool right );

  // Set the camera.
  virtual void          camera ( CameraOption option );

  // Tell the canvas that the scene changed.
  virtual void          changedScene();

  // Copy/paste the camera.
  virtual void          copyCamera() const;
  virtual void          pasteCamera();
  bool                  canPasteCamera() const;

  /// Set the context.
  void                  context ( Usul::Interfaces::IUnknown* context );

  /// Set compute near far mode.
  void                  computeNearFar( bool b );

  // Create.
  void                  create();

  // Clear
  void                  clear();

  /// Get/Set the database pager.
  void                          databasePager( osgDB::DatabasePager* dbPager );
  osgDB::DatabasePager*         databasePager();
  const osgDB::DatabasePager*   databasePager() const;

  // Set the default background.
  void                  defaultBackground();

  // Get/Set the document.
  Document *            document();
  const Document *      document() const;
  void                  document ( Document * );

  // Force all detail to render.
  void                  forceDetail();

  // Get/Set the field of view
  void                  fovSet ( double fov );
  double                fovGet () const;

  // Set/get the frame-dump data.
  void                  frameDump ( const FrameDump &fd ) { _frameDump = fd; }
  const FrameDump &     frameDump() const { return _frameDump; }
  FrameDump &           frameDump()       { return _frameDump; }

  // Handle different modes.
  void                  handleDragging   ( float x, float y, Dragger::Command command );
  void                  handleMenu       ( float x, float y );
  void                  handleNavigation ( float x, float y, bool left, bool middle, bool right, EventAdapter::EventType type );
  virtual void          handlePicking    ( float x, float y, bool left, unsigned int numClicks );
  void                  handleSeek       ( float x, float y, bool left );
  void                  handleTool       ( bool left, bool middle, bool right, bool motion, float x, float y, float z );

  // Is there an accumulation buffer?
  bool                  hasAccumBuffer() const;

  // Is it created?
  bool                  isCreated() const;

  // Hide/Show lights in the scene.
  bool                  lights() const;
  void                  lights( bool );

  // Set/get the navigation manipulator.
  const MatrixManip *   navManip() const;
  MatrixManip *         navManip();
  void                  navManip ( MatrixManip * );

  // Set/get the number of rendering passes. Unavailable numbers have no effect.
  void                  numRenderPasses ( unsigned int );
  unsigned int          numRenderPasses() const;

  // Get the model
  const osg::Node *     model() const;
  osg::Node *           model();

  // Get the mode
  bool                  navigating() const { return NAVIGATION == _currentMode; }
  bool                  picking()    const { return PICK       == _currentMode; }
  bool                  tool()       const { return _currentTool.valid(); }

  void                  navigating( bool b ) { this->setMode( NAVIGATION ); }
  void                  picking   ( bool b ) { this->setMode( PICK       ); }

  // Set/get the mode
  void                  setMode ( ViewMode mode );
  ViewMode              getMode () { return _currentMode; }

  // Cycle through the modes
  void                  cycleMode();
  
  // Set/query/remove the polygon mode.
  void                  setPolygonMode    ( PolygonMode::Face face, osg::PolygonMode::Mode mode );
  void                  togglePolygonMode ( PolygonMode::Face face, osg::PolygonMode::Mode mode );
  bool                  hasPolygonMode    ( PolygonMode::Face face, osg::PolygonMode::Mode mode ) const;
  bool                  hasPolygonMode    ( PolygonMode::Face face ) const;
  bool                  hasPolygonMode() const;
  void                  removePolygonMode();

  // Set/query/remove hidden lines.
  void                  setHiddenLines();
  void                  toggleHiddenLines();
  bool                  hasHiddenLines() const;
  void                  removeHiddenLines();

  // Set/query/remove the shading model.
  void                  setShadeModel    ( ShadeModel::Mode mode );
  void                  toggleShadeModel ( ShadeModel::Mode mode );
  bool                  hasShadeModel    ( ShadeModel::Mode mode ) const;
  bool                  hasShadeModel() const;
  void                  removeShadeModel();

  // Set/query/remove the texture environment.
  void                  setTextureEnvironment ( TexEnv::Mode mode );
  bool                  hasTextureEnvironment ( TexEnv::Mode mode ) const;
  bool                  hasTextureEnvironment() const;
  void                  removeTextureEnvironment();

  // Set/get the texture mode.
  bool                  textureMode ( osg::StateAttribute::GLMode mode ) const;
  void                  textureMode ( osg::StateAttribute::GLMode mode, bool state );

  // Add/Remove clipping plane
  void                  addPlane ( );
  osg::ClipPlane*       addPlane ( const osg::Plane&, bool widget = true );
  void                  addClipBox  ( const osg::BoundingBox& bb );
  void                  removePlane ( unsigned int );
  void                  removePlane ( osg::ClipPlane * );
  void                  removePlanes();

  // Set all the display-lists to on/off.
  bool                  useDisplayLists() const;
  void                  useDisplayLists ( bool state );
  void                  updateDisplayListUse();

  // Get the number of clipping planes in the scene
  unsigned int          planes() const;

  /// Usul::Interfaces::IPolygonMode
  // Get/Set the polygon mode.
  virtual void                    polygonMode ( IPolygonMode::Mode mode );
  virtual IPolygonMode::Mode      polygonMode() const;

  // Render the scene.
  void                  render();

  // Resize the viewer
  void                  resize ( unsigned int width, unsigned int height );

  // Rotate the scene.
  virtual void          rotate ( const osg::Vec3 &axis, float degrees );

  // Set all the material's alpha value.
  void                  setAllMaterialsAlpha( float alpha );

  // Set the status bar text.  TODO Is this needed here?
  void                  setStatusBarText ( const std::string &text, bool force );

  // Set/get the scene.
  virtual void          scene ( Node * );
  virtual const Node *  scene() const;
  virtual Node *        scene();

  /// Get the scene manager
  SceneManager *        sceneManager() { return _sceneManager.get(); }

  // Get/Set back to front sorting.
  bool                  sortBackToFront () const;
  void                  sortBackToFront ( bool );

  // Get/set stereo
  virtual bool          stereo() const;
  virtual void          stereo ( bool );

  // Start/stop the spin.
  void                  spin ( bool );

  ///  Usul::Interfaces::ITextMatrix
  /// Get text at the (x,y) on the screen.
  osgText::Text*        getText     ( unsigned int x, unsigned int y );

  /// Set text value.
  void                  setText     ( unsigned int x, unsigned int y, const std::string& text );

  /// Remove text
  void                  removeText  ( unsigned int x, unsigned int y );

  // Get the time.
  double                timeAverage ( const std::string &name ) const;
  double                timeLast    ( const std::string &name ) const;

  // Spin
  void                  timeoutSpin ();

  // Animate
  bool                  timeoutAnimate ();

  // Set/Get two sided lighting
  void                  twoSidedLighting ( bool twoSided );
  bool                  twoSidedLighting () const;

  // Get the light.
  osg::Light *          light();
  const osg::Light *    light() const;

  // Set/Get lighting
  void                  lighting ( bool on );
  bool                  lighting () const;

  // Set/get the scatter scale.
  double                scatterScale() const;
  void                  scatterScale ( double );

  // Update the cursor based on the internal mode.
  void                  updateCursor();

  // Update the scene.
  void                  update();

  // Update the status-bar.
  void                  updateStatusBar();

  // Set/get the flags that says to update the recorded times.
  void                  updateTimes ( bool state );
  bool                  updateTimes() const;

  /// Set/Get the view matrix
  void                  viewMatrix ( const osg::Matrixf& );
  void                  viewMatrix ( const osg::Matrixd& );
  const osg::Matrixd&   viewMatrix ( ) const;

  // Set/get the viewport.
  const Viewport *      viewport() const;
  Viewport *            viewport();
  void                  viewport ( Viewport *vp );
  void                  viewport ( int x, int y, unsigned int w, unsigned int h );

  // Set/get the viewer.
  const SceneView *     viewer() const;
  SceneView *           viewer();
  void                  viewer ( SceneView * );

  // Write the current frame to an image file.
  bool                  writeImageFile ( const std::string &filename ) const;
  virtual bool          writeImageFile ( const std::string &filename, unsigned int width, unsigned int height ) const;

  // Write the current model or scene to file.
  bool                  writeModelFile ( const std::string &filename, const std::string &options = std::string() ) const;
  bool                  writeSceneFile ( const std::string &filename, const std::string &options = std::string() ) const;

  // Zoom by given amount
  void                  zoom ( double delta );

  // Helper class to save and restore the lod-callback.
  struct LowLods
  {
    LowLods ( Viewer *c );
    ~LowLods();
  private:
    Viewer *_c;
  };

  ///  Usul::Interfaces::ISetTool
  // Set and Release the tool
  virtual void setTool ( ITool * );
  virtual ITool* getTool ();
  virtual void doneTool ();
  virtual void loadLastTool ();

  /// Add/Remove group from projection node
  osg::Group*           projectionGroupGet    ( const std::string& );

protected:

  // Do not use.
  Viewer();

  void                  _showLights();
  void                  _removeLights();
  void                  _setLights();

  void                  _addAxes ();
  void                  _removeAxes ();
  void                  _setAxes (); 

  void                  _setCenterOfRotation();

  void                  _dumpFrame();

  void                  _editLight     ( osgUtil::Hit &hit );
  void                  _editMaterial  ( osgUtil::Hit &hit );

  void                  _findDragger ( const osgUtil::Hit &hit );

  void                  _hiddenLineRender();

  virtual  bool         _intersect ( float x, float y, osg::Node *scene, osgUtil::Hit &hit, bool useWindowCoords = false );
  bool                  _lineSegment ( float mouseX, float mouseY, osg::Vec3 &pt0, osg::Vec3 &pt1, bool useWindowCoords = false );

  void                  _render();

  void                  _setNavCursor ( bool left, bool middle, bool right, EventAdapter::EventType type );

  void                  _setDisplayListsGeode ( osg::Geode *geode );
  void                  _setLodCullCallback ( osg::NodeCallback *cb );

  // Add/Remove selection box from the scene.
  void                  _drawSelectionBox ( const osg::Vec3&, const osg::Vec3& );
  void                  _removeSelectionBox ();

  // Return the view all time.
  double                _animationTime();

  // Get the trackball.
  Trackball*            _trackball();

  // Write the current frame to an image file.
  bool                  _writeImageFile ( const std::string &filename ) const;
  bool                  _writeImageFile ( const std::string &filename, unsigned int width, unsigned int height ) const;

  // Notify of rendering.
  void                  _preRenderNotify();
  void                  _postRenderNotify();

  /// Usul::Interfaces::IViewMatrix
  virtual void                      setViewMatrix ( const osg::Matrixf& );
  virtual void                      setViewMatrix ( const osg::Matrixd& );
  virtual const osg::Matrixd&       getViewMatrix ( ) const;

  /// Usul::Interfaces::IShadeModel
  virtual void                      shadeModel ( IShadeModel::Mode mode );
  virtual IShadeModel::Mode         shadeModel() const;

  /// Usul::Interfaces::IAxes
  virtual void                      setAxes ( bool );
  virtual bool                      hasAxes ( ) const;

  /// Usul::Interfaces::IOpenSceneGraph
  /// Get the pointer to the base class for all OSG objects.
  virtual const osg::Referenced *   osgReferenced() const;
  virtual osg::Referenced *         osgReferenced();

  ///  Usul::Interfaces::ISelectionBox
  virtual void               drawSelectionBox ( const osg::Vec3&, const osg::Vec3& );
  virtual void               removeSelectionBox();

  /// Usul::Interfaces::IStereo
  // Get/set the stereo mode
  virtual void               stereoMode ( unsigned int );
  virtual unsigned int       stereoMode() const;

  //Get/Set the eye distance
  virtual void               stereoEyeDistance ( float );
  virtual float              stereoEyeDistance() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFrameDump
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Set properties for dumping frames.
  virtual void               frameDumpProperties ( const std::string &dir, 
                                                   const std::string &base, 
                                                   const std::string &ext, 
                                                   unsigned int start = 0, 
                                                   unsigned int digits = 10 );
  
  /// Turn on/off frame-dumping.
  virtual void               dumpFrames ( bool );

  /// Are we dumping frames?
  virtual bool               dumpFrames() const;

  /// Reset the file name counter.
  virtual void               resetFrameDumpCounter();

  /// Get the current file number
  virtual unsigned int       currentFile() const;

  /// Should the filenames that have been writed be saved?
  virtual void               saveNames ( bool );
  virtual bool               saveNames() const;

  /// Get the filenames that were written out.
  virtual Filenames          filenames() const;

  ///  Usul::Interfaces::IGetDocument
  virtual IUnknown*          getDocument();

  ///  Usul::Interfaces::IGroup
  virtual osg::Group*        getGroup    ( const std::string& );
  virtual void               removeGroup ( const std::string& );
  virtual bool               hasGroup    ( const std::string& );

  /// Usul::Interfaces::IClippingPlanes
  // Add/Remove clipping plane
  virtual void               addClippingPlane ( );
  virtual void               addClippingPlane ( const osg::Plane& plane );
  virtual void               addClippingBox   ( const osg::BoundingBox& bb );
  virtual void               removeClippingPlane ( unsigned int index );
  virtual void               removeClippingPlanes();

  // Get the number of clipping planes in the scene
  virtual unsigned int       numClippingPlanes() { return this->planes(); }

  /// Usul::Interfaces::IViewer
  virtual void               clearScene();
  virtual int                x() const;
  virtual int                y() const;
  virtual unsigned int       height() const;
  virtual unsigned int       width() const;
  virtual void               handleMessage ( unsigned short message );

  /// Usul::Interfaces::IGetBoundingBox
  virtual osg::BoundingBox   getBoundingBox() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ITrackball
  //
  /////////////////////////////////////////////////////////////////////////////
  
  virtual osg::Vec3          getCenter();
  virtual void               setCenter( const osg::Vec3& );
  virtual float              getDistance();
  virtual void               setDistance ( float );
  virtual osg::Quat          getRotation();
  virtual void               setRotation( const osg::Quat& );
  virtual void               setTrackball ( const osg::Vec3&, float distance, const osg::Quat&, bool makeTrackball, bool setViewerToo );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ISceneIntersect
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual bool               intersect ( float x, float y, osgUtil::Hit &hit );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IRedraw
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void                  redraw();
  virtual void                  setStatsDisplay ( bool b );

  /// Usul::Interfaces::IHeliosView
  virtual OsgTools::Render::Viewer*            heliosView()       { return this; }
  virtual const OsgTools::Render::Viewer*      heliosView() const { return this; }

  /// Usul::Interfaces::ILights
  virtual void                  setLights ( bool );
  virtual bool                  hasLights ( ) const;

  /// Usul::Interfaces::IExport
  virtual bool                  canExport ( const std::string &filename );
  virtual Filters               filtersExport() const;
  virtual Filters               filtersWriteImage() const;
  virtual bool                  exportFile ( const std::string& filename );

  /// Usul::Interfaces::ICenterOrRotation
  virtual void                  showCenterOfRotation ( bool b );
  virtual bool                  showCenterOfRotation ( ) const;

  /// Usul::Interfaces::IScreenCapture
  virtual osg::Image*           screenCapture ( const osg::Vec3f& center, float distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const;
  virtual osg::Image*           screenCapture ( unsigned int height, unsigned int width ) const;

  /// Usul::Interfaces::ISnapShot
  virtual void                  takePicture ( const std::string& filename, float frameSizeScale, unsigned int numSamples );

  // Add the listener (IRenderListener).
  virtual void                  addRenderListener ( IUnknown *listener );

  // Remove all render listeners.
  virtual void                  clearRenderListeners();

  // Remove the listener (IRenderListener).
  virtual void                  removeRenderListener ( IUnknown *caller );

  // Usul::Interfaces::IFrameStamp
  const osg::FrameStamp *       frameStamp() const;
  osg::FrameStamp *             frameStamp();

  // Usul::Interfaces::ICullSceneVisitor.
  virtual CullVisitor *         getCullSceneVisitor ( Usul::Interfaces::IUnknown *caller );

  // Usul::Interfaces::IUpdateSceneVisitor.
  virtual NodeVisitor *         getUpdateSceneVisitor ( Usul::Interfaces::IUnknown *caller );

  /// Add an update listener ( IUpdateListener ).
  virtual void                  addUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Remove an update listener ( IUpdateListener ).
  virtual void                  removeUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Clear update listeners.
  void                          _clearUpdateListeners ();
private:

  // Do not copy.
  Viewer ( const Viewer & );
  Viewer &operator = ( const Viewer & );

  // Possible flags.
  enum
  {
    _UPDATE_TIMES       = 0x00000001,
    _HIDDEN_LINES       = 0x00000004,
    _SORT_BACK_TO_FRONT = 0x00000008,
    _SHOW_AXES          = 0x00000010,
    _SHOW_LIGHTS        = 0x00000020,
    _SHOW_COR           = 0x00000080
  };

  // Local integer constants.
  enum
  {
    _ANIMATION_TIMER_MILLISECONDS    = 15, // 15ms will give about 60FPS for OpenGL rendering.
    _ANIMATION_DURATION_MILLISECONDS = 500 // Half a second.
  };

  //typedef Usul::Threads::Mutex Mutex;
  //typedef Usul::Threads::Guard<Mutex> Guard;
  typedef osg::ref_ptr < osg::LOD > LodPtr;
  typedef osg::ref_ptr < osg::Geode > GeodePtr;
  typedef osg::ref_ptr < Node > NodePtr;
  typedef osg::ref_ptr < Group > GroupPtr;
  typedef osg::ref_ptr < osgText::Text > TextPtr;
  typedef std::pair < bool, osg::Matrixd > CameraBuffer;
  typedef Document::RefPtr DocumentPtr;
  typedef std::map < osg::Geode *, osg::Light * > LightEditors;
  typedef std::vector < LodPtr > LodList;
  typedef std::pair < bool, LodList > Lods;
  typedef std::pair < bool, bool > UseDisplayLists;

  static CameraBuffer _cameraCopyBuffer;
  static MatrixManipPtr _navManipCopyBuffer;
  IContext::QueryPtr _context;
  Renderer::ValidRefPtr _renderer;
  SceneManager::ValidRefPtr _sceneManager;
  ISetCursorType::QueryPtr _setCursor;
  ITimeoutSpin::QueryPtr _timeoutSpin;
  IUnknown::QueryPtr _caller;
  Lods _lods;
  DocumentPtr _document;
  FrameDump _frameDump;
  unsigned int _refCount;
  unsigned int _flags;
  OsgTools::Render::Animation _animation;
  MatrixManipPtr _navManip;
  ITool::QueryPtr _currentTool;
  ITool::QueryPtr _lastTool;
  ViewMode _currentMode;
  LightEditors _lightEditors;
  unsigned int _contextId;
  GradientBackground _gradient;
  unsigned int _corners;
  UseDisplayLists _useDisplayList;
  osg::ref_ptr < osgDB::DatabasePager > _databasePager;
  RenderListeners _renderListeners;
  UpdateListeners _updateListeners;
};

}
}

#endif // __OSG_TOOLS_RENDER_VIEWER_H__
