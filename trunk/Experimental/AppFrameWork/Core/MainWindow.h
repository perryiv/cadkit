
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

#include "AppFrameWork/Core/Window.h"
#include "AppFrameWork/Menus/MenuBar.h"


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT MainWindow : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindow );

  // Constructor
  MainWindow();

  // Build a default GUI.
  void                                buildDefault();

  // Set/get the dirty flag.
  virtual void                        dirty ( bool );
  virtual bool                        dirty() const;

  // Set/get the menu bar.
  const AFW::Menus::MenuBar *         menuBar() const;
  AFW::Menus::MenuBar *               menuBar();
  void                                menuBar ( AFW::Menus::MenuBar *mb );

protected:

  // Use reference counting.
  virtual ~MainWindow();

  void                                _buildDefault();

private:

  // No copying.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  AFW::Menus::MenuBar::RefPtr _menuBar;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_MAIN_WINDOW_CLASS_H_
