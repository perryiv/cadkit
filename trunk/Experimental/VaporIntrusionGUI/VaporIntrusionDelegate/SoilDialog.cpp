
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

#include "SoilDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SoilDialog::SoilDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_soils()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Name" );
  titles.push_back( "Elevation" );
  titles.push_back( "Porosity" );
  titles.push_back( "H2O Porosity" );
  titles.push_back( "Organic Carbon" );
  titles.push_back( "Permeability" );
  titles.push_back( "Viscosity" );
  titles.push_back( "Color" );

  _soilTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SoilDialog::~SoilDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void SoilDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void SoilDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _soils.size(); ++i )
  {
    // create a Source object
    Soil s ( _soils.at( i ) );

    // add a row
    _soilTable->insertRow( rowCount );

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

    // create an item widget for the sixth column
    QTableWidgetItem *item6 = new QTableWidgetItem;
    item6->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the sixth column
    QTableWidgetItem *item7 = new QTableWidgetItem;
    item7->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    if( true == firstPass )
    {
      // set the current item
      _soilTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _soilTable->setItem( rowCount, 0, item0 );
    _soilTable->setItem( rowCount, 1, item1 );
    _soilTable->setItem( rowCount, 2, item2 );
    _soilTable->setItem( rowCount, 3, item3 );
    _soilTable->setItem( rowCount, 4, item4 );
    _soilTable->setItem( rowCount, 5, item5 ); 
    _soilTable->setItem( rowCount, 6, item6 ); 

    // set the values of the row
    _soilTable->item( rowCount, 0 )->setText( s.name.c_str()          );
    _soilTable->item( rowCount, 1 )->setText( s.elevation.c_str()     );
    _soilTable->item( rowCount, 2 )->setText( s.porosity.c_str()      );
    _soilTable->item( rowCount, 3 )->setText( s.waterPorosity.c_str() );
    _soilTable->item( rowCount, 4 )->setText( s.organicCarbon.c_str() );
    _soilTable->item( rowCount, 5 )->setText( s.permeability.c_str()  );
    _soilTable->item( rowCount, 6 )->setText( s.viscosity.c_str()     );

    ++rowCount;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void SoilDialog::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string name        ( this->_name->text().toStdString()           );
  std::string elevation   ( this->_elevation->text().toStdString()      );
  std::string porosity    ( this->_totalPorosity->text().toStdString()  );
  std::string h2oPorosity ( this->_waterPorosity->text().toStdString()  );
  std::string organicC    ( this->_organicCarbon->text().toStdString()  );
  std::string perm        ( this->_permeability->text().toStdString()   );
  std::string viscosity   ( this->_viscosity->text().toStdString()      );

  Soil c ( name, elevation, porosity, h2oPorosity, organicC, perm, viscosity );

  // add the Source to the list of contamimants
  _soils.push_back( c );

  // increment the row count
  int rowCount ( _soilTable->rowCount() );

  // add a row
  _soilTable->insertRow( rowCount );

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

  // create an item widget for the sixth column
  QTableWidgetItem *item6 = new QTableWidgetItem;
  item6->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  // set the current item
  _soilTable->setCurrentItem( item0 );

  // insert the columns
  _soilTable->setItem( rowCount, 0, item0 );
  _soilTable->setItem( rowCount, 1, item1 );
  _soilTable->setItem( rowCount, 2, item2 );
  _soilTable->setItem( rowCount, 3, item3 );
  _soilTable->setItem( rowCount, 4, item4 );
  _soilTable->setItem( rowCount, 5, item5 ); 
  _soilTable->setItem( rowCount, 6, item6 ); 

  // set the values of the row
  _soilTable->item( rowCount, 0 )->setText( name.c_str()        );
  _soilTable->item( rowCount, 1 )->setText( elevation.c_str()   );
  _soilTable->item( rowCount, 2 )->setText( porosity.c_str()    );
  _soilTable->item( rowCount, 3 )->setText( h2oPorosity.c_str() );
  _soilTable->item( rowCount, 4 )->setText( organicC.c_str()    );
  _soilTable->item( rowCount, 5 )->setText( perm.c_str()        );
  _soilTable->item( rowCount, 6 )->setText( viscosity.c_str()   );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void SoilDialog::on_removeButton_clicked()
{
     // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _soilTable->selectedItems() );

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
  Soils newlist;

  // old building cracks
  Soils oldlist ( _soils );

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
  _soils = newlist;

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

void SoilDialog::_clearTable()
{
  // remove all the rows
  for( int i = _soilTable->rowCount() - 1; i >= 0 ; --i )
  {
    _soilTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Soils
//
///////////////////////////////////////////////////////////////////////////////

SoilDialog::Soils SoilDialog::soils()
{
  return _soils;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soils
//
///////////////////////////////////////////////////////////////////////////////

void SoilDialog::soils( Soils s )
{
  _soils = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update a Soil
//
///////////////////////////////////////////////////////////////////////////////

void SoilDialog::on_updateButton_clicked()
{
}