
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

#include "AppFrameWork/Core/Export.h"
#include "AppFrameWork/Menus/MenuBar.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT MainWindow : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindow );

  // Possible flags
  enum
  {
    DIRTY = 0x01
  };

  // Constructor
  MainWindow();

  // Build a default GUI.
  void                                buildDefault();

  // Set/get the dirty flag.
  void                                dirty ( bool );
  bool                                dirty() const;

  // Set/get the main window.
  const AFW::Menus::MenuBar *         menuBar() const { return _menuBar.get(); }
  AFW::Menus::MenuBar *               menuBar()       { return _menuBar.get(); }
  void                                menuBar ( AFW::Menus::MenuBar *mb ) { _menuBar = mb; }

protected:

  // Use reference counting.
  virtual ~MainWindow();

  void                                _buildDefault();

private:

  // No copying.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  AFW::Menus::MenuBar::RefPtr _menuBar;
  unsigned int _flags;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_MAIN_WINDOW_CLASS_H_
