
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

#include "ContaminantDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ContaminantDialog::ContaminantDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_contaminants()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Name" );
  titles.push_back( "H. Law" );
  titles.push_back( "Koc" );
  titles.push_back( "Diff in Air" );
  titles.push_back( "Diff in H2O" );
  titles.push_back( "Atmo. Conc." );

  _contaminantTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ContaminantDialog::~ContaminantDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _contaminants.size(); ++i )
  {
    // create a Source object
    Contaminant c ( _contaminants.at( i ) );

    // add a row
    _contaminantTable->insertRow( rowCount );

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

    // create an item widget for the fifth column
    QTableWidgetItem *item4 = new QTableWidgetItem;
    item4->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the sixth column
    QTableWidgetItem *item5 = new QTableWidgetItem;
    item5->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    if( true == firstPass )
    {
      // set the current item
      _contaminantTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _contaminantTable->setItem( rowCount, 0, item0 );
    _contaminantTable->setItem( rowCount, 1, item1 );
    _contaminantTable->setItem( rowCount, 2, item2 );
    _contaminantTable->setItem( rowCount, 3, item3 );
    _contaminantTable->setItem( rowCount, 4, item4 );
    _contaminantTable->setItem( rowCount, 5, item5 ); 

    // set the values of the row
    _contaminantTable->item( rowCount, 0 )->setText( c.name.c_str()     );
    _contaminantTable->item( rowCount, 1 )->setText( c.henry.c_str()     );
    _contaminantTable->item( rowCount, 2 )->setText( c.koc.c_str()      );
    _contaminantTable->item( rowCount, 3 )->setText( c.airDiff.c_str()  );
    _contaminantTable->item( rowCount, 4 )->setText( c.waterDiff.c_str()  );
    _contaminantTable->item( rowCount, 5 )->setText( c.atmoConc.c_str() );

    ++rowCount;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string name        ( this->_name->text().toStdString()      );
  std::string hlaw        ( this->_hlaw->text().toStdString()      );
  std::string koc         ( this->_koc->text().toStdString()       );
  std::string diffAir     ( this->_diffAir->text().toStdString()   );
  std::string diffH2O     ( this->_diffWater->text().toStdString() );
  std::string atmoConc    ( this->_atmoConc->text().toStdString()  );

  Contaminant c ( _contaminants.size(), name, hlaw, koc, diffAir, diffH2O, atmoConc );

  // add the Source to the list of contamimants
  _contaminants.push_back( c );

  int rowCount ( _contaminantTable->rowCount() );

  // add a row
  _contaminantTable->insertRow( rowCount );

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

  // create an item widget for the fifth column
  QTableWidgetItem *item4 = new QTableWidgetItem;
  item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // create an item widget for the sixth column
  QTableWidgetItem *item5 = new QTableWidgetItem;
  item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // set the current item
  _contaminantTable->setCurrentItem( item0 );

  // insert the columns
  _contaminantTable->setItem( rowCount, 0, item0 );
  _contaminantTable->setItem( rowCount, 1, item1 );
  _contaminantTable->setItem( rowCount, 2, item2 );
  _contaminantTable->setItem( rowCount, 3, item3 );
  _contaminantTable->setItem( rowCount, 4, item4 );
  _contaminantTable->setItem( rowCount, 5, item5 ); 

  // set the values of the row
  _contaminantTable->item( rowCount, 0 )->setText( name.c_str()     );
  _contaminantTable->item( rowCount, 1 )->setText( hlaw.c_str()     );
  _contaminantTable->item( rowCount, 2 )->setText( koc.c_str()      );
  _contaminantTable->item( rowCount, 3 )->setText( diffAir.c_str()  );
  _contaminantTable->item( rowCount, 4 )->setText( diffH2O.c_str()  );
  _contaminantTable->item( rowCount, 5 )->setText( atmoConc.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::on_removeButton_clicked()
{
   // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _contaminantTable->selectedItems() );

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
  Contaminants newlist;

  // old building cracks
  Contaminants oldlist ( _contaminants );

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
  _contaminants = newlist;

  // clear the cracks table
  this->_clearTable();

  // repopulate the table
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contaminants
//
///////////////////////////////////////////////////////////////////////////////

ContaminantDialog::Contaminants ContaminantDialog::contaminants()
{
  return _contaminants;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the contaminants table
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::_clearTable()
{
  // remove all the rows
  for( int i = _contaminantTable->rowCount() - 1; i >= 0 ; --i )
  {
    _contaminantTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the contaminants
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::contaminants( Contaminants c )
{
  _contaminants = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update a Contaminant
//
///////////////////////////////////////////////////////////////////////////////

void ContaminantDialog::on_updateButton_clicked()
{
}