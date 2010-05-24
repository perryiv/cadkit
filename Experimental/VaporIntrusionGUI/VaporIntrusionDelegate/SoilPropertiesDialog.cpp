
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensoil.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "SoilPropertiesDialog.h"
#include "AddSoilDialog.h"

#include "Usul/Strings/Format.h"
#include "Usul/Exceptions/Canceled.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

 SoilPropertiesDialog:: SoilPropertiesDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_currentSoil ( 0 )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  // create and set the soil titles
  QStringList soilTitles;

  // SCS soil type, SCS soil name, Total porosity, water-filled porosity, effective soil gas permeability, 
    // Gas phase dynamic viscosity,  Fraction of organic carbon
  
  soilTitles.push_back( "Name" );
  soilTitles.push_back( "Type" );
  soilTitles.push_back( "Total Porosity (cm3/cm3)" );
  soilTitles.push_back( "Water-filled porosity(cm3/cm3)" );
  soilTitles.push_back( "Effective soil gas permeability [m2]" );
  soilTitles.push_back( "Dynamic Viscosity" );
  soilTitles.push_back( "Fraction of Organic Compound" );

  _soilTable->setHorizontalHeaderLabels( soilTitles );

  // size the columns
  _soilTable->resizeColumnsToContents();

  //initialize the table view
  this->_initialize();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

 SoilPropertiesDialog::~ SoilPropertiesDialog()
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the Soil Library.
//
///////////////////////////////////////////////////////////////////////////////

