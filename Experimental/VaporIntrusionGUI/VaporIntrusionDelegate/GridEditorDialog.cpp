
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

#include "GridEditorDialog.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GridEditorDialog::GridEditorDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_xValues(),
_yValues(),
_zValues()
{
  // Initialize code from Designer.
  this->setupUi ( this );

  QStringList titles;
  titles.push_back( "Position" );
  titles.push_back( "Width");

  _xList->setHorizontalHeaderLabels( titles );
  _yList->setHorizontalHeaderLabels( titles );
  _zList->setHorizontalHeaderLabels( titles );

  // initialize the grids
  this->_initializeGridData();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GridEditorDialog::~GridEditorDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the values in the xyz grids
//
///////////////////////////////////////////////////////////////////////////////

void GridEditorDialog::_initializeGridData()
{
  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

   // clear out any existing grid data
  _xList->clear();
  _yList->clear();
  _zList->clear();

  // get the x values
  _xValues = document->getXGrid();

  // set the Grid values
  for( unsigned int i = 0; i < _xValues.size(); ++i )
  {
    // insert a row
    _xList->insertRow( i );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
    
    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _xList->setItem( i, 0, item0 );
    _xList->setItem( i, 1, item1 );

    // set the current item
    _xList->setCurrentItem( item0 );

    // set the values of the row
    _xList->item( i, 0 )->setText( QString::number( _xValues.at( i ).first ) );
    _xList->item( i, 1 )->setText( QString::number( _xValues.at( i ).second ) );
  }

  // get the y values
  _yValues = document->getYGrid();

  // set the Grid values
  for( unsigned int j = 0; j < _yValues.size(); ++j )
  {
    // insert a row
    _yList->insertRow( j );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
    
    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _yList->setItem( j, 0, item0 );
    _yList->setItem( j, 1, item1 );

    // set the current item
    _yList->setCurrentItem( item0 );

    // set the values of the row
    _yList->item( j, 0 )->setText( QString::number( _yValues.at( j ).first ) );
    _yList->item( j, 1 )->setText( QString::number( _yValues.at( j ).second ) );
  }

  // get the z values
  _zValues = document->getZGrid();

  // set the Grid values
  for( unsigned int k = 0; k < _zValues.size(); ++k )
  {
    // insert a row
    _zList->insertRow( k );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
    
    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _zList->setItem( k, 0, item0 );
    _zList->setItem( k, 1, item1 );

    // set the current item
    _zList->setCurrentItem( item0 );

    // set the values of the row
    _zList->item( k, 0 )->setText( QString::number( _zValues.at( k ).first ) );
    _zList->item( k, 1 )->setText( QString::number( _zValues.at( k ).second ) );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the values in the xyz grids
//
///////////////////////////////////////////////////////////////////////////////

void GridEditorDialog::on_resetButton_clicked()
{
  // set the Grid values
  for( unsigned int i = 0; i < _xValues.size(); ++i )
  {
    // set the values of the row
    _xList->item( i, 0 )->setText( QString::number( _xValues.at( i ).first ) );
    _xList->item( i, 1 )->setText( QString::number( _xValues.at( i ).second ) );
  }

  // set the Grid values
  for( unsigned int j = 0; j < _yValues.size(); ++j )
  {
    // set the values of the row
    _yList->item( j, 0 )->setText( QString::number( _yValues.at( j ).first ) );
    _yList->item( j, 1 )->setText( QString::number( _yValues.at( j ).second ) );
  }

  // set the Grid values
  for( unsigned int k = 0; k < _zValues.size(); ++k )
  {
    // set the values of the row
    _zList->item( k, 0 )->setText( QString::number( _zValues.at( k ).first ) );
    _zList->item( k, 1 )->setText( QString::number( _zValues.at( k ).second ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the values in the xyz grids
//
///////////////////////////////////////////////////////////////////////////////

void GridEditorDialog::on_applyButton_clicked()
{
  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;
  
  // Set the x values
  if( _xValues.size() > 0 )
  {
    // set the position of the first value
    _xValues.at( 0 ).first = static_cast< double > ( _xList->item( 0, 0 )->text().toDouble() );

    // set the width of the first value
    _xValues.at( 0 ).second = static_cast< double > ( _xList->item( 0, 1 )->text().toDouble() );
  }

  // set the x values back into the grid
  for( int row = 1; row < _xList->rowCount(); ++ row )
  {
    // get the previous position
    double prevPos ( _xValues.at( row - 1 ).first  );

    // get the previous width
    double prevWidth( _xValues.at( row - 1 ).second );

    // get the current width
    double currWidth( _xList->item( row, 1 )->text().toDouble() );

    // set the current position
    double currPos( prevPos + prevWidth );

    // set the new position and width
    _xValues.at( row ).first = currPos;
    _xValues.at( row ).second = currWidth;
  }

  //-------------------------------------------------------

  // Set the y values
  if( _yValues.size() > 0 )
  {
    // set the first value
    _yValues.at( 0 ).second = static_cast< double > ( _yList->item( 0, 1 )->text().toDouble() );

  }

  // set the x values back into the grid
  for( int row = 1; row < _yList->rowCount(); ++ row )
  {
    // get the previous position
    double prevPos ( _yValues.at( row - 1 ).first  );

    // get the previous width
    double prevWidth( _yValues.at( row - 1 ).second );

    // get the current width
    double currWidth( _yList->item( row, 1 )->text().toDouble() );

    // set the current position
    double currPos( prevPos + prevWidth );

    // set the new position and width
    _yValues.at( row ).first = currPos;
    _yValues.at( row ).second = currWidth;
  }

  //--------------------------------------------------------------

  // Set the z values
  if( _zValues.size() > 0 )
  {
    // set the first value
    _zValues.at( 0 ).second = static_cast< double > ( _zList->item( 0, 1 )->text().toDouble() );

  }

  // set the x values back into the grid
  for( int row = 1; row < _zList->rowCount(); ++ row )
  {
    // get the previous position
    double prevPos ( _zValues.at( row - 1 ).first  );

    // get the previous width
    double prevWidth( _zValues.at( row - 1 ).second );

    // get the current width
    double currWidth( _zList->item( row, 1 )->text().toDouble() );

    // set the current position
    double currPos( prevPos + prevWidth );

    // set the new position and width
    _zValues.at( row ).first = currPos;
    _zValues.at( row ).second = currWidth;
  }

  // reset the values to the new ones
  this->on_resetButton_clicked();

  // set the values back into the document
  document->setXGrid( _xValues );
  document->setYGrid( _yValues );
  document->setZGrid( _zValues );

  // rebuild the scene
  document->rebuildScene();
}


