
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Property browser.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_TOOLS_PROPERTY_BROWSER_CLASS_H_
#define _QT_TOOLS_PROPERTY_BROWSER_CLASS_H_

#include "QtTools/Export.h"

#include "QtGui/QWidget"

class QTableWidget;


namespace QtTools {


class QT_TOOLS_EXPORT PropertyBrowser : public QWidget
{
  Q_OBJECT;

public:

  typedef QWidget BaseClass;

  PropertyBrowser ( QWidget *parent = 0x0 );
  virtual ~PropertyBrowser();

private:

  void                            _destroy();

  QTableWidget *_table;
};


} // namespace QtTools


#endif // _QT_TOOLS_PROPERTY_BROWSER_CLASS_H_
