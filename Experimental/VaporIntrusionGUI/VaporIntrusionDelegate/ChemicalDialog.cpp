
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

#include "ChemicalDialog.h"
#include "AddChemicalDialog.h"

#include "Usul/Strings/Format.h"
#include "Usul/Exceptions/Canceled.h"

#include <QtGui/QCheckBox>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChemicalDialog::ChemicalDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_chemicals()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Chemical" );
  titles.push_back( "Henry's Constant (cm3/cm3)" );
  titles.push_back( "Koc (cm3/g)" );
  titles.push_back( "Diff in Air (cm2/s)" );
  titles.push_back( "Diff in H2O (cm2/s)" );
  titles.push_back( "Initial Conc. in soil gas (g/cm3)" );
  titles.push_back( "Present" );

  _chemicalTable->setHorizontalHeaderLabels( titles );

  // size the columns
  _chemicalTable->resizeColumnsToContents();

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChemicalDialog::~ChemicalDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _chemicalLibrary.size(); ++i )
  {
    // create a Source object
    Chemical c ( _chemicalLibrary.at( i ) );

    // add a row
    _chemicalTable->insertRow( rowCount );

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

    // create a checkbox widget for the seventh column
    QTableWidgetItem *item6 = new QTableWidgetItem;
    item6->data( Qt::CheckStateRole );
    item6->setCheckState( Qt::Unchecked );

    // if the chemical is present check item6
    for( unsigned int j = 0; j < _chemicals.size(); ++j )
    {
      // get the chemical from the list of chemicals in the experiment
      Chemical cx ( _chemicals.at( j ) );

      // check to see if the name matches that of the current chemical
      if( cx.name == c.name )
      {
        // check the chemical checkbox
        item6->setCheckState( Qt::Checked );
        break;
      }
    }


    if( true == firstPass )
    {
      // set the current item
      _chemicalTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _chemicalTable->setItem( rowCount, 0, item0 );
    _chemicalTable->setItem( rowCount, 1, item1 );
    _chemicalTable->setItem( rowCount, 2, item2 );
    _chemicalTable->setItem( rowCount, 3, item3 );
    _chemicalTable->setItem( rowCount, 4, item4 );
    _chemicalTable->setItem( rowCount, 5, item5 );
    _chemicalTable->setItem( rowCount, 6, item6 );

    // set the values of the row
    _chemicalTable->item( rowCount, 0 )->setText( c.name.c_str()     );
    _chemicalTable->item( rowCount, 1 )->setText( c.henry.c_str()     );
    _chemicalTable->item( rowCount, 2 )->setText( c.koc.c_str()      );
    _chemicalTable->item( rowCount, 3 )->setText( c.airDiff.c_str()  );
    _chemicalTable->item( rowCount, 4 )->setText( c.waterDiff.c_str()  );
    _chemicalTable->item( rowCount, 5 )->setText( c.atmoConc.c_str() );

    ++rowCount;
  }

  // size the columns
  _chemicalTable->resizeColumnsToContents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::on_addButton_clicked()
{
  // create an instance of AddChemicalDialog to handle the request
  AddChemicalDialog dialog;

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

  // get the chemical
  Chemical c ( dialog.createChemical() );

  // set the proper chemical index
  c.index = _chemicals.size();

  // add the Source to the list of contamimants
  _chemicals.push_back( c );
  _chemicalLibrary.push_back( c );

  int rowCount ( _chemicalTable->rowCount() );

  // add a row
  _chemicalTable->insertRow( rowCount );

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

  // create a checkbox widget for the seventh column
  QTableWidgetItem *item6 = new QTableWidgetItem;
  item6->data( Qt::CheckStateRole );
  item6->setCheckState( Qt::Checked );

  // set the current item
  _chemicalTable->setCurrentItem( item0 );

  // insert the columns
  _chemicalTable->setItem( rowCount, 0, item0 );
  _chemicalTable->setItem( rowCount, 1, item1 );
  _chemicalTable->setItem( rowCount, 2, item2 );
  _chemicalTable->setItem( rowCount, 3, item3 );
  _chemicalTable->setItem( rowCount, 4, item4 );
  _chemicalTable->setItem( rowCount, 5, item5 ); 
  _chemicalTable->setItem( rowCount, 6, item6 ); 

  // set the values of the row
  _chemicalTable->item( rowCount, 0 )->setText( c.name.c_str()     );
  _chemicalTable->item( rowCount, 1 )->setText( c.henry.c_str()     );
  _chemicalTable->item( rowCount, 2 )->setText( c.koc.c_str()      );
  _chemicalTable->item( rowCount, 3 )->setText( c.airDiff.c_str()  );
  _chemicalTable->item( rowCount, 4 )->setText( c.waterDiff.c_str()  );
  _chemicalTable->item( rowCount, 5 )->setText( c.atmoConc.c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the library
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::library( Chemicals l )
{
  _chemicalLibrary = l;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the library
//
///////////////////////////////////////////////////////////////////////////////

ChemicalDialog::Chemicals ChemicalDialog::library()
{
  return _chemicalLibrary;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the chemicals specified by the user that will be present in the 
//  experiment space.
//
///////////////////////////////////////////////////////////////////////////////

ChemicalDialog::Chemicals ChemicalDialog::getSelectedChemicals()
{
  // Chemicals to return
  Chemicals chemicals;

  for( int i = 0; i < _chemicalTable->rowCount(); ++ i )
  {
    // get the checkbox item at row i
    QTableWidgetItem* item ( _chemicalTable->item( i, 6 ) );

    // get the value of the check box
    Qt::CheckState checked ( item->checkState() );

    // if it is checked add it to the list of chemicals to return
    if( Qt::Checked == checked )
    {
      // get the chemical at this index
      Chemical c ( _chemicalLibrary.at( i ) );

      // add the chemical to the list of those to return
      chemicals.push_back( c );
    }
  }

  return chemicals;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::on_removeButton_clicked()
{
   // get the currently selected chemicals
  QList<QTableWidgetItem*> selectedItems ( _chemicalTable->selectedItems() );

  std::vector< unsigned int > rowsToRemove;

  // loop through the selected chemicals
  for( int i = 0; i < selectedItems.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // add to the rows to remove list
    rowsToRemove.push_back( row );
  }

  // remove from the active chemicals
  {
    // new building cracks
    Chemicals newlist;

    // old building cracks
    Chemicals oldlist ( _chemicals );

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
    _chemicals = newlist;
  } // end active chemical removal
  
  // remove from the chemical library
  {
    // new building cracks
    Chemicals newlist;

    // old building cracks
    Chemicals oldlist ( _chemicalLibrary );

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
    _chemicalLibrary = newlist;
  } // end library removal

  // clear the cracks table
  this->_clearTable();

  // repopulate the table
  this->_initialize();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the chemicals table
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::_clearTable()
{
  // remove all the rows
  for( int i = _chemicalTable->rowCount() - 1; i >= 0 ; --i )
  {
    _chemicalTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the chemicals
//
///////////////////////////////////////////////////////////////////////////////

ChemicalDialog::Chemicals ChemicalDialog::chemicals()
{
  return _chemicals;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the chemicals
//
///////////////////////////////////////////////////////////////////////////////

void ChemicalDialog::chemicals( Chemicals c )
{
  _chemicals = c;
}
