
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

#include "ScalarEditorDialog.h"
#include "ShowScalarDialog.h"

#include "Usul/Exceptions/Canceled.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ScalarEditorDialog::ScalarEditorDialog ( Usul::Math::Vec3ui dimensions, QWidget *parent ) : 
BaseClass ( parent ),
_materials(),
_dimensions( dimensions )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the grid materials
  _gridMaterials = document->gridMaterials();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ScalarEditorDialog::~ScalarEditorDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button was clicked
//
///////////////////////////////////////////////////////////////////////////////

void ScalarEditorDialog::on_addButton_clicked()
{
  // useful typedefs
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::GridMaterial GridMaterial;
  
  // get the min max
  MinMaxPair minMax ( this->_getMinMax() );  
  Usul::Math::Vec3ui min ( minMax.first );
  Usul::Math::Vec3ui max ( minMax.second );

  // set the grid values
  for( unsigned int i = min[0]; i <= max[0]; ++i )
  {
    for( unsigned int j = min[1]; j <= max[1]; ++j )
    {
      for( unsigned int k = min[2]; k <= max[2]; ++k )
      {
        // get the map key
        std::string key ( Usul::Strings::format( "x", i, "y", j, "z",k ) );

        // get the grid material map for this key
        GridMaterial gm ( _gridMaterials[key] );

        // loop through checked materials and add them to the material map
        for( MaterialsMap::iterator iter = _materials.begin(); iter != _materials.end(); ++iter )
        {
          // get the name
          std::string name ( (*iter).first );

          // get the value
          std::string value ( (*iter).second );

          // set the material
          gm.materials[name] = value;
        }

        // update the materials map
        _gridMaterials[key] = gm;

      }
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove button was clicked
//
///////////////////////////////////////////////////////////////////////////////

void ScalarEditorDialog::on_removeButton_clicked()
{
  // useful typedefs
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::GridMaterial GridMaterial;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // get the min max
  MinMaxPair minMax ( this->_getMinMax() );  
  Usul::Math::Vec3ui min ( minMax.first );
  Usul::Math::Vec3ui max ( minMax.second );

   // remove the grid values
  for( unsigned int i = min[0]; i <= max[0]; ++i )
  {
    for( unsigned int j = min[1]; j <= max[1]; ++j )
    {
      for( unsigned int k = min[2]; k <= max[2]; ++k )
      {
        // get the map key
        std::string key ( Usul::Strings::format( "x", i, "y", j, "z",k ) );

        // get the grid material map for this key
        GridMaterial gm ( _gridMaterials[key] );

        // Get the materials map for the current GridMaterial
        MaterialsMap cmap ( gm.materials );

        // loop through checked materials and add them to the material map
        for( MaterialsMap::iterator iter = _materials.begin(); iter != _materials.end(); ++iter )
        {          
          // get the name
          std::string name ( (*iter).first );

          // get the value
          std::string value ( (*iter).second );

          // remove the map entry
          cmap.erase( name );          
        }

        // set the material
         gm.materials = cmap;

        // update the materials map
        _gridMaterials[key] = gm;

      }
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Show button was clicked
//
///////////////////////////////////////////////////////////////////////////////

void ScalarEditorDialog::on_showButton_clicked()
{
  // get the min max based on current user input
  MinMaxPair minMax ( this->_getMinMax() );

  // Make the dialog.
  ShowScalarDialog dialog( minMax.first, minMax.second, _dimensions, _gridMaterials );

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the materials
//
///////////////////////////////////////////////////////////////////////////////

ScalarEditorDialog::MaterialsMap ScalarEditorDialog::materials()
{
  return _materials;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the materials
//
///////////////////////////////////////////////////////////////////////////////

void ScalarEditorDialog::materials( MaterialsMap m )
{
  _materials = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ok button was clicked
//
///////////////////////////////////////////////////////////////////////////////

void ScalarEditorDialog::onOkClicked()
{
   // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // update the grid materials in teh document
  document->gridMaterials( _gridMaterials );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Min/Max based upon current user input
//
///////////////////////////////////////////////////////////////////////////////

ScalarEditorDialog::MinMaxPair ScalarEditorDialog::_getMinMax()
{

  // useful typedefs
  Usul::Math::Vec3ui Vec3ui;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
  {
    MinMaxPair emptyPair;
    return emptyPair;
  }

  // Parse and set the xmin value.  Make sure it is valid
  unsigned int xMinUI( 0 );
  std::string xMinStr ( xMin->text().toStdString() );
  if( xMinStr.length() > 0 )
    xMinUI = Usul::Convert::Type< std::string, unsigned int>::convert( xMinStr );

  // Parse and set the xmax value.  Make sure it is valid
  unsigned int xMaxUI( 0 );
  std::string xMaxStr ( xMax->text().toStdString() );
  if( xMaxStr.length() > 0 )
    xMaxUI = Usul::Convert::Type< std::string, unsigned int>::convert( xMaxStr );

  // Parse and set the ymin value.  Make sure it is valid
  unsigned int yMinUI( 0 );
  std::string yMinStr ( yMin->text().toStdString() );
  if( yMinStr.length() > 0 )
    yMinUI = Usul::Convert::Type< std::string, unsigned int>::convert( yMinStr );

  // Parse and set the ymax value.  Make sure it is valid
  unsigned int yMaxUI( 0 );
  std::string yMaxStr ( yMax->text().toStdString() );
  if( yMaxStr.length() > 0 )
    yMaxUI = Usul::Convert::Type< std::string, unsigned int>::convert( yMaxStr );

  // Parse and set the xmin value.  Make sure it is valid
  unsigned int zMinUI( 0 );
  std::string zMinStr ( zMin->text().toStdString() );
  if( zMinStr.length() > 0 )
    zMinUI = Usul::Convert::Type< std::string, unsigned int>::convert( zMinStr );

  // Parse and set the xmax value.  Make sure it is valid
  unsigned int zMaxUI( 0 );
  std::string zMaxStr ( zMax->text().toStdString() );
  if( zMaxStr.length() > 0 )
    zMaxUI = Usul::Convert::Type< std::string, unsigned int>::convert( zMaxStr );

  // verify values for x
  if( xMinUI > xMaxUI )
    xMaxUI = xMinUI;

  if( xMinUI < 0 )
    xMinUI = 0;

  if( xMaxUI > _dimensions[0] - 1 )
    xMaxUI = _dimensions[0] - 1;

  // verify values for y
  if( yMinUI > yMaxUI )
    yMaxUI = yMinUI;

  if( yMinUI < 0 )
    yMinUI = 0;

  if( yMaxUI > _dimensions[1] - 1 )
    yMaxUI = _dimensions[1] - 1;

  // verify values for z
  if( zMinUI > zMaxUI )
    zMaxUI = zMinUI;

  if( zMinUI < 0 )
    zMinUI = 0;

  if( zMaxUI > _dimensions[2] - 1 )
    zMaxUI = _dimensions[2] - 1;

  // min and max values
  Usul::Math::Vec3ui min( xMinUI, yMinUI, zMinUI );
  Usul::Math::Vec3ui max( xMaxUI, yMaxUI, zMaxUI );
  MinMaxPair minMax( min, max );

  return minMax;
}