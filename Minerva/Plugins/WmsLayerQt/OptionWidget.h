
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_PLUGINS_WMS_OPTION_WIDGET_H__
#define __MINERVA_PLUGINS_WMS_OPTION_WIDGET_H__

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QLineEdit;
class QComboBox;

class OptionWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef std::vector<std::string> Names;
  
  OptionWidget ( const Names& names, QWidget *parent = 0x0 );
  virtual ~OptionWidget();
  
  // Get the key.
  std::string    key() const;
  
  // Get the value.
  std::string    value() const;
  
private slots:
  void _optionsColumnChanged( int index );
  
private:
  QComboBox *_options;
  QLineEdit *_key;
  QLineEdit *_value;
};

#endif //__MINERVA_PLUGINS_WMS_OPTION_WIDGET_H__
