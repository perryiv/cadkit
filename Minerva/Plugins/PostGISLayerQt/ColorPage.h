
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_COLOR_PAGE_H__
#define __POSTGIS_COLOR_PAGE_H__

#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"

#include "QtCore/QObject"

#include "Usul/Headers/Qt.h"

#if ( QT_VERSION >= QT_VERSION_CHECK ( 4, 3, 0 ) )

#include "QtGui/QWizardPage"

class AddPostGISLayerWidget;

class ColorPage : public QWizardPage
{
public:
  typedef QWizardPage BaseClass;
  typedef Minerva::Core::DB::Connection Connection;
  typedef Minerva::Core::Layers::Layer  Layer;

  ColorPage ( AddPostGISLayerWidget* );

  virtual void initializePage ();

private:
  AddPostGISLayerWidget *_widget;

};

#endif // QT_VERSION

#endif // __POSTGIS_COLOR_PAGE_H__
