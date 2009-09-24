
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

#include "CracksDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CracksDialog::CracksDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_cracks()
{
  // Initialize code from Designer.
  this->setupUi ( this );
  

  QStringList titles;

  titles.push_back( "Start" );
  titles.push_back( "End" );
  titles.push_back( "Width" );

  _xCracksTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CracksDialog::~CracksDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Building Parameters
//
///////////////////////////////////////////////////////////////////////////////

CracksDialog::CracksPair CracksDialog::cracks()
{
  
  return _cracks;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the building
//
///////////////////////////////////////////////////////////////////////////////


void CracksDialog::cracks( CracksPair c )
{
  _cracks = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize (public)
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::initialize()
{
	this->_xInitialize();
  this->_yInitialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::on_xAddButton_clicked()
{
  // get the parameters from the text boxes
  std::string x1 ( this->_startX->text().toStdString()  );
  std::string x2 ( this->_endX->text().toStdString()    );
  std::string y  ( this->_yValue->text().toStdString()   );

  Crack c ( x1, x2, y);

  // add the Source to the list of contamimants
  _cracks.first.push_back( c );

  // increment the row count
  int rowCount ( _xCracksTable->rowCount() );

  // add a row
  _xCracksTable->insertRow( rowCount );

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
  _xCracksTable->setCurrentItem( item0 );

  // insert the columns
  _xCracksTable->setItem( rowCount, 0, item0 );
  _xCracksTable->setItem( rowCount, 1, item1 );
  _xCracksTable->setItem( rowCount, 2, item2 );
  
  // set the values of the row
  _xCracksTable->item( rowCount, 0 )->setText( c.start.c_str() );
  _xCracksTable->item( rowCount, 1 )->setText( c.end.c_str()   );
  _xCracksTable->item( rowCount, 2 )->setText( c.value.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::on_xRemoveButton_clicked()
{
  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _xCracksTable->selectedItems() );

  std::vector< unsigned int > rowsToRemove;

  // loop through the selected contaminants
  for( int i = 0; i < selectedItems.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // add to the rows to remove list
    rowsToRemove.push_back( row );
  }

  // new building cracks
  Cracks newCracks;

  // old building cracks
  Cracks oldCracks ( _cracks.first );

  for( unsigned int i = 0; i < oldCracks.size(); ++i )
  {
    // remove the row or not
    bool removeRow ( false );

    for( unsigned int j = 0; j < rowsToRemove.size(); ++j )
    {
      if( i == rowsToRemove.at( j ) )
      {
        // this row is marked to be removed
        removeRow = true;
      }
    }

    // if the row is not to be removed then add it to the new cracks list
    if( false == removeRow )
    {
      newCracks.push_back( oldCracks.at( i ) );
    }
  }

  // update the building cracks
  _cracks.first = newCracks;

  // clear the cracks table
  this->_xClearTable();

  // repopulate the table
  this->_xInitialize();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the cracks table
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::_xClearTable()
{
  // remove all the rows
  for( int i = _xCracksTable->rowCount() - 1; i >= 0 ; --i )
  {
    _xCracksTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update a crack
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::on_xUpdateButton_clicked()
{
  // get the parameters from the text boxes
  std::string x1 ( this->_startX->text().toStdString()  );
  std::string x2 ( this->_endX->text().toStdString()    );
  std::string y  ( this->_yValue->text().toStdString()   );

  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _xCracksTable->selectedItems() );

  // selected rows
  std::vector< unsigned int > selectedRows;

  // new building cracks
  Cracks newCracks;

  // old building cracks
  Cracks oldCracks ( _cracks.first );

  // loop through the selected contaminants
  for( int i = 0; i < selectedItems.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // add to the rows to remove list
    selectedRows.push_back( row );
  }

  // make a crack entry for this update
  Crack crack ( x1, x2, y );

  for( unsigned int i = 0; i < selectedRows.size(); ++i )
  {
    // update the crack
    _cracks.first.at( selectedRows.at( i ) ) = crack;

  }

  // clear the cracks table
  this->_xClearTable();

  // repopulate the table
  this->_xInitialize();
    
}




///////////////////////////////////////////////////////////////////////////////
//
//  Initialize (private)
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::_xInitialize()
{
   // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  // get the cracks
  Cracks cracks ( _cracks.first );

  for( unsigned int i = 0; i < cracks.size(); ++i )
  {
    // create a Source object
    Crack c ( cracks.at( i ) );

    // add a row
    _xCracksTable->insertRow( rowCount );

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
      _xCracksTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _xCracksTable->setItem( rowCount, 0, item0 );
    _xCracksTable->setItem( rowCount, 1, item1 );
    _xCracksTable->setItem( rowCount, 2, item2 );

    // set the values of the row
    _xCracksTable->item( rowCount, 0 )->setText( c.start.c_str()  );
    _xCracksTable->item( rowCount, 1 )->setText( c.end.c_str()  );
    _xCracksTable->item( rowCount, 2 )->setText( c.value.c_str()   );

    ++rowCount;
  }
}

//-------------------------------------------------------------------------------
// Y Values
//-------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::on_yAddButton_clicked()
{
  // get the parameters from the text boxes
  std::string y1 ( this->_startY->text().toStdString()  );
  std::string y2 ( this->_endY->text().toStdString()    );
  std::string x  ( this->_xValue->text().toStdString()   );

  Crack c ( y1, y2, x );

  // add the Source to the list of contamimants
  _cracks.second.push_back( c );

  // increment the row count
  int rowCount ( _yCracksTable->rowCount() );

  // add a row
  _yCracksTable->insertRow( rowCount );

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
  _yCracksTable->setCurrentItem( item0 );

  // insert the columns
  _yCracksTable->setItem( rowCount, 0, item0 );
  _yCracksTable->setItem( rowCount, 1, item1 );
  _yCracksTable->setItem( rowCount, 2, item2 );

  // set the values of the row
  _yCracksTable->item( rowCount, 0 )->setText( c.start.c_str() );
  _yCracksTable->item( rowCount, 1 )->setText( c.end.c_str()   );
  _yCracksTable->item( rowCount, 2 )->setText( c.value.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::on_yRemoveButton_clicked()
{
  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _yCracksTable->selectedItems() );

  std::vector< unsigned int > rowsToRemove;

  // loop through the selected contaminants
  for( int i = 0; i < selectedItems.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // add to the rows to remove list
    rowsToRemove.push_back( row );
  }

  // new building cracks
  Cracks newCracks;

  // old building cracks
  Cracks oldCracks ( _cracks.second );

  for( unsigned int i = 0; i < oldCracks.size(); ++i )
  {
    // remove the row or not
    bool removeRow ( false );

    for( unsigned int j = 0; j < rowsToRemove.size(); ++j )
    {
      if( i == rowsToRemove.at( j ) )
      {
        // this row is marked to be removed
        removeRow = true;
      }
    }

    // if the row is not to be removed then add it to the new cracks list
    if( false == removeRow )
    {
      newCracks.push_back( oldCracks.at( i ) );
    }
  }

  // update the building cracks
  _cracks.second = newCracks;

  // clear the cracks table
  this->_yClearTable();

  // repopulate the table
  this->_yInitialize();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the cracks table
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::_yClearTable()
{
  // remove all the rows
  for( int i = _yCracksTable->rowCount() - 1; i >= 0 ; --i )
  {
    _yCracksTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update a crack
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::on_yUpdateButton_clicked()
{
  // get the parameters from the text boxes
  std::string y1 ( this->_startY->text().toStdString()  );
  std::string y2 ( this->_endY->text().toStdString()    );
  std::string x  ( this->_xValue->text().toStdString()   );

  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _yCracksTable->selectedItems() );

  // selected rows
  std::vector< unsigned int > selectedRows;

  // new building cracks
  Cracks newCracks;

  // old building cracks
  Cracks oldCracks ( _cracks.first );

  // loop through the selected contaminants
  for( int i = 0; i < selectedItems.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // add to the rows to remove list
    selectedRows.push_back( row );
  }

  // make a crack entry for this update
  Crack crack ( y1, y2, x );

  for( unsigned int i = 0; i < selectedRows.size(); ++i )
  {
    // update the crack
    _cracks.first.at( selectedRows.at( i ) ) = crack;

  }

  // clear the cracks table
  this->_yClearTable();

  // repopulate the table
  this->_yInitialize();
    
}




///////////////////////////////////////////////////////////////////////////////
//
//  Initialize (private)
//
///////////////////////////////////////////////////////////////////////////////

void CracksDialog::_yInitialize()
{
   // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  // get the cracks
  Cracks cracks ( _cracks.second );

  for( unsigned int i = 0; i < cracks.size(); ++i )
  {
    // create a Source object
    Crack c ( cracks.at( i ) );

    // add a row
    _yCracksTable->insertRow( rowCount );

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
      _yCracksTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _yCracksTable->setItem( rowCount, 0, item0 );
    _yCracksTable->setItem( rowCount, 1, item1 );
    _yCracksTable->setItem( rowCount, 2, item2 );

    // set the values of the row
    _yCracksTable->item( rowCount, 0 )->setText( c.start.c_str() );
    _yCracksTable->item( rowCount, 1 )->setText( c.end.c_str()   );
    _yCracksTable->item( rowCount, 2 )->setText( c.value.c_str() );

    ++rowCount;
  }
}
