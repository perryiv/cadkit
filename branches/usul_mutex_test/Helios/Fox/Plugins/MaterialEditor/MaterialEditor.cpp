
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialEditor.h"

#include "Usul/Cast/Cast.h"

#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/Slider.h"
#include "FoxTools/Headers/TextField.h"

#include "FoxTools/Functions/GLVisual.h"

#include "FoxTools/Widgets/SliderTextField.h"

#include "osg/Geode"
#include "osg/ShapeDrawable"



///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( MaterialEditor ) MessageMap[] =
{
  // Message type,                     id,                                handler.
  FXMAPFUNC ( FX::SEL_COMMAND,         MaterialEditor::ID_AMBIENT,         MaterialEditor::onCommandAmbient ),
  FXMAPFUNC ( FX::SEL_COMMAND,         MaterialEditor::ID_DIFFUSE,         MaterialEditor::onCommandDiffuse ),
  FXMAPFUNC ( FX::SEL_COMMAND,         MaterialEditor::ID_SPECULAR,        MaterialEditor::onCommandSpecular ),
  FXMAPFUNC ( FX::SEL_COMMAND,         MaterialEditor::ID_EMISSION,        MaterialEditor::onCommandEmission ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( MaterialEditor, BaseClass, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

MaterialEditor::MaterialEditor ( FX::FXWindow *parent, osg::Material* mat ) :
BaseClass ( parent, "Material Editor",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE ),
_material ( mat )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the material editor
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::show (  )
{
  // Make a copy
  osg::ref_ptr< osg::Material > copy ( new osg::Material ( *_material ) );

  FX::FXVerticalFrame *contents ( new FX::FXVerticalFrame( this , LAYOUT_FILL_X|LAYOUT_FILL_Y) );

  FX::FXHorizontalFrame *hframe ( new FX::FXHorizontalFrame ( contents, FX::LAYOUT_FILL_Y| FX::LAYOUT_FILL_X ) );

#if 0

  // Make a canvas for showing the material
  std::auto_ptr < Helios::Views::Canvas > canvas ( new Helios::Views::Canvas ( 0x0, hframe, FoxTools::Functions::visual() ) );

  // Set the scene
  //canvas->scene( document->buildScene() );
  
  canvas->setLayoutHints( canvas->getLayoutHints() | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

  canvas->updateTimes( false );

  canvas->render();
#endif

  FX::FXTabBook *tabBook ( new FX::FXTabBook ( hframe, 0x0, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

  // Build the tabs
  this->_buildTab ( "Ambient",  tabBook, _ambientSliders,  MaterialEditor::ID_AMBIENT );
  this->_buildTab ( "Diffuse",  tabBook, _diffuseSliders,  MaterialEditor::ID_DIFFUSE );
  this->_buildTab ( "Specular", tabBook, _specularSliders, MaterialEditor::ID_SPECULAR );
  this->_buildTab ( "Emission", tabBook, _emissionSliders, MaterialEditor::ID_EMISSION );

  // Set intial values
  osg::Vec4 ambient ( _material->getAmbient( osg::Material::FRONT_AND_BACK ) );
  this->_update( ambient, _ambientSliders );

  // Set intial values
  osg::Vec4 diffuse ( _material->getDiffuse( osg::Material::FRONT_AND_BACK ) );
  this->_update( diffuse, _diffuseSliders );

  // Set intial values
  osg::Vec4 specular ( _material->getSpecular( osg::Material::FRONT_AND_BACK ) );
  this->_update( specular, _specularSliders );

  // Set intial values
  osg::Vec4 emission ( _material->getEmission( osg::Material::FRONT_AND_BACK ) );
  this->_update( emission, _emissionSliders );

  // Buttons
  FX::FXHorizontalFrame *buttons ( new FX::FXHorizontalFrame ( contents, FX::LAYOUT_FILL_X) );
  new FX::FXButton ( buttons, "Accept", 0x0, this, FX::FXDialogBox::ID_ACCEPT, BUTTON_DEFAULT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK );
  new FX::FXButton ( buttons, "Cancel", 0x0, this, FX::FXDialogBox::ID_CANCEL, BUTTON_DEFAULT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK );

  this->create();

  if( this->execute( FX::PLACEMENT_OWNER ) )
  {
  }
  else
  {
    // Restore if the user cancels
    *_material = *copy;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build tab with given information
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::_buildTab ( const std::string& name, FX::FXTabBook *tabBook, FoxTools::Widgets::SliderTextField* sliders[], unsigned int id )
{
  //Ambient tab
 // FX::FXTabItem *tabItem ( new FX::FXTabItem ( tabBook, name.c_str(), 0x0 ) );

  //frame for ambient tab
  FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( tabBook,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK ) );
  
  //Make the sliders with text fields
  sliders[0] = new FoxTools::Widgets::SliderTextField ( frame, this, id );
  sliders[1] = new FoxTools::Widgets::SliderTextField ( frame, this, id );
  sliders[2] = new FoxTools::Widgets::SliderTextField ( frame, this, id );
  sliders[3] = new FoxTools::Widgets::SliderTextField ( frame, this, id );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ambient values
//
///////////////////////////////////////////////////////////////////////////////

long  MaterialEditor::onCommandAmbient ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the ambient value from the sliders
  osg::Vec4 ambient ( this->_getVec4 ( _ambientSliders ) ) ;

  // Set the material's ambient value
  _material->setAmbient( osg::Material::FRONT_AND_BACK, ambient );

  // Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set diffuse value
//
///////////////////////////////////////////////////////////////////////////////

long  MaterialEditor::onCommandDiffuse ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the diffuse value from the sliders
  osg::Vec4 diffuse ( this->_getVec4 ( _diffuseSliders )  );

  // Set the material's diffuse value
  _material->setDiffuse( osg::Material::FRONT_AND_BACK, diffuse );

  // Handled
  return 1;
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Set specular value
//
///////////////////////////////////////////////////////////////////////////////

long  MaterialEditor::onCommandSpecular ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the specular value from the sliders
  osg::Vec4 specular ( this->_getVec4 ( _specularSliders ) );

  // Set the material's specular value
  _material->setSpecular ( osg::Material::FRONT_AND_BACK, specular );

  // Handled
  return 1;
}

  

///////////////////////////////////////////////////////////////////////////////
//
//  Set Emission value
//
///////////////////////////////////////////////////////////////////////////////

long  MaterialEditor::onCommandEmission ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the emission value from the sliders
  osg::Vec4 emission (  this->_getVec4 ( _emissionSliders )  );

  // Set the material's emission value
  _material->setEmission ( osg::Material::FRONT_AND_BACK, emission );

  // Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update sliders and text field with given vec4
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::_update   ( const osg::Vec4& vec, FoxTools::Widgets::SliderTextField* sliders[] )
{
  sliders[0]->setValue( (int) (vec[0] * 255) );
  sliders[1]->setValue( (int) (vec[1] * 255) );
  sliders[2]->setValue( (int) (vec[2] * 255) );
  sliders[3]->setValue( (int) (vec[3] * 255) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build vec4 from given sliders
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 MaterialEditor::_getVec4 ( FoxTools::Widgets::SliderTextField* sliders[] )
{
  double v1 ( (double )sliders[0]->getValue() / 255 );
  double v2 ( (double )sliders[1]->getValue() / 255 );
  double v3 ( (double )sliders[2]->getValue() / 255 );
  double v4 ( (double )sliders[3]->getValue() / 255 );

  return osg::Vec4 ( v1, v2, v3, v4 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this material transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool MaterialEditor::isTransparent () const
{
  return ( _ambientSliders[3]->getValue() < 255 || _diffuseSliders[3]->getValue() < 255 || _specularSliders[3]->getValue() < 255 || _emissionSliders[3]->getValue() < 255 );
}

