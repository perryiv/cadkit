
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
  // flush the list
  _inputList->clear();

  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // the current row count
  unsigned int rowCount( 0 );

  // set the first item so focus can be set to the top of the list
  if( _category.activators.size() > 0 )
  {
    // insert a row
    _inputList->insertRow( rowCount );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // create an item widget for the third column
    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _inputList->setItem( rowCount, 0, item0 );
    _inputList->setItem( rowCount, 1, item1 );
    _inputList->setItem( rowCount, 2, item2 );

    // set the current item
    _inputList->setCurrentItem( item0 );

    // set the name
    QString name ( _category.activators.at( 0 ).name.c_str() );
    _inputList->item( rowCount, 0 )->setText( name );

    // set the value
    std::string value( _category.activators.at( 0 ).value );
    _inputList->item( rowCount, 1 )->setText( value.c_str() );

    // set the description
    std::string description( _category.activators.at( 0 ).description );
    _inputList->item( rowCount, 2 )->setText( description.c_str() );

    // increment the row count
    ++rowCount;

    for( unsigned int j = 0; j < _category.activatees.size(); ++j )
    {
      if( _category.activatees.at( j ).activatedBy == _category.activators.at( 0 ).name )
      {
        // insert a row
        _inputList->insertRow( rowCount );

        std::vector< QTableWidgetItem * > subItems;
        for( unsigned int k = 0; k < 3; ++k )
        {
          // create an item widget for the first column
          QTableWidgetItem *item = new QTableWidgetItem;
          item->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

          // set the item in this row
          _inputList->setItem( rowCount, k, item );
        }

        // update the text of the row
        _inputList->item( rowCount, 0 )->setText( _category.activatees.at( j ).name.c_str() );
        _inputList->item( rowCount, 1 )->setText( _category.activatees.at( j ).value.c_str() );
        _inputList->item( rowCount, 2 )->setText( _category.activatees.at( j ).description.c_str() );

        // increment the row count
        ++rowCount;

      }// end internal if

    }// end for loop

  }// end initial if

  for( unsigned int i = 1; i < _category.activators.size(); ++i )
  {
    // insert a row
    _inputList->insertRow( rowCount );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // create an item widget for the third column
    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _inputList->setItem( rowCount, 0, item0 );
    _inputList->setItem( rowCount, 1, item1 );
    _inputList->setItem( rowCount, 2, item2 );

    // set the values of the row
    _inputList->item( rowCount, 0 )->setText( _category.activators.at( i ).name.c_str() );
    _inputList->item( rowCount, 1 )->setText( _category.activators.at( i ).value.c_str() );
    _inputList->item( rowCount, 2 )->setText( _category.activators.at( i ).description.c_str() );

    // increment the row count
    ++rowCount;

    for( unsigned int j = 0; j < _category.activatees.size(); ++j )
    {
      if( _category.activatees.at( j ).activatedBy == _category.activators.at( i ).name )
      {
        // insert a row
       _inputList->insertRow( rowCount );

        std::vector< QTableWidgetItem * > subItems;
        for( unsigned int k = 0; k < 3; ++k )
        {
          // create an item widget for the first column
          QTableWidgetItem *item = new QTableWidgetItem;
          item->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

          // set the item in this row
          _inputList->setItem( rowCount, k, item );
        }

        // update the text of the row
        _inputList->item( rowCount, 0 )->setText( _category.activatees.at( j ).name.c_str() );
        _inputList->item( rowCount, 1 )->setText( _category.activatees.at( j ).value.c_str() );
        _inputList->item( rowCount, 2 )->setText( _category.activatees.at( j ).description.c_str() );

        // increment the row count
        ++rowCount;

      }// end internal if

    }// end for loop

  }
  

#if 0
  // get the current input parameters
  Usul::Interfaces::IVaporIntrusionGUI::ParameterList plist ( document->parameters() );

  // set the Grid values
  for( unsigned int i = 0; i < plist.size(); ++i )
  {
    // insert a row
    _inputList->insertRow( i );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
    
    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _inputList->setItem( i, 0, item0 );
    _inputList->setItem( i, 1, item1 );

    // set the current item
    _inputList->setCurrentItem( item0 );

    // set the values of the row
    _inputList->item( i, 0 )->setText( plist.at( i ).second.first.c_str() );
    _inputList->item( i, 1 )->setText( plist.at( i ).second.second.c_str() );
  }
#endif
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
//  Reset button was pushed
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

    bool checkNext ( true );

    // find and update the value
    for( unsigned int j = 0; j < _category.activatees.size(); ++j )
    {
      // get the name of the category entry
      std::string catName ( _category.activatees.at( j ).name );

      // check for match
      if( name == catName )
      {
        // update the value
        _category.activatees.at( j ).value = value;

        // don't check the activators
        checkNext = false;

        // leave the for loop
        break;
      }
    }//  end of activatees for loop

    // if we are supposed to check the activators
    if( true == checkNext )
    {
      // find and update the value
      for( unsigned int j = 0; j < _category.activators.size(); ++j )
      {
        // get the name of the category entry
        std::string catName ( _category.activators.at( j ).name );

        // check for match
        if( name == catName )
        {
          // update the value
          _category.activators.at( j ).value = value;

          // don't check the activators
          checkNext = false;

          // leave the for loop
          break;
        }
      }// end of activators for loop
    }// end of if
  }// end of category for loop

  // update the category in the document
  document->updateCategory( _category );

  this->_initializeList();

  //// get the parameter list from the document
  //Usul::Interfaces::IVaporIntrusionGUI::ParameterList pList ( document->parameters() );

  //for( int row = 0; row < _inputList->rowCount(); ++ row )
  //{
  //  // get the value from the list
  //  std::string value ( _inputList->item( row, 0 )->text().toStdString() );

  //  // set the new value
  //  pList.at( row ).second.first = value;
  //}

  //// update the parameter list on the document
  //document->parameters( pList );
}