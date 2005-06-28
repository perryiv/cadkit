
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL child window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_CHILD_WINDOW_H_
#define _FOX_TOOLS_CHILD_WINDOW_H_

#include "FoxTools/Export/Export.h"

#include "FoxTools/Headers/MDIChild.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IWindow.h"
#include "Usul/Interfaces/IQuestion.h"

namespace FX
{
  class FXMenuPane; 
  class FXIcon;
  class FXMDIClient;
}

namespace FoxTools {
namespace Windows {

class FOX_TOOLS_EXPORT MdiChildWindow : public FX::FXMDIChild,
                                        public Usul::Interfaces::IView,
                                        public Usul::Interfaces::IWindow,
                                        public Usul::Interfaces::IQuestion
{
public:

  // Useful typedefs.
  typedef FXMDIChild BaseClass;
  typedef Usul::Interfaces::IDocument Document;
  typedef Document::ValidRefPtr DocumentPtr;
  typedef Usul::Pointers::Configs::NoRefCountingNullAccessThrows ViewPointerPolicy;
  typedef Usul::Pointers::SmartPointer < Usul::Interfaces::IViewer, ViewPointerPolicy > ViewPtr;

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction/destruction.
  MdiChildWindow ( 
    Document *doc,
    FX::FXMDIClient *parent,
    const std::string &name,
    FX::FXIcon *icon = 0x0,
    FX::FXMenuPane *menu = 0x0,
    FX::FXuint options = 0 );
  MdiChildWindow();
  virtual ~MdiChildWindow();

  // Create.
  virtual void              create();

  /// Get the document.
  const Document *          document() const;
  Document *                document();

  /// Usul::Interfaces::IView
  virtual Usul::Interfaces::IViewer* view();
  virtual void                       view( Usul::Interfaces::IViewer* );

  // Is it created?
  bool                      isCreated() const;

  // Message handlers.
  long                      onCommandClose           ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandDocumentClose   ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandDropFile        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandUpdateTitle     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onDisplayListUpdate      ( FX::FXObject *, FX::FXSelector, void * );

  long                      onMotion                 ( FX::FXObject *, FX::FXSelector, void * );
  long                      onBuildScene             ( FX::FXObject *, FX::FXSelector, void * );
  long                      onClearScene             ( FX::FXObject *, FX::FXSelector, void * );
  long                      onFocusIn                ( FX::FXObject *, FX::FXSelector, void * );

  // Set the status bar text.
  void                      setStatusBarText ( const std::string &text, bool force );

protected:

  void                      _initPlacement();
  void                      _reportErrors ( unsigned int options, bool clear = false );  

  /// Usul::Interfaces::IWindow
  virtual void              setFocus();
  virtual void              handleMessage ( unsigned short message );

  /// Usul::Interfaces::IQuestion
  virtual std::string       question ( const std::string &buttons,  const std::string &title, const std::string &text );

private:

  DocumentPtr _document;
  ViewPtr _view;
  unsigned int _refCount;

  FXDECLARE ( MdiChildWindow );
};

} // namespace Windows
} // namespace FoxTools


#endif //_FOX_TOOLS_CHILD_WINDOW_H_
