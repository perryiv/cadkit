
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

#include "AppFrameWork/Windows/Frame.h"
#include "AppFrameWork/Windows/StatusBar.h"
#include "AppFrameWork/Menus/MenuBar.h"


namespace AFW {
namespace Windows {


class APP_FRAME_WORK_EXPORT MainWindow : public Frame
{
public:

  // Typedefs.
  typedef Frame BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Write configuration to disk.
  virtual void                        configWrite() const;

  // Set/get the dirty flag.
  virtual void                        dirty ( bool );
  virtual bool                        dirty() const;

  // Initialize, for two-step construction.
  virtual void                        init();

  // Set/get the menu bar.
  const AFW::Menus::MenuBar *         menuBar() const;
  AFW::Menus::MenuBar *               menuBar();
  virtual void                        menuBar ( AFW::Menus::MenuBar * );

  // Set/get the status bar.
  const StatusBar *                   statusBar() const;
  StatusBar *                         statusBar();
  virtual void                        statusBar ( StatusBar * );

protected:

  // Constructor
  MainWindow();

  // Use reference counting.
  virtual ~MainWindow();

  void                                _buildDefault();

private:

  // No copying.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  AFW::Menus::MenuBar::RefPtr _menuBar;
  StatusBar::RefPtr _statusBar;
  Frame::RefPtr _editors;

  AFW_DECLARE_OBJECT ( MainWindow );
};


} // namespace Windows
} // namespace AFW


#endif //_APP_FRAME_WORK_MAIN_WINDOW_CLASS_H_
