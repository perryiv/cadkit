
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

#include "RefineGridDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RefineGridDialog::RefineGridDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_refinements()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Axis" );
  titles.push_back( "Start Value" );
  titles.push_back( "End Value" );
  titles.push_back( "Step Size" );

  _gridTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RefineGridDialog::~RefineGridDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void RefineGridDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void RefineGridDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _refinements.size(); ++i )
  {
    // create a Source object
    GridRefinement g ( _refinements.at( i ) );

    // add a row
    _gridTable->insertRow( rowCount );

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
    _gridTable->setItem( rowCount, 2, item2 );
    _gridTable->setItem( rowCount, 3, item3 );

    // set the values of the row
    _gridTable->item( rowCount, 0 )->setText( g.axis.c_str()          );
    _gridTable->item( rowCount, 1 )->setText( g.start.c_str()     );
    _gridTable->item( rowCount, 2 )->setText( g.end.c_str()      );
    _gridTable->item( rowCount, 3 )->setText( g.value.c_str() );

    ++rowCount;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void RefineGridDialog::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string axis  ( this->_getAxis()                    );
  std::string start ( this->_start->text().toStdString()  );
  std::string end   ( this->_end->text().toStdString()    );
  std::string value ( this->_value->text().toStdString()  );

  GridRefinement gr ( axis, start, end, value );
 
  // add the Source to the list of contamimants
  _refinements.push_back( gr );

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

  // create an item widget for the third column
  QTableWidgetItem *item2 = new QTableWidgetItem;
  item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the fourth column
  QTableWidgetItem *item3 = new QTableWidgetItem;
  item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // set the current item
  _gridTable->setCurrentItem( item0 );

  // insert the columns
  _gridTable->setItem( rowCount, 0, item0 );
  _gridTable->setItem( rowCount, 1, item1 );
  _gridTable->setItem( rowCount, 2, item2 );
  _gridTable->setItem( rowCount, 3, item3 );

  // set the values of the row
  _gridTable->item( rowCount, 0 )->setText( axis.c_str()  );
  _gridTable->item( rowCount, 1 )->setText( start.c_str() );
  _gridTable->item( rowCount, 2 )->setText( end.c_str()   );
  _gridTable->item( rowCount, 3 )->setText( value.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void RefineGridDialog::on_removeButton_clicked()
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
  GridRefinements newlist;

  // old building cracks
  GridRefinements oldlist ( _refinements );

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
  _refinements = newlist;

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

void RefineGridDialog::_clearTable()
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

RefineGridDialog::GridRefinements RefineGridDialog::refinements()
{
  return _refinements;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soils
//
///////////////////////////////////////////////////////////////////////////////

void RefineGridDialog::refinements( GridRefinements gr )
{
  _refinements = gr;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update a Soil
//
///////////////////////////////////////////////////////////////////////////////

void RefineGridDialog::on_updateButton_clicked()
{
  // get the parameters from the text boxes
  std::string axis  ( this->_getAxis()                    );
  std::string start ( this->_start->text().toStdString()  );
  std::string end   ( this->_end->text().toStdString()    );
  std::string value ( this->_value->text().toStdString()  );

  GridRefinement gr ( axis, start, end, value );

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
    _refinements.at( selectedRows.at( i ) ) = gr;
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

std::string RefineGridDialog::_getAxis()
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