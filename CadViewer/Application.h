
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_APPLICATION_CLASS_H_
#define _CV_APPLICATION_CLASS_H_

#include "CadViewer/Declarations.h"

#include "Usul/Pointers/Pointers.h"

#include "VRV/Core/Application.h"

#include "OsgTools/Grid.h"

#include <string>
#include <vector>


namespace CV {

	
// The CadViewer application
class Application : public VRV::Core::Application
{
public:

  // Useful typedefs
  typedef VRV::Core::Application      BaseClass;
  typedef Usul::Interfaces::IUnknown  Unknown;

  // Constructor.
  Application ( );

  // Destructor.
  virtual ~Application();

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Is this the application or main thread?
  static bool                   isAppThread();
  static bool                   isMainThread();

protected:

  // These are not implemented by design.
  Application ( const Application &app );
  Application& operator = ( const Application &app );

  // Calculate the frame-rate.
  double                        _calculateFrameRate() const;

  // Initialize.
  void                          _initGrid ( osg::Node *node );
  void                          _initText();

  // Called by the kernel to initialize this instance.
  virtual void                  _init();

  // Navigate if we are supposed to.
  virtual void                  _navigate();

  // Get the number of selected
  unsigned int                  _numSelected();

  // Called by the kernel before the frame.
  virtual void                  _latePreFrame();

  // Called by the kernel after the frame.
  virtual void                  _postFrame();

  // Select/unselect the intersected node, if any.
  void                          _select();
  void                          _unselect();

  // Fill the given vector with the selected matrix-transforms.
  //void                          _selected ( CV::Functors::Tool::Transforms &vt );

  // rebuild the grid in case the _gridFunctor's state has been updated
  void                          _rebuildGrid();

  // Update the scene holding text.
  void                          _update ( OsgTools::Text &, const std::string & );

  // Update.
  void                          _updateFrameRateDisplay();
  void                          _updateAnalogText();
  void                          _updateSceneTool();

  // Use the scene tool if we are supposed to.
  void                          _useSceneTool();

  // For readability.
  typedef unsigned long                                             ThreadId;
  typedef osg::ref_ptr<osg::MatrixTransform>                        MatTransPtr;
  typedef Usul::Functors::Interaction::Common::BaseFunctor::RefPtr  FunctorPtr;
  typedef std::auto_ptr<OsgTools::Text>                             TextPtr;

  // Data members.
  static ThreadId   _appThread;
  static ThreadId   _mainThread;
  MatTransPtr       _gridBranch;
  FunctorPtr        _sceneTool;
  TextPtr           _pickText;
  TextPtr           _navText;
  TextPtr           _frameText;
  TextPtr           _msgText;
  unsigned int      _flags;
  std::vector<OsgTools::Grid> _gridFunctors;
  bool              _textures;
};


}; // namespace CV


#endif // _CV_APPLICATION_CLASS_H_
