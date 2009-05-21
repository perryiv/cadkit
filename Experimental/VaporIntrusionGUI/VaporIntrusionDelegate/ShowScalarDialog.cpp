
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

#include "ShowScalarDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowScalarDialog::ShowScalarDialog ( Vec3ui min, Vec3ui max, Vec3ui dim, GridMaterials gm, QWidget *parent ) : 
BaseClass ( parent ),
_gridMaterials( gm ),
_min( min ),
_max( max ),
_dimensions( dim )
{
  // Initialize code from Designer.
  this->setupUi ( this );
  

  QStringList titles;

  titles.push_back( "X" );
  titles.push_back( "Y" );
  titles.push_back( "Z" );
  titles.push_back( "Materials" );

  _inputList->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowScalarDialog::~ShowScalarDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the table
//
///////////////////////////////////////////////////////////////////////////////

void ShowScalarDialog::_initialize()
{
  // useful typedefs
  typedef Usul::Interfaces::IVaporIntrusionGUI::GridMaterial GridMaterial;
  typedef Usul::Interfaces::IVaporIntrusionGUI::MaterialsMap MaterialsMap;

   // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // first pass through z values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = _min[0]; i <= _max[0]; ++i )
  {
    for( unsigned int j = _min[1]; j <= _max[1]; ++j )
    {
      for( unsigned int k = _min[2]; k <= _max[2]; ++k )
      {
        // add a row
        _inputList->insertRow( rowCount );

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

        if( true == firstPass )
        {
          // set the current item
          _inputList->setCurrentItem( item0 );

          // no longer the first pass
          firstPass = false;
        }

        // get the map key
        std::string key ( Usul::Strings::format( "x", i, "y", j, "z",k ) );

        // the materials map for this point
        MaterialsMap mmap ( _gridMaterials[key].materials );

        // the materials for this grid point converted to a string
        std::string materialsAsString;

        // set the materials string from the values in the map
        for( MaterialsMap::iterator iter = mmap.begin(); iter != mmap.end(); ++iter )
        {
          // get the name
          std::string name ( (*iter).first );

          // get the value
          std::string value ( (*iter).second );

          if( materialsAsString.length() > 0 )
          {
            // append to the string
            materialsAsString = Usul::Strings::format( materialsAsString, " | {", name, ":", value, " }" );
          }
          else
          {
            // append to the string
            materialsAsString = Usul::Strings::format( materialsAsString, "{", name, ":", value, " }" );
          }

        }
        // insert the columns
        _inputList->setItem( rowCount, 0, item0 );
        _inputList->setItem( rowCount, 1, item1 );
        _inputList->setItem( rowCount, 2, item2 );
        _inputList->setItem( rowCount, 3, item3 );
  
        // set the values of the row
        _inputList->item( rowCount, 0 )->setText( Usul::Strings::format( i ).c_str() );
        _inputList->item( rowCount, 1 )->setText( Usul::Strings::format( j ).c_str() );
        _inputList->item( rowCount, 2 )->setText( Usul::Strings::format( k ).c_str() );
        _inputList->item( rowCount, 3 )->setText( materialsAsString.c_str() );

        ++rowCount;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the materials
//
///////////////////////////////////////////////////////////////////////////////

ShowScalarDialog::GridMaterials ShowScalarDialog::gridMaterials()
{
  return _gridMaterials;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the materials
//
///////////////////////////////////////////////////////////////////////////////

void ShowScalarDialog::gridMaterials( GridMaterials gm )
{
  _gridMaterials = gm;
}
