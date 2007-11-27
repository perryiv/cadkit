
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for the action.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PROPERTIES_ACTION_H__
#define __PROPERTIES_ACTION_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/ILayer.h"

#include "QtGui/QAction"

class PropertiesAction : public QAction
{
  Q_OBJECT

public:

  // Typedefs.
  typedef QAction BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ILayer   ILayer;

  // Constructor.
  PropertiesAction ( ILayer *layer, IUnknown *caller );

  // Destructor.
  virtual ~PropertiesAction();

private slots:

  void                _slot();

private:

  // No copying or assignment.
  PropertiesAction ( const PropertiesAction & );
  PropertiesAction &operator = ( const PropertiesAction & );

  void                _destroy();

  void                _execute();

  ILayer::RefPtr _layer;
  IUnknown::RefPtr _caller;
};


#endif //__PROPERTIES_ACTION_H__
