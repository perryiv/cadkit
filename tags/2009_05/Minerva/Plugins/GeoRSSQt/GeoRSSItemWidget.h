
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GEO_RSS_ITEM_WIDGET_H__
#define __GEO_RSS_ITEM_WIDGET_H__

#include "Minerva/Plugins/GeoRSSQt/CompileGuard.h"
#include "ui_GeoRSSItemWidget.h"

#include "QtGui/QWidget"

class GeoRSSItemWidget : public QWidget,
                         private Ui::GeoRSSItemWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  
  /// Construction/Destruction.
  GeoRSSItemWidget ( QWidget* parent = 0x0 );
  virtual ~GeoRSSItemWidget();
  
  /// Set/get the title.
  void                     title ( const std::string& );
  std::string              title() const;
  
  /// Set/get the description.
  void                     description ( const std::string& );
  std::string              description() const;
  
};



#endif // __GEO_RSS_ITEM_WIDGET_H__
