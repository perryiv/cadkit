
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/OptionsDialog.h"

#include "QtGui/QTreeWidgetItem"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsDialog ( const Options& options, QWidget* parent ) : BaseClass ( parent ),
  _options ( options )
{
  this->setupUi( this );
  
  QStringList titles;
  titles.push_back( "Key" );
  titles.push_back( "Value");
  _optionsTreeWidget->setHeaderLabels ( titles );
  
  for ( Options::const_iterator iter = _options.begin(); iter != _options.end(); ++iter )
  {
    QTreeWidgetItem *item ( new QTreeWidgetItem ( _optionsTreeWidget ) );
    QString key ( iter->first.c_str() );
    QString value ( iter->second.c_str() );
    
    item->setText ( 0, key );
    item->setText ( 1, value );
    
    _optionsTreeWidget->addTopLevelItem( item );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::~OptionsDialog()
{
}
