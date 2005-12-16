
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CODE_MAKER_APPLICATION_CLASS_H_
#define _CODE_MAKER_APPLICATION_CLASS_H_

class MainWindow;


class Application : public wxApp
{
public:

  typedef wxApp BaseClass;

  Application();
  virtual ~Application();

protected:

  virtual bool                        OnInit();
  virtual int                         OnExit();

  virtual bool                        ProcessIdle();

private:

  MainWindow *_mainWindow;
};


DECLARE_APP ( Application );


#endif // _CODE_MAKER_APPLICATION_CLASS_H_
