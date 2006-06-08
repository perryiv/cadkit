
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_VIEWER_H__
#define __OSG_TOOLS_RENDER_VIEWER_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Variable.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Bits/Bits.h"

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
#include "Usul/Interfaces/ISceneStage.h"
#include "Usul/Interfaces/ICenterOfRotation.h"
#include "Usul/Interfaces/IScreenCapture.h"

#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Render/Animation.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"

#include "OsgTools/Draggers/Dragger.h"

#include "osgUtil/SceneView"

#include "osg/ref_ptr"
#include "osg/ShadeModel"
#include "osg/PolygonMode"
#include "osg/Geode"

#include "osgText/Text"

#include "osgGA/MatrixManipulator"

#include <list>
#include <string>

namespace osg { class ClipPlane; }

namespace osgUtil { class Hit; }

namespace OsgTools {
namespace Render {

class Trackball; 
struct ActionAdapter; 
class EventAdapter;

class OSG_TOOLS_EXPORT Viewer : public Usul::Base::Referenced,
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
                                public Usul::Interfaces::ISceneStage,
                                public Usul::Interfaces::ICenterOfRotation,
                                public Usul::Interfaces::IScreenCapture
{
public:

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef osgUtil::SceneView SceneView;
  typedef osg::Node Node;
  typedef osg::Group Group;
  typedef osg::Viewport Viewport;
  typedef osg::PolygonMode PolygonMode;
  typedef osg::ShadeModel ShadeModel;
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
  typedef Usul::Interfaces::ITimeoutSpin   ITimeoutSpin;

  enum ViewMode
  {
    NAVIGATION,
    PICK
  };

  // Construction/destruction.
  Viewer ( Document *doc, IContext* context, IUnknown *caller );
  virtual ~Viewer();

  // Add a chore.
  void                  addChoreRedraw();

  // Add a chore to reset the camera.
  // void                  addChoreCameraFit();
  // void                  addChoreCameraReset();

    // Edit the background color.
  void                  editBackground();

  // Edit the background color.
  void                  setBackground ( const osg::Vec4 &);

  // Get/Set show axes state.
  void                  axes( bool );
  bool                  axes() const;

  // Set/get the background color. Throws if getting color from a null viewer.
  void                  backgroundColor ( const osg::Vec4 &color );
  const osg::Vec4 &     backgroundColor() const;

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
  const osg::Node *     model () const;
  osg::Node *           model ();

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

  // Add/Remove clipping plane
  void                  addPlane ( );
  osg::ClipPlane*       addPlane ( const osg::Plane&, bool widget = true );
  void                  addClipBox  ( const osg::BoundingBox& bb );
  void                  removePlane ( unsigned int );
  void                  removePlane ( osg::ClipPlane * );
  void                  removePlanes ();

  // Set all the display-lists to on/off according to the global setting.
  bool                  displayLists() const;
  void                  setDisplayLists();

  // Get the number of clipping planes in the scene
  unsigned int          planes ();

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

  // Set text on canvas
  void                  text             ( float x, float y, unsigned int row, unsigned int col, const std::string& text );
  void                  textCreateMatrix ( float x, float y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth );
  void                  textRemoveMatrix ( float x, float y );

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

  // Set/Get lighting
  void                  lighting ( bool on );
  bool                  lighting () const;

  // Update the cursor based on the internal mode.
  void                  updateCursor();

  // Update the scene.
  void                  updateScene();

  // Update the status-bar.
  void                  updateStatusBar();

  // Set/get the flags that says to update the recorded times.
  void                  updateTimes ( bool state ) { _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _UPDATE_TIMES ); }
  bool                  updateTimes() const { return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _UPDATE_TIMES ); }

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
  bool                  writeImageFile ( const std::string &filename, const std::string &options = std::string() ) const;

  // Write the current scene to file.
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
  bool                  _writeImageFile ( const std::string &filename, double percent ) const;
  bool                  _writeImageFile ( const std::string &filename, unsigned int height, unsigned int width ) const;

  // Add/Remove scene stage.
  void                  _addSceneStage();
  void                  _removeSceneStage();

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

  ///  Usul::Interfaces::IOpenSceneGraph
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
  virtual void               filenamesSave ( bool );

  /// Get the filenames that were written out.
  virtual const Filenames&   filenames () const;

