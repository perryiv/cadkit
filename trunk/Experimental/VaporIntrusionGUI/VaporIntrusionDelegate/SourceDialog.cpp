
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

#include "SourceDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SourceDialog::SourceDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Name" );
  titles.push_back( "Location" );
  titles.push_back( "Size" );
  titles.push_back( "Contaminants" );

  _sourceTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SourceDialog::~SourceDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Sources.
//
///////////////////////////////////////////////////////////////////////////////

SourceDialog::Sources SourceDialog::sources()
{
  return _sources;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Sources.
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::sources( Sources s )
{
  _sources = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _sources.size(); ++i )
  {
    // create a Source object
    Source s ( _sources.at( i ) );

    // add a row
    _sourceTable->insertRow( rowCount );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the third column
    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the third column
    QTableWidgetItem *item3 = new QTableWidgetItem;
    item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    if( true == firstPass )
    {
      // set the current item
      _sourceTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _sourceTable->setItem( rowCount, 0, item0 );
    _sourceTable->setItem( rowCount, 1, item1 );
    _sourceTable->setItem( rowCount, 2, item2 );
    _sourceTable->setItem( rowCount, 3, item3 );
  
    // get the source contaminants
    Contaminants sc( _sources.at( i ).contaminants );

   // string to hold the contaminant data
    std::string scstr ("( ");

    // loop through the already assigned contaminants
    for( unsigned int j = 0; j < sc.size(); ++j )
    {
      // get the contaminant
      std::string cname ( sc.at( j ).name );
      
      if( j < sc.size() - 1 )
      {
        // if this isn't the last element add a comma after the contaminant name
        scstr = Usul::Strings::format( scstr, cname, ", " );
      }
      else
      {
        // omit comma if this is the last element
        scstr = Usul::Strings::format( scstr, cname );
      }
    }

    // closed the ()
    scstr = Usul::Strings::format( scstr, " )" );

    // set the values of the row
    _sourceTable->item( rowCount, 0 )->setText( s.name.c_str() );
    _sourceTable->item( rowCount, 1 )->setText( Usul::Strings::format( "( ", s.x, ", ", s.y, ", ", s.z, " )" ).c_str() );
    _sourceTable->item( rowCount, 2 )->setText( Usul::Strings::format( "( ", s.l, ", ", s.w, ", ", s.h, " )" ).c_str() );
    _sourceTable->item( rowCount, 3 )->setText( scstr.c_str() );


    ++rowCount;
  }
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a Source
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::on_addButton_clicked()
{
  // get the parameters from the text boxes
  std::string l    ( this->_length->text().toStdString()        );
  std::string w    ( this->_width->text().toStdString()         );
  std::string h    ( this->_height->text().toStdString()        );
  std::string x    ( this->_xpos->text().toStdString()          );
  std::string y    ( this->_ypos->text().toStdString()          );
  std::string z    ( this->_zpos->text().toStdString()          );
  std::string name ( this->_name->text().toStdString()          );

  Contaminants c;

  // create a Source object
  Source s ( l, w, h, x, y, z, name, c );
  
  // add the Source to the list of contamimants
  _sources.push_back( s );

  int rowCount ( _sourceTable->rowCount() );

  // add a row
  _sourceTable->insertRow( rowCount );

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
  _sourceTable->setCurrentItem( item0 );

  // insert the columns
  _sourceTable->setItem( rowCount, 0, item0 );
  _sourceTable->setItem( rowCount, 1, item1 );
  _sourceTable->setItem( rowCount, 2, item2 );

  // set the values of the row
  _sourceTable->item( rowCount, 0 )->setText( name.c_str() );
  _sourceTable->item( rowCount, 1 )->setText( Usul::Strings::format( "( ", x, ", ", y, ", ", z, " )" ).c_str() );
  _sourceTable->item( rowCount, 2 )->setText( Usul::Strings::format( "( ", l, ", ", w, ", ", h, " )" ).c_str() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a Source
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::on_removeButton_clicked()
{
     // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedItems ( _sourceTable->selectedItems() );

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
  Sources newlist;

  // old building cracks
  Sources oldlist ( _sources );

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
  _sources = newlist;

  // clear the cracks table
  this->_clearTable();

  // repopulate the table
  this->_initialize();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the sources table
//
///////////////////////////////////////////////////////////////////////////////

void SourceDialog::_clearTable()
{
  // remove all the rows
  for( int i = _sourceTable->rowCount() - 1; i >= 0 ; --i )
  {
    _sourceTable->removeRow( i );
  }
}