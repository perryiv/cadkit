
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

#include "InputParameterDialog.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InputParameterDialog::InputParameterDialog ( Usul::Interfaces::IVaporIntrusionGUI::Category category, QWidget *parent ) : 
BaseClass ( parent ),
_category( category )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  std::vector< std::string > header ( _category.columnNames );

  QStringList titles;
  for( unsigned int i = 0; i < header.size(); ++i )
  {
    //_inputList->insertColumn( i );

    std::string colName ( header.at( i ) );

    titles.push_back( colName.c_str() );
  }

  _inputList->setHorizontalHeaderLabels( titles );

  this->_initializeList();
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InputParameterDialog::~InputParameterDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the input list.
//
///////////////////////////////////////////////////////////////////////////////

void InputParameterDialog::_initializeList ()
{
  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // the current row count
  unsigned int rowCount( 0 );

  // initial condition check
  bool firstpass ( true );

  // add the columns to the dialog
  for( InputColumns::iterator column = _category.columns.begin(); 
       column != _category.columns.end(); 
       ++column )
  {
    // verify that the column is active
    bool isActive( true );

    // check the activators
    if( (*column).second.activators.size() > 0 )
    {
      for( ActivatorPairs::iterator aPairs = (*column).second.activators.begin();
           aPairs != (*column).second.activators.end();
           ++aPairs )
      {
        // get the activator name
        std::string name ( (*aPairs).first );

        // get the expected activator value
        std::string value ( (*aPairs).second.second );

        // get the current activator value
        std::string aCurrentValue ( _category.columns[ name ].value );

        // get the comparitor
        int aComp ( (*aPairs).second.first );

        // check the activation value
        if( false == document->checkValue( aComp, aCurrentValue, value ) )
        {
          // if the values don't agree then de-activate this row
          isActive = false;

          // stop checking
          break;
        }
      }
    }    

    if( true == isActive )
    {
      // insert a row
      _inputList->insertRow( rowCount );

      // create an item widget for the first column
      QTableWidgetItem *item0 = new QTableWidgetItem;
      item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

      // create an item widget for the second column
      QTableWidgetItem *item1 = new QTableWidgetItem;
      item1->setTextAlignment( Qt::AlignCenter | Qt::AlignVCenter );

      // create an item widget for the third column
      QTableWidgetItem *item2 = new QTableWidgetItem;
      item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

      if( true == firstpass )
      {
        // set the current item
        _inputList->setCurrentItem( item0 );

        // no longer the first time through
        firstpass = false;
      }    

      // insert the columns
      _inputList->setItem( rowCount, 0, item0 );
      _inputList->setItem( rowCount, 1, item1 );
      _inputList->setItem( rowCount, 2, item2 );

      // set the name
      QString name ( (*column).second.name.c_str() );
      _inputList->item( rowCount, 0 )->setText( name );

      // set the value
      std::string value( (*column).second.value );
      _inputList->item( rowCount, 1 )->setText( value.c_str() );

      // set the description
      std::string description( (*column).second.description );
      _inputList->item( rowCount, 2 )->setText( description.c_str() );

      // increment the row count
      ++rowCount;
    }

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply button was pushed
//
///////////////////////////////////////////////////////////////////////////////

void InputParameterDialog::on_resetButton_clicked()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply button was pushed
//
///////////////////////////////////////////////////////////////////////////////

void InputParameterDialog::on_applyButton_clicked()
{
  
  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  for( int row = 0; row < _inputList->rowCount(); ++ row )
  {
    // get the name
    std::string name ( _inputList->item( row, 0 )->text().toStdString() );

    // get the value
    std::string value ( _inputList->item( row, 1 )->text().toStdString() );

    // get the column
    InputColumn column ( _category.columns[ name ] );

    // change the value
    column.value = value;

    // set the column
    _category.columns[ name ] = column;

  }

  // update the category in the document
  document->updateCategory( _category );

  // remove all the rows
  for( int i = _inputList->rowCount() - 1; i >= 0 ; --i )
  {
    _inputList->removeRow( i );
  }
  
  // reinitialize the list
  this->_initializeList();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply button was pushed
//
///////////////////////////////////////////////////////////////////////////////

void InputParameterDialog::okClicked()
{
  this->on_applyButton_clicked();
}