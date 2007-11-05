
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SNAP_SHOT_WIDGET_H__
#define __SNAP_SHOT_WIDGET_H__

#include "CompileGuard.h"
#include "ui_SnapShot.h"

#include "Usul/Interfaces/IUnknown.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class SnapShotWidget : public QWidget,
                       private Ui::SnapShot
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef std::vector < std::string > Files;

  SnapShotWidget( QWidget *parent = 0x0, Usul::Interfaces::IUnknown* caller = 0x0 );
  virtual ~SnapShotWidget();

protected:
  void             _deleteFiles ();

private slots:

  void             on__snapShotButton_clicked();

private:

  Files            _files;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};


#endif // __SNAP_SHOT_WIDGET_H__
