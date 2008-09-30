
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_GEO_RSS_LAYER_WIDGET_H__
#define __ADD_GEO_RSS_LAYER_WIDGET_H__

#include "Minerva/Plugins/GeoRSSQt/CompileGuard.h"
#include "ui_AddGeoRSSLayerWidget.h"

#include "Usul/Math/Vector4.h"

#include "QtGui/QWidget"

class AddGeoRSSLayerWidget : public QWidget,
                             private Ui::AddGeoRSSLayerWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  /// Construction/Destruction.
  AddGeoRSSLayerWidget ( QWidget *parent = 0x0 );
  virtual ~AddGeoRSSLayerWidget();

  /// Set/get the url.
  void              url ( const std::string& );
  std::string       url() const;
  
  /// Set/get the refresh rate.
  void              refreshRate ( double );
	double            refreshRate() const;
  
  /// Set/get the enable filtering.
  void              enableFiltering ( bool );
	bool              enableFiltering() const;
  
  /// Set/get the element to filter.
  void              element ( const std::string& );
	std::string       element() const;
  
  /// Set/get the maximum age of items.
  void              maximumAge ( double );
  double            maximumAge() const;
  
  /// Set/get the maximum number of items.
  void              maximumItems ( unsigned int );
  unsigned int      maximumItems() const;
  
  /// Set/get the value to filter.
  void              value ( const std::string& );
	std::string       value() const;
  
  /// Set/get the color.
  void              color ( const Usul::Math::Vec4f& );
  Usul::Math::Vec4f color() const;
  
  /// Set/get the use regular expression flag.
  void              useRegEx ( bool b );
  bool              useRegEx() const;
  
};


#endif // __ADD_GEO_RSS_LAYER_WIDGET_H__
