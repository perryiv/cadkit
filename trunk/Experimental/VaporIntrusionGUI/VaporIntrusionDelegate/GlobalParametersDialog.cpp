
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

#include "GlobalParametersDialog.h"

#include "Usul/Strings/Format.h"
#include "Usul/Exceptions/Canceled.h"

#include <QtGui/QCheckBox>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GlobalParametersDialog::GlobalParametersDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_chemicals()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Chemical" );
	titles.push_back( "Ambient Vapor Concentration" );
	titles.push_back( "Indoor Air Concentration" );

  _oxygenCO2Table->setHorizontalHeaderLabels( titles );

  // size the columns
  _oxygenCO2Table->resizeColumnsToContents();

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GlobalParametersDialog::~GlobalParametersDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void GlobalParametersDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void GlobalParametersDialog::_initialize()
{
  // Oxygen
  Chemical oxygen ( _chemicals.first );

	// Carbon Dioxide
	Chemical co2 ( _chemicals.second );

  // add a row
  _oxygenCO2Table->insertRow( 0 );
	_oxygenCO2Table->insertRow( 1 );

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

  // set the current item
  _oxygenCO2Table->setCurrentItem( item0 );

  // insert the columns
  _oxygenCO2Table->setItem( 0, 0, item0 );
  _oxygenCO2Table->setItem( 0, 1, item1 );
  _oxygenCO2Table->setItem( 0, 2, item2 );

  _oxygenCO2Table->setItem( 1, 0, item3 );
  _oxygenCO2Table->setItem( 1, 1, item4 );
  _oxygenCO2Table->setItem( 1, 2, item5 );

  // set the values of the row
  _oxygenCO2Table->item( 0, 0 )->setText( oxygen.name.c_str()     );
  _oxygenCO2Table->item( 0, 1 )->setText( oxygen.ambient.c_str()     );
  _oxygenCO2Table->item( 0, 2 )->setText( oxygen.indoor.c_str()      );

  _oxygenCO2Table->item( 1, 0 )->setText( co2.name.c_str()  );
  _oxygenCO2Table->item( 1, 1 )->setText( co2.ambient.c_str()  );
  _oxygenCO2Table->item( 1, 2 )->setText( co2.indoor.c_str() );

  // size the columns
  _oxygenCO2Table->resizeColumnsToContents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the chemicals
//
///////////////////////////////////////////////////////////////////////////////

GlobalParametersDialog::ChemicalPair GlobalParametersDialog::chemicals()
{
	this->finalize();

  return _chemicals;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the chemicals table
//
///////////////////////////////////////////////////////////////////////////////

void GlobalParametersDialog::_clearTable()
{
  // remove all the rows
  for( int i = _oxygenCO2Table->rowCount() - 1; i >= 0 ; --i )
  {
    _oxygenCO2Table->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the chemicals
//
///////////////////////////////////////////////////////////////////////////////

void GlobalParametersDialog::chemicals( ChemicalPair c )
{
  _chemicals = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize user input
//
///////////////////////////////////////////////////////////////////////////////

void GlobalParametersDialog::finalize()
{

}

