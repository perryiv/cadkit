
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialEditorComponent.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/Child.h"
#include "FoxTools/Predicates/IsMemberOf.h"
#include "FoxTools/Predicates/HasText.h"
#include "FoxTools/Functors/SetText.h"
#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/ColorSelector.h"
#include "FoxTools/Headers/ColorDialog.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/CheckButton.h"
#include "FoxTools/Headers/RadioButton.h"
#include "FoxTools/Headers/Separator.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/Label.h"
#include "FoxTools/Headers/ColorWheel.h"
#include "FoxTools/Headers/ColorBar.h"
#include "FoxTools/Headers/Matrix.h"
#include "FoxTools/Headers/ScrollWindow.h"
#include "FoxTools/Headers/Splitter.h"
#include "FoxTools/Builder/Reader.h"

#include "OsgFox/Views/Canvas.h"

#include "OsgTools/ShapeFactory.h"

#include "Usul/Functors/If.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Adaptors/Random.h"

#include <memory>
#include <functional>
#include <algorithm>
#include <vector>

#ifdef NDEBUG
#include "MaterialEditor.h"
#endif

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MaterialEditorComponent , MaterialEditorComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Some zeros.
//
///////////////////////////////////////////////////////////////////////////////

#define ZEROS 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialEditorComponent::MaterialEditorComponent() : BaseClass(), FX::FXObject(),
  _mat ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialEditorComponent::~MaterialEditorComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MaterialEditorComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IMaterialEditor::IID:
    return static_cast < Usul::Interfaces::IMaterialEditor*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the material editor inside a modal dialog.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::runModalDialog()
{
#if 0
  FX::FXDialogBox dialog ( FoxTools::Functions::mainWindow(), "Material Editor", FX::DECOR_TITLE | FX::DECOR_CLOSE | FX::DECOR_BORDER | FX::DECOR_RESIZE, ZEROS );
  FoxTools::Builder::Reader reader;
  reader.read ( "E:\\perry\\src\\OsgFox\\Plugins\\MaterialEditor\\config.xml", &dialog );
  dialog.execute ( FX::PLACEMENT_OWNER );
#else
  #ifdef NDEBUG
MaterialEditor dialog ( FoxTools::Functions::mainWindow(), _mat.get() );
dialog.show();
#else
  FX::FXDialogBox dialog ( FoxTools::Functions::mainWindow(), "Material Editor", FX::DECOR_TITLE | FX::DECOR_CLOSE | FX::DECOR_BORDER | FX::DECOR_RESIZE, ZEROS );
  FrameMap frames ( this->_makeFrames ( &dialog ) );
  this->_makeMaterialPalette ( frames[MATERIAL_PALETTE] );
  this->_makeColorSelector   ( frames[COLOR_SELECTOR]   );
  this->_makeColorButtons    ( frames[COLOR_BUTTONS]    );
  this->_makeActionButtons   ( frames[ACTION_BUTTONS]   );
  dialog.execute ( FX::PLACEMENT_OWNER );
#endif
#endif
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current material.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::setCurrentMaterial ( osg::Material *m )
{
  _mat = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the frames.
//
///////////////////////////////////////////////////////////////////////////////

MaterialEditorComponent::FrameMap MaterialEditorComponent::_makeFrames ( FX::FXComposite *parent )
{
  std::auto_ptr<FX::FXHorizontalFrame> outer ( new FX::FXHorizontalFrame ( parent, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y, ZEROS ) );
  FX::FXHorizontalFrame *innerLeft  ( new FX::FXHorizontalFrame ( outer.get(), FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y, ZEROS ) );
  FX::FXHorizontalFrame *innerRight ( new FX::FXHorizontalFrame ( outer.get(), FX::LAYOUT_FILL_Y, ZEROS ) );
  FX::FXSplitter *splitterLeft ( new FX::FXSplitter ( innerLeft, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::SPLITTER_HORIZONTAL ) );

  FrameMap frames;
  frames[MATERIAL_PALETTE] = new FX::FXVerticalFrame ( splitterLeft,  FX::LAYOUT_FILL_Y | FX::LAYOUT_FILL_X );
  frames[COLOR_SELECTOR]   = new FX::FXVerticalFrame ( splitterLeft,  FX::LAYOUT_FILL_Y | FX::LAYOUT_FILL_X );
  frames[COLOR_BUTTONS]    = new FX::FXVerticalFrame ( innerRight, FX::LAYOUT_FILL_Y );
  frames[ACTION_BUTTONS]   = new FX::FXVerticalFrame ( innerRight, FX::LAYOUT_FILL_Y | FX::LAYOUT_RIGHT, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7 );

#ifdef _DEBUG

  frames[MATERIAL_PALETTE]->setBackColor ( FX::FXVec3f ( 0, 0, 1 ) );
  frames[COLOR_SELECTOR]->  setBackColor ( FX::FXVec3f ( 0, 1, 0 ) );
  frames[COLOR_BUTTONS]->   setBackColor ( FX::FXVec3f ( 0, 1, 1 ) );
  frames[ACTION_BUTTONS]->  setBackColor ( FX::FXVec3f ( 1, 0, 0 ) );
  innerLeft->               setBackColor ( FX::FXVec3f ( 1, 0, 1 ) );
  innerRight->              setBackColor ( FX::FXVec3f ( 1, 1, 0 ) );
  outer->                   setBackColor ( FX::FXVec3f ( 1, 1, 1 ) );

#endif

  outer.release();
  return frames;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a new color selector. Note: this function builds the default color 
//  selector and then proceeds to customize it. This customization largely 
//  depends on the current implementation of FOX's color selector.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::_makeColorSelector ( FX::FXComposite *parent )
{
  // For readability.
  typedef std::auto_ptr < FX::FXColorSelector > ColorSelectorPtr;
  typedef FoxTools::Predicates::IsMemberOf IsMemberOf;
  typedef FoxTools::Predicates::HasText HasText;
  typedef std::vector < FX::FXWindow *     > Windows;
  typedef std::vector < FX::FXLabel *      > Labels;
  typedef std::vector < FX::FXColorWheel * > ColorWheels;
  typedef std::vector < FX::FXColorBar *   > ColorBars;
  typedef FoxTools::Functions::Children < Windows,     FX::FXWindow     > GetWindows;
  typedef FoxTools::Functions::Children < Labels,      FX::FXLabel      > GetLabels;
  typedef FoxTools::Functions::Children < ColorWheels, FX::FXColorWheel > GetColorWheels;
  typedef FoxTools::Functions::Children < ColorBars,   FX::FXColorBar   > GetColorBars;
  typedef FoxTools::Functors::SetText < FX::FXLabel > SetText;

  // Make standard color-selector.
  ColorSelectorPtr selector ( new FX::FXColorSelector ( parent, this ) );

  // Add these flags.
  selector->setLayoutHints ( selector->getLayoutHints() | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

  // Include slider for alpha channel.
  selector->setOpaqueOnly ( FALSE );

  // Set color to be diffuse channel from material.
  const osg::Vec4f c ( ( _mat.valid() ) ? _mat->getDiffuse ( osg::Material::FRONT ) : osg::Vec4f ( 0, 0, 0, 1 ) );
  selector->setRGBA ( FX::FXVec4f ( c[0], c[1], c[2], c[3] ) );

  // We don't want to see these...
  selector->acceptButton()->hide();
  selector->cancelButton()->hide();

  // Get all the child windows and also the labels.
  Windows windows ( GetWindows::get ( selector.get() ) );
  Labels labels   ( GetLabels::get  ( selector.get(), true ) );

  // This should hide the custom-color palette.
  std::for_each ( windows.begin(), windows.end(), Usul::Functors::ifThen ( IsMemberOf ( FXMETACLASS ( FX::FXHorizontalSeparator ), true, 0 ), std::mem_fun ( &FX::FXWindow::hide ) ) );
  std::for_each ( windows.begin(), windows.end(), Usul::Functors::ifThen ( IsMemberOf ( FXMETACLASS ( FX::FXHorizontalFrame     ), true, 1 ), std::mem_fun ( &FX::FXWindow::hide ) ) );

  // Change these labels.
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Red:"        ), SetText ( "R:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Green:"      ), SetText ( "G:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Blue:"       ), SetText ( "B:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Alpha:"      ), SetText ( "A:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Hue:"        ), SetText ( "H:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Saturation:" ), SetText ( "S:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Value:"      ), SetText ( "V:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Cyan:"       ), SetText ( "C:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Magenta:"    ), SetText ( "M:" ) ) );
  std::for_each ( labels.begin(), labels.end(), Usul::Functors::ifThen ( HasText ( "Yellow:"     ), SetText ( "Y:" ) ) );

  // Get the color wheel and slider.
  ColorWheels cw ( GetColorWheels::get ( selector.get(), true ) );
  ColorBars   cb ( GetColorBars::get   ( selector.get(), true ) );
  cw.at(0)->setLayoutHints ( FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y    | FX::LAYOUT_SIDE_TOP | FX::LAYOUT_SIDE_LEFT );
  cb.at(0)->setLayoutHints ( FX::LAYOUT_RIGHT  | FX::LAYOUT_FIX_WIDTH | FX::LAYOUT_FILL_Y );

  // Release the color-selector.
  selector.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the buttons.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::_makeActionButtons ( FX::FXComposite *parent )
{
  const unsigned int width ( 70 );
  const unsigned int layout ( FX::BUTTON_NORMAL | FX::LAYOUT_FIX_WIDTH | FX::LAYOUT_CENTER_X );
  new FX::FXButton ( parent, "Apply",   0x0, 0x0, 0, layout, 0, 0, width );
  new FX::FXButton ( parent, "OK",      0x0, 0x0, 0, layout, 0, 0, width );
  new FX::FXButton ( parent, "Cancel",  0x0, 0x0, 0, layout, 0, 0, width );
  new FX::FXButton ( parent, "New",     0x0, 0x0, 0, layout, 0, 0, width );
  new FX::FXButton ( parent, "Reset",   0x0, 0x0, 0, layout, 0, 0, width );
  new FX::FXButton ( parent, "Default", 0x0, 0x0, 0, layout, 0, 0, width );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the color-channel buttons.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::_makeColorButtons ( FX::FXComposite *parent )
{
  this->_makeColorButton ( parent, "Ambient"  );
  this->_makeColorButton ( parent, "Diffuse"  );
  this->_makeColorButton ( parent, "Specular" );
  this->_makeColorButton ( parent, "Emissive" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a single color-channel button.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::_makeColorButton ( FX::FXComposite *parent, const std::string &text )
{
  std::auto_ptr<FX::FXHorizontalFrame> button ( new FX::FXHorizontalFrame ( parent, FX::LAYOUT_NORMAL, ZEROS ) );
  new FX::FXRadioButton ( button.get(), "",  0x0, 0 );
  new FX::FXCheckButton ( button.get(), text.c_str(),  0x0, 0 );
  button.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the material palette.
//
///////////////////////////////////////////////////////////////////////////////

void MaterialEditorComponent::_makeMaterialPalette ( FX::FXComposite *parent )
{
  OsgTools::ShapeFactory::Ptr factory ( new OsgTools::ShapeFactory );
  std::auto_ptr<FX::FXScrollWindow> scroll ( new FX::FXScrollWindow ( parent, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y ) );
  FX::FXVerticalFrame *vf ( new FX::FXVerticalFrame ( scroll.get(), FX::LAYOUT_FILL_X ) );
  //const unsigned int numColumns ( 4 );
  const unsigned int size ( 50 );
  //FX::FXMatrix *matrix ( new FX::FXMatrix ( scroll.get(), numColumns, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::MATRIX_BY_COLUMNS ) );
  Usul::Adaptors::Random<float> random ( 0, 1 );
  for ( unsigned int i = 0; i < 100; ++i )
  {
    //FX::FXGLCanvas *view ( new FX::FXGLCanvas ( scroll.get(), FoxTools::Functions::visual() ) );
    FX::FXHorizontalFrame *hf ( new FX::FXHorizontalFrame ( vf, FX::LAYOUT_FILL_X | FX::LAYOUT_FIX_HEIGHT, 0, 0, 0, size ) );
    //OsgFox::Views::View *view ( new OsgFox::Views::View ( 0x0, hf, FoxTools::Functions::visual() ) );
    hf->setBackColor ( FX::normalize ( FX::FXVec3f ( random(), random(), random() ) ) );
    //view->setWidth  ( size );
    //view->setHeight ( size );
#if 0
    view->setLayoutHints ( FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );
    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
    geode->addDrawable ( factory->sphere ( 1.0f, OsgTools::ShapeFactory::MeshSize ( 50, 50 ) ) );
    osg::ref_ptr<osg::Material> mat ( new osg::Material );
    mat->setDiffuse ( osg::Material::FRONT, osg::Vec4f ( random(), random(), random(), 1 ) );
    mat->setAmbient ( osg::Material::FRONT, mat->getDiffuse ( osg::Material::FRONT ) );
    geode->getOrCreateStateSet()->setAttribute ( mat.get() );
    view->scene ( geode.get() );
    view->addChoreCameraFit();
#endif
  }
  scroll.release();
}
