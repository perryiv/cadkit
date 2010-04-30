
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

#include "SoilLayerDialog.h"
#include "AddSoilDialog.h"

#include "Usul/Strings/Format.h"
#include "Usul/Exceptions/Canceled.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

 SoilLayerDialog:: SoilLayerDialog ( QWidget *parent ) : 
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

 SoilLayerDialog::~ SoilLayerDialog()
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the Soil Library.
//
///////////////////////////////////////////////////////////////////////////////

SoilLayerDialog::SoilLibrary SoilLayerDialog::library()
{
  return _library;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soil Library.
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::library( SoilLibrary l )
{
  _library = l;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Soils.
//
///////////////////////////////////////////////////////////////////////////////

 SoilLayerDialog::Soils  SoilLayerDialog::soils()
{
  return _soils;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Soils.
//
///////////////////////////////////////////////////////////////////////////////

void  SoilLayerDialog::soils( Soils s )
{
  _soils = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void  SoilLayerDialog::initialize()
{
  this->_initialize();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void  SoilLayerDialog::_initialize()
{
  // initialize the soil drop down
  this->_initSoils();

  // size the columns
  _soilTable->resizeColumnsToContents();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the experiment soils to match this soil if they contain it
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::_updateSoils( Soil s )
{
  for( unsigned int i = 0; i < _soils.size(); ++i )
  {
    // get the soil
    Soil soil ( _soils.at( i ) );

    // get the soil name
    std::string name ( soil.name );

    // check to see if the names match
    if( s.name == soil.name )
    {
			// set the updated soil attributes
			soil.attributes( s.name, s.type, s.porosity, s.waterPorosity, s.permeability, s.viscosity, s.carbon );

      // replace the soul
      _soils.at( i ) = soil;

      // break
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the soil at index
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::_updateSoil( unsigned int index )
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

void SoilLayerDialog::_initSoils()
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

  // make sure there is at least 1 soil 
  if( _soils.size() <= 0 )
  {
    return;
  }

  // make sure there is at least 1 soil   
  Usul::Math::Vec4f fColor ( _soils.at( 0 ).color );
  Usul::Math::Vec4i iColor (   static_cast< int > ( fColor[0] * 255 ),
                               static_cast< int > ( fColor[1] * 255 ),
                               static_cast< int > ( fColor[2] * 255 ),
                               static_cast< int > ( fColor[3] * 255 ) );

  // set the initial name
  currentSoilName->setText( _soils.at( 0 ).name.c_str() );

  // set the color in the color box
  QColor c ( iColor[0], iColor[1], iColor[2], iColor[3] );
  color->color( c );

  for( unsigned int i = 0; i < _soils.size(); ++i )
  {
    // get the name
    std::string name ( _soils.at( i ).layerName );

    // add the name to the drop down box
    soilDropDown->addItem( name.c_str() );    
  }
  

  
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize items for the current selection
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::finalize()
{
  // update the library
  this->_updateLibrary();

  // make sure there is at least 1 soil
  if( _soils.size() <= 0 )
  {
    return;
  }

  // get the index of the current soil
  unsigned int index ( _currentSoil ); 

  // get the current color
  QColor qc ( color->color() );

  // get the current soil
  Soil s ( _soils.at( index ) );

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
  _soils.at( index ) = s;

  // upadte the soil name
  currentSoilName->setText( s.name.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update any changes to the library through user edit
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::_updateLibrary()
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
    this->_updateSoils( lSoil );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal received on drop down box to change selected item
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::on_soilDropDown_activated( int index )
{
  // finalize the current soil
  this->finalize();

  // get the index of the next soil
  unsigned int nextIndex ( static_cast< unsigned int > ( index ) );

  // update the current index
  _currentSoil = nextIndex;

	// get the color
	Usul::Math::Vec4f fColor ( _soils.at( nextIndex ).color );
  Usul::Math::Vec4i iColor (   static_cast< int > ( fColor[0] * 255 ),
                               static_cast< int > ( fColor[1] * 255 ),
                               static_cast< int > ( fColor[2] * 255 ),
                               static_cast< int > ( fColor[3] * 255 ) );

	// set the color in the color box
  QColor c ( iColor[0], iColor[1], iColor[2], iColor[3] );
  color->color( c );

	// set the name in the edit box
	this->currentSoilName->setText( _soils.at( nextIndex ).name.c_str() );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal received on color button change
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::on_color_selected ( const QColor & color )
{
  this->finalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal received on apply button clicked
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::on_applyButton_clicked()
{
  this->finalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the soils table
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::_clearTable()
{
  // remove all the rows
  for( int i = _soilTable->rowCount() - 1; i >= 0 ; --i )
  {
    _soilTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button clicked
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::on_removeButton_clicked()
{
   // get the currently selected chemicals
  QList<QTableWidgetItem*> selectedItems ( _soilTable->selectedItems() );

  std::vector< unsigned int > rowsToRemove;

  // loop through the selected chemicals
  for( int i = 0; i < selectedItems.size(); ++i )
  {
    // get the current item
    QTableWidgetItem* item = selectedItems.at( i );

    // get the row index
    unsigned int row ( item->row() );

    // add to the rows to remove list
    rowsToRemove.push_back( row );
  }

  // remove from the active chemicals
  {
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
  } // end active chemical removal
  
  // remove from the chemical library
  {
    // new building cracks
    Soils newlist;

    // old building cracks
    Soils oldlist ( _library );

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
    _library = newlist;
  } // end library removal

  // clear the cracks table
  this->_clearTable();

  // repopulate the table
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button clicked
//
///////////////////////////////////////////////////////////////////////////////

void SoilLayerDialog::on_addButton_clicked()
{
  // create an instance of AddSoilDialog to handle the request
  AddSoilDialog dialog;

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

  // get the chemical
  Soil s ( dialog.createSoil() );

  // add the Source to the list of contamimants
  _library.push_back( s );

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

  // create a checkbox widget for the seventh column
  QTableWidgetItem *item6 = new QTableWidgetItem;
  item6->data( Qt::CheckStateRole );
  item6->setCheckState( Qt::Checked );

  // set the current item
  _soilTable->setCurrentItem( item0 );

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
 
}