
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PERFORMER_JSAL_APPLICATION_CLASS_H_
#define _PERFORMER_JSAL_APPLICATION_CLASS_H_

#include "JSAL/Performer/CompileGuard.h"
#include "JSAL/Interfaces/IApplication.h"
#include "JSAL/Interfaces/IChild.h"
#include "JSAL/Interfaces/IQuit.h"
#include "JSAL/Interfaces/IViewer.h"

#include "vrj/Draw/Pf/PfApp.h"

#include "Usul/Base/Referenced.h"

class pfNode;
class pfGroup;


namespace JSAL {
namespace Performer {


class Application : public vrj::PfApp,
                    public Usul::Base::Referenced,
                    public JSAL::Interfaces::IApplication,
                    public JSAL::Interfaces::IChild,
                    public JSAL::Interfaces::IQuit,
                    public JSAL::Interfaces::IViewer
{
public:

  /// Typedefs.
  typedef vrj::PfApp BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Application );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Application();

protected:

  typedef Usul::Interfaces::IUnknown Unknown;
  typedef SAL::Interfaces::INode Node;
  typedef USUL_VALID_ACCESS_REF_POINTER(::pfNode) ValidAccessPfNodePtr;
  typedef USUL_VALID_ACCESS_REF_POINTER(::pfGroup) ValidAccessPfGroupPtr;
  typedef USUL_VALID_REF_POINTER(::pfNode) ValidPfNodePtr;
  typedef USUL_VALID_REF_POINTER(::pfGroup) ValidPfGroupPtr;

  /// Do not copy.
  Application ( const Application & );
  Application &operator = ( const Application & );

  /// Use reference counting.
  virtual ~Application();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IChild
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the parent.
  virtual void              parent ( Unknown * );
  virtual Unknown *         parent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IQuit
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called when program is about to quit.
  virtual void              quit();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IViewer
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the scene.
  virtual void              scene ( Node * );
  virtual Node *            scene();

  /////////////////////////////////////////////////////////////////////////////

  // Called once for each display every frame.
  virtual void              appChanFunc ( pfChannel *channel );

  // Called by the Performer Draw Manager as soon as the window is opened.
  virtual void              configPWin ( pfPipeWindow *window );

  // Called when it's time to clean up.
  virtual void              exit();

  // Get the pfNode from the IUnknown.
  static ::pfNode *         _getNode ( Usul::Interfaces::IUnknown * );

  // Returns the current scene graph.
  virtual ::pfGroup *       getScene();

  // Called by the Performer Draw Manager to initialize the scene.
  virtual void              initScene();

  // Called between pfInit and pfConfig.
  virtual void              preForkInit();

  // Called before and after the frame is drawn.
  virtual void              preFrame();
  virtual void              postFrame();

private:

  Usul::Interfaces::IUnknown *_parent; // No circular reference.
  ValidAccessPfGroupPtr _root;
  ValidAccessPfNodePtr _scene;
  ValidAccessPfGroupPtr _light;
};


}; // namespace Performer
}; // namespace JSAL


#endif // _PERFORMER_JSAL_APPLICATION_CLASS_H_
