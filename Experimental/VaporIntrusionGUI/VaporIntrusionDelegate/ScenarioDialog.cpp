
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

#include "ScenarioDialog.h"
#include "Usul/Strings/Format.h"
#include "Usul/Exceptions/Canceled.h"

#include <QtGui/QCheckBox>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ScenarioDialog::ScenarioDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_scenario()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Parameter" );
  titles.push_back( "Option" );

  _scenarioTable->setHorizontalHeaderLabels( titles );

  // size the columns
  _scenarioTable->resizeColumnsToContents();

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ScenarioDialog::~ScenarioDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void ScenarioDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void ScenarioDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _scenario.size(); ++i )
  {
    // create a Source object
    ScenarioEntry s ( _scenario.at( i ) );

    // add a row
    _scenarioTable->insertRow( rowCount );

     // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter ); 

		// insert the columns
    _scenarioTable->setItem( rowCount, 0, item0 );

    // set the values of the row
    _scenarioTable->item( rowCount, 0 )->setText( s.question.c_str()	);

		if( s.type == IVPI::SCENARIO_TYPE_DROP_DOWN )
		{
			QComboBox *typeBox = new QComboBox;

			for( unsigned int i = 0; i < s.options.size(); ++i )
			{
				typeBox->addItem( s.options.at( i ).c_str() );
				
			}

			// set the currently selected option
			typeBox->setCurrentIndex( s.selectedOption );

			// insert the columns
		  _scenarioTable->setCellWidget( rowCount, 1, typeBox );
		}
		else
		{
			// create an item widget for the first column
			QTableWidgetItem *item1 = new QTableWidgetItem;
			item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter ); 

			// insert the columns
			_scenarioTable->setItem( rowCount, 1, item1 );

			// set the values of the row
			_scenarioTable->item( rowCount, 1 )->setText( s.value.c_str()	);

		}

    if( true == firstPass )
    {
      // set the current item
      _scenarioTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    ++rowCount;
  }

  // size the columns
  _scenarioTable->resizeColumnsToContents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the scenario table
//
///////////////////////////////////////////////////////////////////////////////

void ScenarioDialog::_clearTable()
{
  // remove all the rows
  for( int i = _scenarioTable->rowCount() - 1; i >= 0 ; --i )
  {
    _scenarioTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scenario
//
///////////////////////////////////////////////////////////////////////////////

ScenarioDialog::Scenario ScenarioDialog::scenario()
{
  return _scenario;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scenario
//
///////////////////////////////////////////////////////////////////////////////

void ScenarioDialog::scenario( Scenario c )
{
  _scenario = c;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Finalize table items and setup _scenario member for retrieval
//
///////////////////////////////////////////////////////////////////////////////

void ScenarioDialog::finalize()
{
 
  // set the chemical concentrations
  for( unsigned int i = 0; i < _scenario.size(); ++i )
  {
		// get the current scenerio entry
		IVPI::VPIScenarioEntry entry ( _scenario.at( i ) );

		std::string value ( entry.value );

		if( entry.type == IVPI::SCENARIO_TYPE_DROP_DOWN )
		{
			QComboBox *typeBox = new QComboBox;

			typeBox = reinterpret_cast< QComboBox* > (  _scenarioTable->cellWidget( i, 1 ) );

			// get table entries for this index
			entry.selectedOption = typeBox->currentIndex();
		}
		else
		{
			// get table entries for this index
			value = _scenarioTable->item( i, 1 )->text().toStdString();
		}
    
		// set the value
		entry.value = value;

    _scenario.at( i ) = entry;
  }
}