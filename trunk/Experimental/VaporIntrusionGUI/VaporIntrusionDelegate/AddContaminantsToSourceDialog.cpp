
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

#include "AddContaminantsToSourceDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddContaminantsToSourceDialog::AddContaminantsToSourceDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  // create and set the source titles
  QStringList sourceTitles;

  sourceTitles.push_back( "Name" );
  sourceTitles.push_back( "Contaminants" );

  _sourceTable->setHorizontalHeaderLabels( sourceTitles );

  // create and set the contaminant titles
  QStringList contaminantTitles;

  contaminantTitles.push_back( "Name" );
  contaminantTitles.push_back( "H. Law" );
  contaminantTitles.push_back( "Koc" );
  contaminantTitles.push_back( "Diff in Air" );
  contaminantTitles.push_back( "Diff in H2O" );
  contaminantTitles.push_back( "Atmo. Conc." );

  _contaminantTable->setHorizontalHeaderLabels( contaminantTitles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddContaminantsToSourceDialog::~AddContaminantsToSourceDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Sources.
//
///////////////////////////////////////////////////////////////////////////////

AddContaminantsToSourceDialog::Sources AddContaminantsToSourceDialog::sources()
{
  return _sources;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Sources.
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::sources( Sources s )
{
  _sources = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contaminants
//
///////////////////////////////////////////////////////////////////////////////

AddContaminantsToSourceDialog::Contaminants AddContaminantsToSourceDialog::contaminants()
{
  return _contaminants;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the contaminants
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::contaminants( Contaminants c )
{
  _contaminants = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization of sources.
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::_initSource()
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

    // get the source contaminants
    Contaminants sc( s.contaminants );

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
    _sourceTable->item( rowCount, 1 )->setText( scstr.c_str() );

    // increment row count
    ++rowCount;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization of contaminants.
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::_initContaminants()
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
    item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the sixth column
    QTableWidgetItem *item5 = new QTableWidgetItem;
    item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

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
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::_initialize()
{
  // initialize the sources
  this->_initSource();

  // initialize the contaminants
  this->_initContaminants();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a Source
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::on_addButton_clicked()
{
  // get the currently selected Contaminants
  QList<QTableWidgetItem*> selectedContaminants ( _contaminantTable->selectedItems() );

  // temp value to hold the selected contaminants
  Contaminants c;

  // loop through the selected contaminants
  for( int i = 0; i < selectedContaminants.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedContaminants.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // get the contaminant
    if( row < _contaminants.size() )
    {
      // temp value to hold the contaminant
      Contaminant ctemp ( _contaminants.at( row ) );

      // add to the temp list of selected contaminants
      c.push_back( ctemp );
    }
  }
    
  // get the currently selected sources
  QList<QTableWidgetItem*> selectedSources ( _sourceTable->selectedItems() );

  // loop through the selected sources and add the selected contaminants
  for( int i = 0; i < selectedSources.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedSources.at( i );

    // get the row index
    unsigned int row ( item->row() );

    if( row < _sources.size() )
    {
      // loop through and add contaminants to the selected source
      for( unsigned int j = 0; j < c.size(); ++j )
      {
        // add the contaminant to the list of contaminants for this source
        _sources.at( row ).contaminants.push_back( c.at( j ) );
      }
    }

  }
  
   // remove all the rows
  for( int i = _sourceTable->rowCount() - 1; i >= 0 ; --i )
  {
    _sourceTable->removeRow( i );
  }

  // reinitialize the sources list
  this->_initSource();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a Source
//
///////////////////////////////////////////////////////////////////////////////

void AddContaminantsToSourceDialog::on_clearButton_clicked()
{

}

