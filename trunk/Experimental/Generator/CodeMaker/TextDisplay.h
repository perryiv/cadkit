
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The text display window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CODE_MAKER_TEXT_DISPLAY_CLASS_H_
#define _CODE_MAKER_MAIN_WINDOW_CLASS_H_


class MainWindow : public wxFrame
{
public:

  typedef wxFrame BaseClass;

  MainWindow();
  virtual ~MainWindow();

protected:

  void            _onBrowseForDirectory ( wxCommandEvent &event );
  void            _onCloseMainWindow    ( wxCloseEvent &event );

private:

  wxPanel *_leftPanel;

  DECLARE_CLASS ( MainWindow );
  DECLARE_EVENT_TABLE();
};


#endif // _CODE_MAKER_MAIN_WINDOW_CLASS_H_
