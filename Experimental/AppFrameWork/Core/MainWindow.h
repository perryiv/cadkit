
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MAIN_WINDOW_CLASS_H_
#define _APP_FRAME_WORK_MAIN_WINDOW_CLASS_H_

#include "AppFrameWork/Core/Frame.h"
#include "AppFrameWork/Core/TextWindow.h"
#include "AppFrameWork/Menus/MenuBar.h"


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT MainWindow : public Frame
{
public:

  // Typedefs.
  typedef Frame BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindow );

  // Constructor
  MainWindow();

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Build a default GUI.
  void                                buildDefault();

  // Set/get the dirty flag.
  virtual void                        dirty ( bool );
  virtual bool                        dirty() const;

  // Set/get the menu bar.
  const AFW::Menus::MenuBar *         menuBar() const;
  AFW::Menus::MenuBar *               menuBar();
  void                                menuBar ( AFW::Menus::MenuBar * );

  // Set/get the text window.
  const TextWindow *                  textWindow() const;
  TextWindow *                        textWindow();
  void                                textWindow ( TextWindow * );

protected:

  // Use reference counting.
  virtual ~MainWindow();

  void                                _buildDefault();

private:

  // No copying.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  AFW::Menus::MenuBar::RefPtr _menuBar;
  Frame::RefPtr _editors;
  TextWindow::RefPtr _textWindow;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_MAIN_WINDOW_CLASS_H_
