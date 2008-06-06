
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

#include "QtTools/PropertyBrowser.h"

#include "QtGui/QHBoxLayout"
#include "QtGui/QHeaderView"
#include "QtGui/QTableWidget"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyBrowser::PropertyBrowser ( QWidget *parent ) : 
  BaseClass ( parent ),
  _table ( 0x0 )
{
  // Make the top-level layout.
  QHBoxLayout *layout ( new QHBoxLayout ( parent ) );
  this->setLayout ( layout );
  layout->setMargin ( 0 );

  // Make the table widget and add it to the layout.
  _table = new QTableWidget;
  layout->addWidget ( _table );

  // Set table properties.
  _table->setColumnCount ( 2 );
  _table->setRowCount ( 10 );
  _table->setAlternatingRowColors ( true );
  _table->setAutoScroll ( true );
  _table->verticalHeader()->setHidden ( 1 );
  QStringList labels; labels.push_back ( "Property" ); labels.push_back ( "Value" );
  _table->setHorizontalHeaderLabels ( labels );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyBrowser::~PropertyBrowser()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &PropertyBrowser::_destroy ), "5323338710" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy stuff here.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyBrowser::_destroy()
{
  // Note: _table gets deleted by it's parent.
}
