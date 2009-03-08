
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
#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IAnimationNotify.h"
#include "Usul/Interfaces/IAxes.h"
#include "Usul/Interfaces/ICamera.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/IClippingPlanes.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IExportImage.h"
#include "Usul/Interfaces/IExportScene.h"
#include "Usul/Interfaces/IExportModel.h"
#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/IIntersectNotify.h"
#include "Usul/Interfaces/IModelsScene.h"
#include "Usul/Interfaces/IMouseEventListener.h"
#include "Usul/Interfaces/IMouseEventSubject.h"
#include "Usul/Interfaces/IOpenGLContext.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IProjectionMatrix.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IRenderInfoOSG.h"
#include "Usul/Interfaces/IRenderListener.h"
#include "Usul/Interfaces/IRenderNotify.h"
#include "Usul/Interfaces/IRenderLoop.h"
#include "Usul/Interfaces/IRenderingPasses.h"
#include "Usul/Interfaces/IScreenCapture.h"
#include "Usul/Interfaces/ISnapShot.h"
#include "Usul/Interfaces/ISpin.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/ITimeoutAnimate.h"
#include "Usul/Interfaces/ITimeoutSpin.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IViewMode.h"

#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Render/Animation.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"
#include "OsgTools/Widgets/ClipPlane.h"

#include "osgUtil/SceneView"
#include "osgUtil/LineSegmentIntersector"

#include "osg/ref_ptr"
#include "osg/RenderInfo"
#include "osg/ShadeModel"
#include "osg/PolygonMode"
#include "osg/TexEnv"
#include "osg/Geode"

#include "osgText/Text"

#include "osgGA/MatrixManipulator"

#include "osgManipulator/Dragger"

#include <list>
#include <string>
#include <vector>

namespace osg { class ClipPlane; class NodeVisitor; }
namespace osgUtil { class Hit; class CullVisitor; }
namespace OsgTools { namespace Render { class Trackball; struct ActionAdapter; class EventAdapter; } }


namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT Viewer : public Usul::Base::Object,
                                public Usul::Interfaces::IAxes,
                                public Usul::Interfaces::IViewMatrix,
                                public Usul::Interfaces::IShadeModel,
                                public Usul::Interfaces::IPolygonMode,
                                public Usul::Interfaces::IExportImage,
                                public Usul::Interfaces::IExportScene,
                                public Usul::Interfaces::IExportModel,
                                public Usul::Interfaces::IOpenSceneGraph,
                                public Usul::Interfaces::IFrameDump,
                                public Usul::Interfaces::ITextMatrix,
                                public Usul::Interfaces::IGetDocument,
                                public Usul::Interfaces::IGroup,
                                public Usul::Interfaces::IClippingPlanes,
                                public Usul::Interfaces::IGetBoundingBox,
                                public Usul::Interfaces::ICamera,
                                public Usul::Interfaces::ITrackball,
                                public Usul::Interfaces::ISceneIntersect,
                                public Usul::Interfaces::IRedraw,
                                public Usul::Interfaces::ISpin,
                                public Usul::Interfaces::IScreenCapture,
                                public Usul::Interfaces::ISnapShot,
                                public Usul::Interfaces::IView,
                                public Usul::Interfaces::IRenderNotify,
                                public Usul::Interfaces::IIntersectNotify,
                                public Usul::Interfaces::IFrameStamp,
                                public Usul::Interfaces::IUpdateSceneVisitor,
                                public Usul::Interfaces::ICullSceneVisitor,
                                public Usul::Interfaces::IUpdateSubject,
                                public Usul::Interfaces::IMouseEventSubject,
                                public Usul::Interfaces::IClippingDistance,
                                public Usul::Interfaces::IViewport,
                                public Usul::Interfaces::IRenderLoop,
                                public Usul::Interfaces::IRenderingPasses,
                                public Usul::Interfaces::IViewMode,
																public Usul::Interfaces::IModelsScene,
                                public Usul::Interfaces::IRenderInfoOSG,
                                public Usul::Interfaces::IProjectionMatrix
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
  typedef osgManipulator::Dragger Dragger;
  typedef osg::ref_ptr < Dragger > DraggerPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IOpenGLContext IContext;
  typedef Usul::Interfaces::ITimeoutSpin ITimeoutSpin;
  typedef FrameDump::Names Filenames;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef Usul::Interfaces::IRenderListener IRenderListener;
  typedef Usul::Interfaces::IIntersectListener IIntersectListener;
  typedef std::vector<IRenderListener::RefPtr> RenderListeners;
  typedef std::vector<IIntersectListener::RefPtr> IntersectListeners;
  typedef osgUtil::CullVisitor CullVisitor;
  typedef osg::NodeVisitor NodeVisitor;
  typedef Usul::Interfaces::IUpdateListener IUpdateListener;
  typedef std::vector<IUpdateListener::RefPtr> UpdateListeners;
  typedef Usul::Interfaces::IMouseEventListener IMouseEventListener;
  typedef std::vector<IMouseEventListener::RefPtr> MouseEventListeners;
  typedef Renderer::GradientBackground GradientBackground;
  typedef Renderer::Corners            Corners;
  typedef Usul::Interfaces::IViewMode IViewMode;
  typedef IViewMode::ViewMode ViewMode;
  typedef osg::RenderInfo RenderInfo;
  typedef Usul::Interfaces::IFrameDump IFrameDump;

  // Construction
  Viewer ( Document *doc, IUnknown* context, IUnknown *caller );

  // Edit the background color.
  void                  setBackground ( const osg::Vec4 & );

  // Get/Set show axes state (IAxes).
  void                  axesShown ( bool );
  bool                  isAxesShown() const;

  /// Set the axes label (IAxes).
  virtual void          axesLabels ( const std::string& x, const std::string& y, const std::string& z );
  
  /// Set/get back face culling.
  void                  showBackFaces ( bool b );
  bool                  isBackFacesShowing() const;

  // Set/get the background color. Throws if getting color from a null viewer.
  void                  backgroundColor ( const osg::Vec4 &color, unsigned int corners );
  void                  backgroundColor ( const osg::Vec4 &color );
  osg::Vec4             backgroundColor ( unsigned int corners ) const;
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
  void                  buttonPress      ( EventAdapter *ea );
  void                  buttonRelease    ( EventAdapter *ea );

  /// Get the caller.
  IUnknown*             caller();
  const IUnknown *      caller() const;

  // Set the camera.
  virtual void          camera ( CameraOption option );

  /// Get/set the clipping distances (Usul::Interfaces::IClippingDistance).
  virtual void          getClippingDistances ( double &nearDist, double &farDist ) const;
  virtual void          setClippingDistances ( double nearDist, double farDist );
  
  // Tell the canvas that the scene changed.
  virtual void          changedScene();

  /// Hide/Show the center of rotation.
  void                  showCenterOfRotation ( bool b );
  bool                  showCenterOfRotation() const;

  /// Set the context.
  void                  context ( Usul::Interfaces::IUnknown* context );

  /// Set/get compute near far mode.
  void                  computeNearFarSet ( bool b );
  bool                  computeNearFarGet() const;

  // Create.
  void                  create();

  // Clear
  void                  clear();

  // Set the default background.
  void                  defaultBackground();

  // Get/Set the document.
  Document *            document();
  const Document *      document() const;
  void                  document ( Document * );

  EventAdapter *        eventAdaptor ( float x, float y, bool left, bool middle, bool right, EventAdapter::EventType type );

  // Set/get the frame-dump data.
  void                  frameDump ( const FrameDump &fd ) { _frameDump = fd; }
  const FrameDump &     frameDump() const { return _frameDump; }
  FrameDump &           frameDump()       { return _frameDump; }

  // Is there an accumulation buffer?
  bool                  hasAccumBuffer() const;

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

  // The mouse have moved.
  void                  mouseMove ( EventAdapter *ea );

  // Get the mode
  bool                  navigating() const { return NAVIGATION == _currentMode; }
  bool                  picking()    const { return PICK       == _currentMode; }

  void                  navigating( bool b ) { this->setViewMode( NAVIGATION ); }
  void                  picking   ( bool b ) { this->setViewMode( PICK       ); }

  // Get the render info for this Viewer.
  virtual RenderInfo    getRenderInfo() const;

  // Set/get the mode
  void                  setViewMode ( ViewMode mode );
  ViewMode              getViewMode () const;
  bool                  isViewModeCurrent ( ViewMode mode ) const;

  // Save/load the state.
  void                  stateLoad();
  void                  stateSave() const;

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
  osg::ClipPlane*       addPlane();
  osg::ClipPlane*       addPlane ( const osg::Plane&, bool widget = true );
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

  // Set the trackball state (ITrackball).
  virtual void          setTrackball ( const osg::Vec3d&, double distance, const osg::Quat&, bool makeTrackball, bool setViewerToo );
  
  // Set/get the scene.
  virtual void          scene ( Node * );
  virtual const Node *  scene() const;
  virtual Node *        scene();

  /// Get the scene manager
	const SceneManager *  sceneManager() const;
  SceneManager *        sceneManager();

  // Get/Set back to front sorting.
  bool                  sortBackToFront() const;
  void                  sortBackToFront ( bool );

  // Get/set stereo
  virtual bool          stereo() const;
  virtual void          stereo ( bool );

  // Start/stop the spin.
  void                  spin ( bool );

  /// Usul::Interfaces::ITextMatrix
  osgText::Text*        getText     ( unsigned int x, unsigned int y );
  void                  setText     ( unsigned int x, unsigned int y, const std::string& text, const osg::Vec4f& color, const osg::Vec4f& backDropColor = osg::Vec4f ( 0, 0, 0, 0 ) );
  void                  removeText  ( unsigned int x, unsigned int y );

  // Get/set show-text state.
  void                  textShown ( bool );
  bool                  isTextShown() const;

  // Get the time.
  double                timeAverage ( const std::string &name ) const;
  double                timeLast    ( const std::string &name ) const;

  // Spin
  void                  timeoutSpin ();

  // Animate
  bool                  timeoutAnimate ();

  // Set/Get two sided lighting
  void                  twoSidedLightingSet ( bool twoSided );
  bool                  twoSidedLightingGet () const;

  // Get the light.
  osg::Light *          light();
  const osg::Light *    light() const;

  // Set/Get lighting
  void                  lighting ( bool on );
  bool                  lighting () const;

  // Set/get the scatter scale.
  double                scatterScale() const;
  void                  scatterScale ( double );

  // Get/set the stereo mode
  void                  stereoMode ( unsigned int );
  unsigned int          stereoMode() const;

  //Get/Set the eye distance
  void                  stereoEyeDistance ( float );
  float                 stereoEyeDistance() const;

  // Update the scene.
  void                  update();

  // Update the status-bar.
  void                  updateStatusBar();

  // Set/get the flags that says to update the recorded times.
  void                  updateTimes ( bool state );
  bool                  updateTimes() const;
  
  /// Set/get the use low lods state.
  void                  useLowLodsSet ( bool b );
  bool                  useLowLodsGet() const;
  
  /// Set/get the use high lods state.
  void                  useHighLodsSet ( bool b );
  bool                  useHighLodsGet() const;
  
  /// Set/get the use multisamle state.
  void                  useMultisampleSet ( bool b );
  bool                  useMultisampleGet() const;

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
  virtual bool          writeImageFile ( const std::string &filename ) const;

  // Write the current model to file (IExportModel).
  virtual bool          writeModelFile ( const std::string &filename, const std::string &options = std::string() ) const;

  // Write the current scene to file (IExportScene).
  virtual bool          writeSceneFile ( const std::string &filename, const std::string &options = std::string() ) const;

  // Zoom by given amount
  void                  zoom ( double delta );

  // Helper class to save and restore the lod-callback.
  struct LowLods
  {
    LowLods ( Viewer &c );
    ~LowLods();
  private:
    Viewer &_c;
  };

  /// Add/Remove group from projection node
  osg::Group*           projectionGroupGet    ( const std::string& );

