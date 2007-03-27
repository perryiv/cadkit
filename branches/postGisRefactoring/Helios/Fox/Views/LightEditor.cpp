
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Fox/Views/LightEditor.h"

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

using namespace Helios::Views;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( LightEditor ) MessageMap[] =
{
  // Message type,                     id,                                handler.
  FXMAPFUNC ( FX::SEL_COMMAND,         LightEditor::ID_AMBIENT,         LightEditor::onCommandAmbient ),
  FXMAPFUNC ( FX::SEL_COMMAND,         LightEditor::ID_DIFFUSE,         LightEditor::onCommandDiffuse ),
  FXMAPFUNC ( FX::SEL_COMMAND,         LightEditor::ID_SPECULAR,        LightEditor::onCommandSpecular ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( LightEditor, BaseClass, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

LightEditor::LightEditor ( FX::FXWindow *parent, osg::Light* light, Usul::Interfaces::IUnknown* caller ) :
BaseClass ( parent, "Light Editor",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE ),
_light ( light ),
_viewer ( caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the material editor
//
///////////////////////////////////////////////////////////////////////////////

void LightEditor::show (  )
{
  // Make a copy
  osg::ref_ptr< osg::Light > copy ( new osg::Light ( *_light ) );

  FX::FXVerticalFrame *contents ( new FX::FXVerticalFrame( this , LAYOUT_FILL_X|LAYOUT_FILL_Y) );

  FX::FXHorizontalFrame *hframe ( new FX::FXHorizontalFrame ( contents, FX::LAYOUT_FILL_Y| FX::LAYOUT_FILL_X ) );

  FX::FXTabBook *tabBook ( new FX::FXTabBook ( hframe, 0x0, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

  // Build the tabs
  this->_buildTab ( "Ambient",  tabBook, _ambientSliders,  LightEditor::ID_AMBIENT );
  this->_buildTab ( "Diffuse",  tabBook, _diffuseSliders,  LightEditor::ID_DIFFUSE );
  this->_buildTab ( "Specular", tabBook, _specularSliders, LightEditor::ID_SPECULAR );

  // Set intial values
  osg::Vec4 ambient ( _light->getAmbient( ) );
  this->_update( ambient, _ambientSliders );

  // Set intial values
  osg::Vec4 diffuse ( _light->getDiffuse( ) );
  this->_update( diffuse, _diffuseSliders );

  // Set intial values
  osg::Vec4 specular ( _light->getSpecular( ) );
  this->_update( specular, _specularSliders );

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
    //*_light = *copy;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build tab with given information
//
///////////////////////////////////////////////////////////////////////////////

void LightEditor::_buildTab ( const std::string& name, FX::FXTabBook *tabBook, FoxTools::Widgets::SliderTextField* sliders[], unsigned int id )
{
  //Ambient tab
  FX::FXTabItem *tabItem ( new FX::FXTabItem ( tabBook, name.c_str(), 0x0 ) );

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

long  LightEditor::onCommandAmbient ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the ambient value from the sliders
  osg::Vec4 ambient ( this->_getVec4 ( _ambientSliders ) ) ;

  // Set the light's ambient value
  _light->setAmbient( ambient );

  _viewer->render();

  // Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set diffuse value
//
///////////////////////////////////////////////////////////////////////////////

long  LightEditor::onCommandDiffuse ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the diffuse value from the sliders
  osg::Vec4 diffuse ( this->_getVec4 ( _diffuseSliders )  );

  // Set the lights's diffuse value
  _light->setDiffuse( diffuse );

  _viewer->render();

  // Handled
  return 1;
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Set specular value
//
///////////////////////////////////////////////////////////////////////////////

long  LightEditor::onCommandSpecular ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the specular value from the sliders
  osg::Vec4 specular ( this->_getVec4 ( _specularSliders ) );

  // Set the lights's specular value
  _light->setSpecular ( specular );

  _viewer->render();

  // Handled
  return 1;
}

 
///////////////////////////////////////////////////////////////////////////////
//
//  Update sliders and text field with given vec4
//
///////////////////////////////////////////////////////////////////////////////

void LightEditor::_update   ( const osg::Vec4& vec, FoxTools::Widgets::SliderTextField* sliders[] )
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

osg::Vec4 LightEditor::_getVec4 ( FoxTools::Widgets::SliderTextField* sliders[] )
{
  double v1 ( (double )sliders[0]->getValue() / 255 );
  double v2 ( (double )sliders[1]->getValue() / 255 );
  double v3 ( (double )sliders[2]->getValue() / 255 );
  double v4 ( (double )sliders[3]->getValue() / 255 );

  return osg::Vec4 ( v1, v2, v3, v4 );
}


