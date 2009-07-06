
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

#include "BuildingDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BuildingDialog::BuildingDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_building(),
_cracks()
{
  // Initialize code from Designer.
  this->setupUi ( this );
  

  QStringList titles;

  titles.push_back( "Start" );
  titles.push_back( "End" );
  titles.push_back( "Width" );

  _cracksTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BuildingDialog::~BuildingDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Building Parameters
//
///////////////////////////////////////////////////////////////////////////////

BuildingDialog::Building BuildingDialog::building()
{
  Building b( _lengthField->text().toStdString(), _widthField->text().toStdString(), _heightField->text().toStdString(), 
              "0", "0", "0", 
              _volume->text().toStdString() );
  b.cracks = _cracks;  
  _building = b;
  return b;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the building
//
///////////////////////////////////////////////////////////////////////////////


void BuildingDialog::building( Building b )
{
  _building = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Yes/No there is a building present
//
///////////////////////////////////////////////////////////////////////////////

bool BuildingDialog::useBuilding()
{
  bool value ( _buildingExists->checkState() );
  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void BuildingDialog::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string x1 ( this->_startX->text().toStdString()  );
  std::string x2 ( this->_endX->text().toStdString()    );
  std::string y1 ( this->_startY->text().toStdString()  );
  std::string y2 ( this->_endY->text().toStdString()    );
  std::string w  ( this->_width->text().toStdString()   );

  Crack c ( x1, x2, y1, y2, w );

  // add the Source to the list of contamimants
  _cracks.push_back( c );

  // increment the row count
  int rowCount ( _cracksTable->rowCount() );

  // add a row
  _cracksTable->insertRow( rowCount );

  // create an item widget for the first column
  QTableWidgetItem *item0 = new QTableWidgetItem;
  item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the second column
  QTableWidgetItem *item1 = new QTableWidgetItem;
  item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the third column
  QTableWidgetItem *item2 = new QTableWidgetItem;
  item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // set the current item
  _cracksTable->setCurrentItem( item0 );

  // insert the columns
  _cracksTable->setItem( rowCount, 0, item0 );
  _cracksTable->setItem( rowCount, 1, item1 );
  _cracksTable->setItem( rowCount, 2, item2 );

  // create the column strings
  std::string xcol ( Usul::Strings::format( "( ", x1, ", ", y1, " )" ) );
  std::string ycol ( Usul::Strings::format( "( ", x2, ", ", y2, " )" ) );

  // set the values of the row
  _cracksTable->item( rowCount, 0 )->setText( xcol.c_str() );
  _cracksTable->item( rowCount, 1 )->setText( ycol.c_str() );
  _cracksTable->item( rowCount, 2 )->setText( w.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void BuildingDialog::on_removeButton_clicked()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize (public)
//
///////////////////////////////////////////////////////////////////////////////

void BuildingDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize (private)
//
///////////////////////////////////////////////////////////////////////////////

void BuildingDialog::_initialize()
{
   // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  // get the cracks
  Cracks cracks ( _building.cracks );

  for( unsigned int i = 0; i < cracks.size(); ++i )
  {
    // create a Source object
    Crack c ( cracks.at( i ) );

    // add a row
    _cracksTable->insertRow( rowCount );

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
      _cracksTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _cracksTable->setItem( rowCount, 0, item0 );
    _cracksTable->setItem( rowCount, 1, item1 );
    _cracksTable->setItem( rowCount, 2, item2 );

    // create the column strings
    std::string xcol ( Usul::Strings::format( "( ", c.x1, ", ", c.y1, " )" ) );
    std::string ycol ( Usul::Strings::format( "( ", c.x2, ", ", c.y2, " )" ) );

    // set the values of the row
    _cracksTable->item( rowCount, 0 )->setText( xcol.c_str()  );
    _cracksTable->item( rowCount, 1 )->setText( ycol.c_str()  );
    _cracksTable->item( rowCount, 2 )->setText( c.w.c_str()   );

    ++rowCount;
  }

  // Define bulding parameter information for the text boxes
  std::string length ( _building.l );
  std::string width  ( _building.w );
  std::string height ( _building.h );
  std::string volume ( _building.v );

  // set the text boxes
  _lengthField->setText ( length.c_str()  );
  _widthField->setText  ( width.c_str()   );
  _heightField->setText ( height.c_str()  );
  _volume->setText      ( volume.c_str()  );

}
