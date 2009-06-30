
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "SourceDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SourceDialog::SourceDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Name" );
  titles.push_back( "Location" );
  titles.push_back( "Size" );

  _sourceTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SourceDialog::~SourceDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Sources.
//
///////////////////////////////////////////////////////////////////////////////

SourceDialog::Sources SourceDialog::sources()
{
  return _sources;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Sources.
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::sources( Sources s )
{
  _sources = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _sources.size(); ++i )
  {
    // create a Source object
    Source s ( _sources.at( i ) );

    // add a row
    _sourceTable->insertRow( rowCount );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the third column
    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    if( true == firstPass )
    {
      // set the current item
      _sourceTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _sourceTable->setItem( rowCount, 0, item0 );
    _sourceTable->setItem( rowCount, 1, item1 );
    _sourceTable->setItem( rowCount, 2, item1 );

    // set the values of the row
    _sourceTable->item( rowCount, 0 )->setText( s.name.c_str() );
    _sourceTable->item( rowCount, 1 )->setText( Usul::Strings::format( "( ", s.x, ", ", s.y, ", ", s.z, " )" ).c_str() );
    _sourceTable->item( rowCount, 2 )->setText( Usul::Strings::format( "( ", s.l, ", ", s.w, ", ", s.h, " )" ).c_str() );


    ++rowCount;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add a Source
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string l    ( this->_length->text().toStdString()        );
  std::string w    ( this->_width->text().toStdString()         );
  std::string h    ( this->_height->text().toStdString()        );
  std::string x    ( this->_xpos->text().toStdString()          );
  std::string y    ( this->_ypos->text().toStdString()          );
  std::string z    ( this->_zpos->text().toStdString()          );
  std::string name ( this->_name->text().toStdString()          );

  Contaminants c;

  // create a Source object
  Source s ( l, w, h, x, y, z, name, c );
  
  // add the Source to the list of contamimants
  _sources.push_back( s );

  int rowCount ( _sourceTable->rowCount() );

  // add a row
  _sourceTable->insertRow( rowCount );

  // create an item widget for the first column
  QTableWidgetItem *item0 = new QTableWidgetItem;
  item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the second column
  QTableWidgetItem *item1 = new QTableWidgetItem;
  item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the third column
  QTableWidgetItem *item2 = new QTableWidgetItem;
  item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the fourth column
  QTableWidgetItem *item3 = new QTableWidgetItem;
  item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // set the current item
  _sourceTable->setCurrentItem( item0 );

  // insert the columns
  _sourceTable->setItem( rowCount, 0, item0 );
  _sourceTable->setItem( rowCount, 1, item1 );
  _sourceTable->setItem( rowCount, 2, item2 );

  // set the values of the row
  _sourceTable->item( rowCount, 0 )->setText( name.c_str() );
  _sourceTable->item( rowCount, 1 )->setText( Usul::Strings::format( "( ", x, ", ", y, ", ", z, " )" ).c_str() );
  _sourceTable->item( rowCount, 2 )->setText( Usul::Strings::format( "( ", l, ", ", w, ", ", h, " )" ).c_str() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a Source
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::on_removeButton_clicked()
{

}