  ///  Usul::Interfaces::ITextMatrix
  virtual void               setText      ( float x, float y, unsigned int row, unsigned int col, const std::string& text );
  virtual void               createMatrix ( float x, float y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth );
  virtual void               removeMatrix ( float x, float y );

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
  virtual void               removeClippingPlanes (  );

  // Get the number of clipping planes in the scene
  virtual unsigned int       numClippingPlanes () { return this->planes(); }

  /// Usul::Interfaces::IViewer
  //virtual void            render(); //Render defined above.
  virtual void               clearScene();
  virtual int                x();
  virtual int                y();
  virtual int                height();
  virtual int                width();
  virtual void               handleMessage ( unsigned short message );

  int                        x() const;
  int                        y() const;
  int                        height() const;
  int                        width() const;

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

  virtual void               redraw();
  virtual void               setStatsDisplay ( bool b );

  /// Usul::Interfaces::IHeliosView
  virtual OsgTools::Render::Viewer*            HeliosView()       { return this; }
  virtual const OsgTools::Render::Viewer*      HeliosView() const { return this; }

  /// Usul::Interfaces::ILights
  virtual void               setLights ( bool );
  virtual bool               hasLights ( ) const;

  /// Usul::Interfaces::ISceneStage
  virtual bool sceneStage() const;
  virtual void sceneStage( bool b );

  /// Usul::Interfaces::IExport
  virtual bool                  canExport ( const std::string &filename );
  virtual Filters               filtersExport() const;
  virtual Filters               filtersWriteImage() const;
  virtual bool                  exportFile ( const std::string& filename );
  virtual bool                  writeImageFile ( const std::string &filename, unsigned int height, unsigned int width ) const;

  /// Usul::Interfaces::ICenterOrRotation
  virtual void showCenterOfRotation ( bool b );
  virtual bool showCenterOfRotation ( ) const;

  /// Usul::Interfaces::IScreenCapture
  virtual osg::Image* screenCapture ( const osg::Vec3f& center, float distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const;
  virtual osg::Image* screenCapture ( unsigned int height, unsigned int width ) const;

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
    _SHOW_STAGE         = 0x00000040,
    _SHOW_COR           = 0x00000080
  };

  // Local integer constants.
  enum
  {
    _ANIMATION_TIMER_MILLISECONDS    = 15, // 15ms will give about 60FPS for OpenGL rendering.
    _ANIMATION_DURATION_MILLISECONDS = 500 // Half a second.
  };

  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef osg::ref_ptr<osg::LOD> LodPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  typedef osg::ref_ptr<Node> NodePtr;
  typedef osg::ref_ptr<Group> GroupPtr;
  typedef osg::ref_ptr < osgText::Text > TextPtr;
  typedef std::pair < bool,osg::Matrixd > CameraBuffer;
  typedef Document::RefPtr DocumentPtr;
  typedef std::pair < float, float > XYPair;
  typedef std::pair < int, int > MatrixProperties;
  typedef std::vector < std::vector< TextPtr > > TextMatrix;
  typedef std::pair < TextMatrix, GeodePtr > MatrixPair;
  typedef std::pair < MatrixPair, MatrixProperties > MatrixData;
  typedef std::map < XYPair, MatrixData > TextMap;
  typedef std::map< osg::Geode *, osg::Light * > LightEditors;
  typedef std::vector<LodPtr> LodList;
  typedef std::pair<bool,LodList> Lods;

  static CameraBuffer _cameraCopyBuffer;
  static MatrixManipPtr _navManipCopyBuffer;
  IContext::RefPtr _context;
  Renderer::ValidRefPtr _renderer;
  SceneManager::ValidRefPtr _sceneManager;
  ISetCursorType::QueryPtr _setCursor;
  ITimeoutSpin::QueryPtr _timeoutSpin;
  IUnknown::QueryPtr _caller;
  Lods _lods;
  DocumentPtr _document;
  FrameDump _frameDump;
  TextMap _textMap;
  unsigned int _refCount;
  unsigned int _flags;
  OsgTools::Render::Animation _animation;
  MatrixManipPtr _navManip;
  ITool::QueryPtr _currentTool;
  ITool::QueryPtr _lastTool;
  ViewMode _currentMode;
  LightEditors _lightEditors;
  unsigned int _contextId;
};

}
}

#endif // __OSG_TOOLS_RENDER_VIEWER_H__
