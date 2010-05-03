
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

#include "SourcePropertiesDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SourcePropertiesDialog::SourcePropertiesDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  
  // create and set the chemical titles
  QStringList chemicalTitles;
  
  chemicalTitles.push_back( "Name" );
  chemicalTitles.push_back( "Concentration" );
  chemicalTitles.push_back( "H. Law" );
  chemicalTitles.push_back( "Koc" );
  chemicalTitles.push_back( "Diff in Air" );
  chemicalTitles.push_back( "Diff in H2O" );
  chemicalTitles.push_back( "Atmo. Conc." );

  _chemicalTable->setHorizontalHeaderLabels( chemicalTitles );

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

SourcePropertiesDialog::~SourcePropertiesDialog()
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the Sources.
//
///////////////////////////////////////////////////////////////////////////////

 SourcePropertiesDialog::Source  SourcePropertiesDialog::source()
{
  return _source;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Sources.
//
///////////////////////////////////////////////////////////////////////////////

void  SourcePropertiesDialog::source( Source s )
{
  _source = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void  SourcePropertiesDialog::_initialize()
{
  // initialize the source drop down
  this->_initSource();

  // initialize the chemicals
  this->_updateSource();

  // size the columns
  _chemicalTable->resizeColumnsToContents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the source at index
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::_updateSource()
{
  // set the name in the edit box
  sourceName->setText( _source.name.c_str() );

  Usul::Math::Vec4f fColor ( _source.color );
  Usul::Math::Vec4i iColor (   static_cast< int > ( fColor[0] * 255 ),
                               static_cast< int > ( fColor[1] * 255 ),
                               static_cast< int > ( fColor[2] * 255 ),
                               static_cast< int > ( fColor[3] * 255 ) );

  // set the color in the color box
  QColor c ( iColor[0], iColor[1], iColor[2], iColor[3] );
  color->color( c );

  // clear the chemical table
  this->_clearTable();

  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  // get the chemicals
  Chemicals chemicals ( _source.chemicals );

  for( unsigned int i = 0; i < chemicals.size(); ++i )
  {
    // create a Source object
    Chemical c ( chemicals.at( i ) );

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

    // create an item widget for the sixth column
    QTableWidgetItem *item6 = new QTableWidgetItem;
    item6->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

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
    _chemicalTable->item( rowCount, 0 )->setText( c.name.c_str()       );
    _chemicalTable->item( rowCount, 1 )->setText( c.sourceConc.c_str() );
    _chemicalTable->item( rowCount, 2 )->setText( c.henry.c_str()      );
    _chemicalTable->item( rowCount, 3 )->setText( c.koc.c_str()        );
    _chemicalTable->item( rowCount, 4 )->setText( c.airDiff.c_str()    );
    _chemicalTable->item( rowCount, 5 )->setText( c.waterDiff.c_str()  );
    _chemicalTable->item( rowCount, 6 )->setText( c.atmoConc.c_str()   );

    ++rowCount;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Init source drop down
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::_initSource()
{

  // set the name in the edit box
  sourceName->setText( _source.name.c_str() );

  Usul::Math::Vec4f fColor ( _source.color );
  Usul::Math::Vec4i iColor (   static_cast< int > ( fColor[0] * 255 ),
                               static_cast< int > ( fColor[1] * 255 ),
                               static_cast< int > ( fColor[2] * 255 ),
                               static_cast< int > ( fColor[3] * 255 ) );

  // set the color in the color box
  QColor c ( iColor[0], iColor[1], iColor[2], iColor[3] );
  color->color( c );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize items for the current selection
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::finalize()
{
  // get the name in the edit box
  std::string n ( sourceName->text().toStdString() );

  // get the current color
  QColor qc ( color->color() );

  // get the current source
  Source s ( _source );

  // set the name of the source
  s.name = n;

  // set the color 
  s.color = Usul::Math::Vec4f( static_cast< float >( qc.red() )   / 255.0f,
                               static_cast< float >( qc.green() ) / 255.0f,
                               static_cast< float >( qc.blue() )  / 255.0f,
                               static_cast< float >( qc.alpha() ) / 255.0f );

  // set the chemical concentrations
  for( unsigned int i = 0; i < s.chemicals.size(); ++i )
  {
    // get the concentration level
    std::string conc ( _chemicalTable->item( i, 1 )->text().toStdString() );

    // set the concentration level
    s.chemicals.at( i ).sourceConc = conc;
  }

  // set the source
  _source = s;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::initialize()
{
  this->_initialize();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the chemicals table
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::_clearTable()
{
  // remove all the rows
  for( int i = _chemicalTable->rowCount() - 1; i >= 0 ; --i )
  {
    _chemicalTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal received on source name change
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::on_sourceName_editingFinished()
{
  this->finalize();
}