SoilPropertiesDialog::SoilLibrary SoilPropertiesDialog::library()
{
  return _library;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soil Library.
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::library( SoilLibrary l )
{
  _library = l;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Soils.
//
///////////////////////////////////////////////////////////////////////////////

 SoilPropertiesDialog::Soil  SoilPropertiesDialog::soil()
{
  return _soil;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soils.
//
///////////////////////////////////////////////////////////////////////////////

void  SoilPropertiesDialog::soil( Soil s )
{
  _soil = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void  SoilPropertiesDialog::initialize()
{
  this->_initialize();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void  SoilPropertiesDialog::_initialize()
{
  // initialize the soil drop down
  this->_initSoil();

  // size the columns
  _soilTable->resizeColumnsToContents();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the experiment soils to match this soil if they contain it
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::_updateSoil( Soil s )
{
    // get the soil
    Soil soil ( _soil );

    // check to see if the names match
    if( s.name == soil.name )
    {
			// set the updated soil attributes
			soil.attributes( s.name, s.type, s.porosity, s.waterPorosity, s.permeability, s.viscosity, s.carbon );

      // replace the soul
      _soil = soil;
    }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the soil at index
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::_updateSoil( unsigned int index )
{
   // make sure there is at least index soils
  if( _library.size() <= index )
  {
    return;
  }

  // the current number of rows
  unsigned int rowCount ( index );

  // get the soils
  Soil s ( _library.at( index ) );

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

  // insert the columns
  _soilTable->setItem( rowCount, 0, item0 );
  _soilTable->setItem( rowCount, 1, item1 );
  _soilTable->setItem( rowCount, 2, item2 );
  _soilTable->setItem( rowCount, 3, item3 );
  _soilTable->setItem( rowCount, 4, item4 );
  _soilTable->setItem( rowCount, 5, item5 ); 

  // set the values of the row
  _soilTable->item( rowCount, 0 )->setText( s.name.c_str()       );
  _soilTable->item( rowCount, 1 )->setText( s.type.c_str() );
  _soilTable->item( rowCount, 2 )->setText( s.porosity.c_str()      );
  _soilTable->item( rowCount, 3 )->setText( s.waterPorosity.c_str()        );
  _soilTable->item( rowCount, 4 )->setText( s.permeability.c_str()    );
  _soilTable->item( rowCount, 5 )->setText( s.carbon.c_str()   );

  ++rowCount;
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Init soil drop down
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::_initSoil()
{
  // make sure the library has something
  if( _library.size() <= 0 )
  {
    return;
  }

  // fill out the library table
  for( unsigned int i = 0; i < _library.size(); ++i )
  {    
    // update the soil
    this->_updateSoil( i );

  }

  Usul::Math::Vec4f fColor ( _soil.color );
  Usul::Math::Vec4i iColor (   static_cast< int > ( fColor[0] * 255 ),
                               static_cast< int > ( fColor[1] * 255 ),
                               static_cast< int > ( fColor[2] * 255 ),
                               static_cast< int > ( fColor[3] * 255 ) );

  // set the initial name
  currentSoilName->setText( _soil.name.c_str() );

	// set the soil layer name
	layerName->setText( _soil.layerName.c_str() );

  // set the color in the color box
  QColor c ( iColor[0], iColor[1], iColor[2], iColor[3] );
  color->color( c );  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize items for the current selection
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::finalize()
{
  // update the library
  this->_updateLibrary();

  // get the current color
  QColor qc ( color->color() );

  // get the current soil
  Soil s ( _soil );

  // set the color 
  s.color = Usul::Math::Vec4f( static_cast< float >( qc.red() )   / 255.0f,
                               static_cast< float >( qc.green() ) / 255.0f,
                               static_cast< float >( qc.blue() )  / 255.0f,
                               static_cast< float >( qc.alpha() ) / 255.0f );


  // get the selected row from the soil library
  QList<QTableWidgetItem*> selectedItems ( _soilTable->selectedItems() );
  
  // make sure there is something there
  if( selectedItems.size() > 0 )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( 0 );

    // get the row index
    unsigned int row ( item->row() );

    // get the attributes
    std::string n    ( _soilTable->item( row, 0 )->text().toStdString() );
    std::string t    ( _soilTable->item( row, 1 )->text().toStdString() );
    std::string p    ( _soilTable->item( row, 2 )->text().toStdString() );
    std::string wp   ( _soilTable->item( row, 3 )->text().toStdString() );
    std::string perm ( _soilTable->item( row, 4 )->text().toStdString() );
    std::string carb ( _soilTable->item( row, 5 )->text().toStdString() );

    // set the attributes
    s.attributes( n, t, p, wp, perm, "1.0", carb );
  }

  // set the soil
  _soil = s;

  // upadte the soil name
  currentSoilName->setText( s.name.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update any changes to the library through user edit
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::_updateLibrary()
{
  // make sure there is a library
  if( static_cast< int > ( _library.size() ) != _soilTable->rowCount() )
    return;

  // check the table
  for( int i = 0; i < _soilTable->rowCount(); ++ i )
  {
    // get the attributes
    std::string n    ( _soilTable->item( i, 0 )->text().toStdString() );
    std::string t    ( _soilTable->item( i, 1 )->text().toStdString() );
    std::string p    ( _soilTable->item( i, 2 )->text().toStdString() );
    std::string wp   ( _soilTable->item( i, 3 )->text().toStdString() );
    std::string perm ( _soilTable->item( i, 4 )->text().toStdString() );
    std::string carb ( _soilTable->item( i, 5 )->text().toStdString() );

    // Create the soil from user input
    Soil iSoil;

    // set the attributes
    iSoil.attributes( n, t, p, wp, perm, "1.0", carb );

    // get the corresponding soil in the library
    Soil lSoil ( _library.at( i ) );

    // update the attributes
    lSoil.attributes( n, t, p, wp, perm, "1.0", carb );

    // set the soil
    _library.at( i ) = lSoil;

    // update the experiment soils
    this->_updateSoil( lSoil );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal received on color button change
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::on_color_selected ( const QColor & color )
{
  this->finalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal received on apply button clicked
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::on_applyButton_clicked()
{
  this->finalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the soils table
//
///////////////////////////////////////////////////////////////////////////////

void SoilPropertiesDialog::_clearTable()
{
  // remove all the rows
  for( int i = _soilTable->rowCount() - 1; i >= 0 ; --i )
  {
    _soilTable->removeRow( i );
  }
}

