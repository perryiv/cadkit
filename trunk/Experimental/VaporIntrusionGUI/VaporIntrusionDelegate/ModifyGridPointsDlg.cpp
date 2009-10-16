
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

#include "ModifyGridPointsDlg.h"

#include "Usul/Strings/Format.h"
#include "Usul/Convert/Convert.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ModifyGridPointsDlg::ModifyGridPointsDlg ( QWidget *parent ) : 
BaseClass ( parent ),
_axisPoints()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Axis" );
  titles.push_back( "Value" );
  _gridTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ModifyGridPointsDlg::~ModifyGridPointsDlg()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _axisPoints.size(); ++i )
  {
    // create a Source object
    GridAxisPoint g ( _axisPoints.at( i ) );

    // add a row
    _gridTable->insertRow( rowCount );

     // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    if( true == firstPass )
    {
      // set the current item
      _gridTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _gridTable->setItem( rowCount, 0, item0 );
    _gridTable->setItem( rowCount, 1, item1 );

    // set the values of the row
    _gridTable->item( rowCount, 0 )->setText( g.axis.c_str() );
    _gridTable->item( rowCount, 1 )->setText( g.value.c_str() );

    ++rowCount;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string axis  ( this->_getAxis()                    );
  std::string value ( this->_value->text().toStdString()  );

  GridAxisPoint gap ( axis, value );
 
  // add the Source to the list of contamimants
  _axisPoints.push_back( gap );

  // increment the row count
  int rowCount ( _gridTable->rowCount() );

  // add a row
  _gridTable->insertRow( rowCount );

  // create an item widget for the first column
  QTableWidgetItem *item0 = new QTableWidgetItem;
  item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the second column
  QTableWidgetItem *item1 = new QTableWidgetItem;
  item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // set the current item
  _gridTable->setCurrentItem( item0 );

  // insert the columns
  _gridTable->setItem( rowCount, 0, item0 );
  _gridTable->setItem( rowCount, 1, item1 );

  // set the values of the row
  _gridTable->item( rowCount, 0 )->setText( axis.c_str()  );
  _gridTable->item( rowCount, 1 )->setText( value.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::on_removeButton_clicked()
{
  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _gridTable->selectedItems() );

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
  GridAxisPoints newlist;

  // old building cracks
  GridAxisPoints oldlist ( _axisPoints );

  for( unsigned int i = 0; i < oldlist.size(); ++i )
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
      newlist.push_back( oldlist.at( i ) );
    }
  }

  // update the building cracks
  _axisPoints = newlist;

  // clear the cracks table
  this->_clearTable();

  // repopulate the table
  this->_initialize();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the soils table
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::_clearTable()
{
  // remove all the rows
  for( int i = _gridTable->rowCount() - 1; i >= 0 ; --i )
  {
    _gridTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Soils
//
///////////////////////////////////////////////////////////////////////////////

ModifyGridPointsDlg::GridAxisPoints ModifyGridPointsDlg::gridAxisPoints()
{
  return _axisPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soils
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::gridAxisPoints( GridAxisPoints ap )
{
  _axisPoints = ap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update a Soil
//
///////////////////////////////////////////////////////////////////////////////

void ModifyGridPointsDlg::on_updateButton_clicked()
{
  // get the parameters from the text boxes
  std::string axis  ( this->_getAxis()                    );
  std::string value ( this->_value->text().toStdString()  );

  GridAxisPoint gap ( axis, value );

  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _gridTable->selectedItems() );

  // User selected rows
  std::vector< unsigned int > selectedRows;

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

  for( unsigned int i = 0; i < selectedRows.size(); ++i )
  {
    // updated the selected rows
    _axisPoints.at( selectedRows.at( i ) ) = gap;
  }

  // clear the cracks table
  this->_clearTable();

  // repopulate the table
  this->_initialize();

}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the currently selected axis radio and return a string representation
//
///////////////////////////////////////////////////////////////////////////////

std::string ModifyGridPointsDlg::_getAxis()
{
  std::string axis;

  // X Axis is selected
  if( true == _x->isChecked() )
  {
    axis = "X";
  };

  // Y Axis is selected
  if( true == _y->isChecked() )
  {
    axis = "Y";
  };

  // Z Axis is selected
  if( true == _z->isChecked() )
  {
    axis = "Z";
  };

  return axis;
}