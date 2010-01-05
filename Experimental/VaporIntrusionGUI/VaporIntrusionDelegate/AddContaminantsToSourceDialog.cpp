
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

 AddContaminantsToSourceDialog:: AddContaminantsToSourceDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  // create and set the source titles
  QStringList sourceTitles;

  sourceTitles.push_back( "Name" );
  sourceTitles.push_back( "Chemicals" );

  _sourceTable->setHorizontalHeaderLabels( sourceTitles );

  // create and set the chemical titles
  QStringList chemicalTitles;

  chemicalTitles.push_back( "Name" );
  chemicalTitles.push_back( "H. Law" );
  chemicalTitles.push_back( "Koc" );
  chemicalTitles.push_back( "Diff in Air" );
  chemicalTitles.push_back( "Diff in H2O" );
  chemicalTitles.push_back( "Atmo. Conc." );

  _contaminantTable->setHorizontalHeaderLabels( chemicalTitles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

 AddContaminantsToSourceDialog::~ AddContaminantsToSourceDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Sources.
//
///////////////////////////////////////////////////////////////////////////////

 AddContaminantsToSourceDialog::Sources  AddContaminantsToSourceDialog::sources()
{
  return _sources;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Sources.
//
///////////////////////////////////////////////////////////////////////////////

void  AddContaminantsToSourceDialog::sources( Sources s )
{
  _sources = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the chemicals
//
///////////////////////////////////////////////////////////////////////////////

 AddContaminantsToSourceDialog::Chemicals  AddContaminantsToSourceDialog::chemicals()
{
  return _chemicals;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the chemicals
//
///////////////////////////////////////////////////////////////////////////////

void  AddContaminantsToSourceDialog::chemicals( Chemicals c )
{
  _chemicals = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void  AddContaminantsToSourceDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization of sources.
//
///////////////////////////////////////////////////////////////////////////////

void  AddContaminantsToSourceDialog::_initSource()
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

    // get the source chemicals
    Chemicals sc( s.chemicals );

    // string to hold the chemical data
    std::string scstr ("( ");

    // loop through the already assigned chemicals
    for( unsigned int j = 0; j < sc.size(); ++j )
    {
      // get the chemical
      std::string cname ( sc.at( j ).name );
      
      if( j < sc.size() - 1 )
      {
        // if this isn't the last element add a comma after the chemical name
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
//  Initialization of chemicals.
//
///////////////////////////////////////////////////////////////////////////////

void  AddContaminantsToSourceDialog::_initContaminants()
{
// first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _chemicals.size(); ++i )
  {
    // create a Source object
    Chemical c ( _chemicals.at( i ) );

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

void  AddContaminantsToSourceDialog::_initialize()
{
  // initialize the sources
  this->_initSource();

  // initialize the chemicals
  this->_initContaminants();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a Source
//
///////////////////////////////////////////////////////////////////////////////

void  AddContaminantsToSourceDialog::on_addButton_clicked()
{
  // get the currently selected Chemicals
  QList<QTableWidgetItem*> selectedChemicals ( _contaminantTable->selectedItems() );

  // temp value to hold the selected chemicals
  Chemicals c;

  // loop through the selected chemicals
  for( int i = 0; i < selectedChemicals.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedChemicals.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // get the chemical
    if( row < _chemicals.size() )
    {
      // temp value to hold the chemical
      Chemical ctemp ( _chemicals.at( row ) );

      // add to the temp list of selected chemicals
      c.push_back( ctemp );
    }
  }
    
  // get the currently selected sources
  QList<QTableWidgetItem*> selectedSources ( _sourceTable->selectedItems() );

  // loop through the selected sources and add the selected chemicals
  for( int i = 0; i < selectedSources.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedSources.at( i );

    // get the row index
    unsigned int row ( item->row() );

    if( row < _sources.size() )
    {
      // loop through and add chemicals to the selected source
      for( unsigned int j = 0; j < c.size(); ++j )
      {
        // add the chemical to the list of chemicals for this source
        _sources.at( row ).chemicals.push_back( c.at( j ) );
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

void  AddContaminantsToSourceDialog::on_removeButton_clicked()
{
 // get the currently selected Chemicals
  QList<QTableWidgetItem*> selectedChemicals ( _contaminantTable->selectedItems() );

  // temp value to hold the selected chemicals
  Chemicals c;

  // loop through the selected chemicals
  for( int i = 0; i < selectedChemicals.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedChemicals.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // get the chemical
    if( row < _chemicals.size() )
    {
      // temp value to hold the chemical
      Chemical ctemp ( _chemicals.at( row ) );

      // add to the temp list of selected chemicals
      c.push_back( ctemp );
    }
  }
    
  // get the currently selected sources
  QList<QTableWidgetItem*> selectedSources ( _sourceTable->selectedItems() );

  // loop through the selected sources and add the selected chemicals
  for( int i = 0; i < selectedSources.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedSources.at( i );

    // get the row index
    unsigned int row ( item->row() );

    if( row < _sources.size() )
    {
      // get the old source
      Chemicals oldlist ( _sources.at( row ).chemicals );

      // new source
      Chemicals newlist;

      // loop through and remove chemicals from the selected source
      for( unsigned int j = 0; j < oldlist.size(); ++j )
      {
        // get the chemical name from the old lise
        std::string nameFromSource ( oldlist.at( j ).name );

        // remove?
        bool removeEntry ( false );

        // loop through the chemicals to remove
        for( unsigned int k = 0; k < c.size(); ++k )
        {
          // get the chemical name from the list of those to be removed
          std::string nameToRemove ( c.at( k ).name );

          if( nameToRemove == nameFromSource )
          {
            removeEntry = true;
            break;
          }

        }

        if( false == removeEntry )
        {
          newlist.push_back( oldlist.at( j ) );
        }

      }
      
      // update the source chemical list at i
      _sources.at( row ).chemicals = newlist;
      
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