protected:

  // Use reference counting.
  virtual ~Viewer();

  void                  _addAxes ();
  void                  _removeAxes ();
  void                  _setAxes (); 

  void                  _dumpFrame();

  void                  _editMaterial  ( osgUtil::LineSegmentIntersector::Intersection &hit );

  void                  _findDragger ( const osgUtil::LineSegmentIntersector::Intersection &hit );

  void                  _hiddenLineRender();

  bool                  _intersect ( float x, float y, osg::Node *scene, osgUtil::LineSegmentIntersector::Intersection &hit, bool useWindowCoords = false );
  bool                  _intersect ( float x, float y, osg::Node *scene, osgUtil::LineSegmentIntersector::Intersections &intersections, bool useWindowCoords = false );
  void                  _intersectNotify ( float x, float y, osgUtil::LineSegmentIntersector::Intersection &hit );
  bool                  _lineSegment ( float mouseX, float mouseY, osg::Vec3d &pt0, osg::Vec3d &pt1, bool useWindowCoords = false );

  void                  _render();

  void                  _setCenterOfRotation();

  void                  _setNavCursor ( bool left, bool middle, bool right, EventAdapter::EventType type );

  void                  _setDisplayListsGeode ( osg::Geode *geode );
  void                  _setLodCullCallback ( osg::NodeCallback *cb );

  // Get the trackball.
  Trackball*            _trackball();

  // Write the current frame to an image file.
  bool                  _writeImageFile ( const std::string &filename ) const;
  bool                  _writeImageFile ( const std::string &filename, unsigned int width, unsigned int height ) const;

  // Notify of rendering.
  void                  _preRenderNotify();
  void                  _postRenderNotify();

  // Handle different modes.
  void                  _handleDragging   ( EventAdapter *ea );
  void                  _handleIntersect  ( EventAdapter *ea );
  void                  _handleNavigation ( EventAdapter *ea );
  void                  _handlePicking    ( EventAdapter *ea );
  void                  _handleSeek       ( EventAdapter *ea );

  // Notify of mouse event.
  void                  _mouseEventNotify ( EventAdapter *ea );

  // Set the LOD state.
  void                  _useLowLodsSet ( bool b );
  void                  _useHighLodsSet ( bool b );

  /// Usul::Interfaces::IViewMatrix
  virtual void                      setViewMatrix ( const osg::Matrixd& );
  virtual osg::Matrixd              getViewMatrix ( ) const;

  /// Usul::Interfaces::IShadeModel
  virtual void                      shadeModel ( IShadeModel::Mode mode );
  virtual IShadeModel::Mode         shadeModel() const;

  /// Usul::Interfaces::IOpenSceneGraph
  /// Get the pointer to the base class for all OSG objects.
  virtual const osg::Referenced *   osgReferenced() const;
  virtual osg::Referenced *         osgReferenced();

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
  
  /// Set frame-dump state.
  virtual void               setFrameDumpState ( IFrameDump::DumpState );

  /// Get frame-dump state
  virtual IFrameDump::DumpState getFrameDumpState() const;

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

  // Add/Remove clipping plane (IClippingPlanes).
  virtual unsigned int       addClippingPlane();
  virtual unsigned int       addClippingPlane ( const osg::Plane& plane );
  virtual void               removeClippingPlane ( unsigned int index );
  virtual void               removeClippingPlanes();

  // Get/Set the clipping plane (IClippingPlanes).
  virtual osg::Plane         getClippingPlane ( unsigned int );
  virtual void               setClippingPlane ( unsigned int, const osg::Plane& );

  // Get the number of clipping planes in the scene (IClippingPlanes).
  virtual unsigned int       numClippingPlanes() { return this->planes(); }

  /// Get the view port parameters (IViewport).
  virtual double             x() const;
  virtual double             y() const;
  virtual double             height() const;
  virtual double             width() const;

  /// Usul::Interfaces::IGetBoundingBox
  virtual osg::BoundingBox   getBoundingBox() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ITrackball
  //
  /////////////////////////////////////////////////////////////////////////////
  
  virtual osg::Vec3d         getCenter();
  virtual void               setCenter( const osg::Vec3d& );
  virtual double             getDistance();
  virtual void               setDistance ( double );
  virtual osg::Quat          getRotation();
  virtual void               setRotation( const osg::Quat& );

  // Intersect with the scene (ISceneIntersect).
  virtual bool               intersect ( float x, float y, osgUtil::LineSegmentIntersector::Intersection &  );
  virtual bool               intersect ( float x, float y, osgUtil::LineSegmentIntersector::Intersections & );
  virtual bool               intersect ( double x, double y, osgUtil::PolytopeIntersector::Intersection &,  double width = 0.05, double height = 0.05, unsigned int dimensionMask = osgUtil::PolytopeIntersector::AllDims );
  virtual bool               intersect ( double x, double y, osgUtil::PolytopeIntersector::Intersections &, double width = 0.05, double height = 0.05, unsigned int dimensionMask = osgUtil::PolytopeIntersector::AllDims );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IRedraw
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void                  redraw();
  virtual void                  setStatsDisplay ( bool b );

  /// Usul::Interfaces::IExportImage
  virtual Filters               filtersWriteImage() const;
  
  /// Usul::Interfaces::IExportModel
  virtual Filters               filtersWriteModel() const;

  /// Usul::Interfaces::IExportScene
  virtual Filters               filtersWriteScene() const;

  /// Usul::Interfaces::IScreenCapture
  virtual osg::Image*           screenCapture ( const osg::Vec3d& center, double distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const;
  virtual osg::Image*           screenCapture ( unsigned int height, unsigned int width ) const;

  /// Usul::Interfaces::ISnapShot
  virtual void                  takePicture ( const std::string& filename, float frameSizeScale, unsigned int numSamples );

  // Add the listeners.
  virtual void                  addRenderListener ( IUnknown *listener );
  virtual void                  addIntersectListener ( IUnknown *listener );
  virtual void                  addMouseEventListener ( IUnknown *listener );
  virtual void                  addUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Remove all listeners.
  virtual void                  clearRenderListeners();
  virtual void                  clearIntersectListeners();
  virtual void                  clearMouseEventListeners();
  void                          _clearUpdateListeners ();

  // Remove the listener.
  virtual void                  removeRenderListener ( IUnknown *caller );
  virtual void                  removeIntersectListener ( IUnknown *caller );
  virtual void                  removeMouseEventListener ( IUnknown *caller );
  virtual void                  removeUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Usul::Interfaces::IFrameStamp
  const osg::FrameStamp *       frameStamp() const;
  osg::FrameStamp *             frameStamp();

  // Usul::Interfaces::ICullSceneVisitor.
  virtual CullVisitor *         getCullSceneVisitor ( Usul::Interfaces::IUnknown *caller );

  // Usul::Interfaces::IUpdateSceneVisitor.
  virtual NodeVisitor *         getUpdateSceneVisitor ( Usul::Interfaces::IUnknown *caller );

  /// Get/Set render loop flag (IRenderLoop).
  virtual void                  renderLoop ( bool b );
  virtual bool                  renderLoop () const;

  /// Get/Set the number of rendering passes (IRenderingPasses).
  virtual void                  renderingPasses ( unsigned int number );
  virtual unsigned int          renderingPasses () const;

	// Get the model's scene (IModelsScene).
  virtual const osg::Group *    modelsScene() const;
  virtual osg::Group *          modelsScene();
  
  // Get the projection matrix (IProjectionMatrix).
  virtual osg::Matrixd          getProjectionMatrix() const;
  
private:

  // Do not use.
  Viewer();
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
    _SHOW_COR           = 0x00000080,
    _SHOW_TEXT          = 0x00000100,
    _USE_LOW_LODS       = 0x00000200,
    _USE_HIGH_LODS      = 0x00000400,
    _USE_MULTISAMPLE    = 0x00000800
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
  typedef Document::RefPtr DocumentPtr;
  typedef std::vector < LodPtr > LodList;
  typedef std::pair < bool, LodList > Lods;
  typedef std::pair < bool, bool > UseDisplayLists;
  typedef OsgTools::Widgets::ClipPlane ClipPlaneWidget;
  typedef std::map < unsigned int, ClipPlaneWidget::Ptr > ClipPlaneWidgets;

  IContext::QueryPtr _context;
  Renderer::ValidRefPtr _renderer;
  SceneManager::RefPtr _sceneManager;
  ITimeoutSpin::QueryPtr _timeoutSpin;
  IUnknown::QueryPtr _caller;
  Lods _lods;
  DocumentPtr _document;
  FrameDump _frameDump;
  unsigned int _flags;
  OsgTools::Render::Animation _animation;
  MatrixManipPtr _navManip;
  ViewMode _currentMode;
  unsigned int _contextId;
  UseDisplayLists _useDisplayList;
  RenderListeners _renderListeners;
  IntersectListeners _intersectListeners;
  UpdateListeners _updateListeners;
  MouseEventListeners _mouseEventListeners;
  DraggerPtr _activeDragger;
  osgManipulator::PointerInfo _pointerInfo;
  ClipPlaneWidgets _clipPlaneWidgets;
  osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback> _clampProjectionMatrixCallback;
};

}
}

#endif // __OSG_TOOLS_RENDER_VIEWER_H__
