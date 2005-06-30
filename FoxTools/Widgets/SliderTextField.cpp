
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Widgets/SliderTextField.h"

#include "FoxTools/Headers/Slider.h"
#include "FoxTools/Headers/TextField.h"
#include "FoxTools/Headers/Label.h"

#include "Usul/Headers/SStream.h"
#include <stdexcept>

using namespace FoxTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( SliderTextField ) MessageMap[] =
{
  // Message type,                     id,                                handler.
  FXMAPFUNC ( FX::SEL_COMMAND,         SliderTextField::ID_SLIDER,         SliderTextField::onCommandSlider  ),
  FXMAPFUNC ( FX::SEL_CHANGED,         SliderTextField::ID_SLIDER,         SliderTextField::onChangeSlider  ),
  FXMAPFUNC ( FX::SEL_COMMAND,         SliderTextField::ID_TEXT,           SliderTextField::onCommandText   ),
};

// This class, base class, array of message handlers, size of the array.
FXIMPLEMENT ( SliderTextField, BaseClass, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

SliderTextField::SliderTextField ( FX::FXComposite *parent, FX::FXObject* tgt, FX::FXSelector sel, int start, int finish ) : 
BaseClass ( parent, FX::LAYOUT_FILL_X ),
_start ( start ),
_finish ( finish ),
_slider ( 0x0 ),
_textField ( 0x0 ) 
{
  if( finish < start )
    throw std::runtime_error ( "Error 1226126650: Finish value is less than starting value. " );

  this->setTarget( tgt );
  this->setSelector ( sel );

  _slider = new FX::FXSlider ( this, this, SliderTextField::ID_SLIDER, FX::SLIDER_NORMAL | FX::LAYOUT_FILL_X );
  _slider->setRange( start, finish );
  _slider->setValue( start );

  _textField = new FX::FXTextField ( this, 3, this, SliderTextField::ID_TEXT );

  std::ostringstream os;
  os << start;
  _textField->setText ( os.str().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

SliderTextField::SliderTextField ( FX::FXComposite *parent, char* label, FX::FXObject* tgt, FX::FXSelector sel, int start, int finish ) : 
BaseClass ( parent, FX::LAYOUT_FILL_X ),
_start ( start ),
_finish ( finish ),
_slider ( 0x0 ),
_textField ( 0x0 ) 
{
  if( finish < start )
    throw std::runtime_error ( "Error 1226126650: Finish value is less than starting value. " );

  this->setTarget( tgt );
  this->setSelector ( sel );

  new FX::FXLabel( this, label );

  _slider = new FX::FXSlider ( this, this, SliderTextField::ID_SLIDER, FX::SLIDER_HORIZONTAL | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_ROW | FX::LAYOUT_FILL_COLUMN, 0, 0, 200 );
  _slider->setRange( start, finish );
  _slider->setValue( start );
  _slider->setIncrement ( ( finish - start ) / 10 );

  _textField = new FX::FXTextField ( this, 3, this, SliderTextField::ID_TEXT );

  std::ostringstream os;
  os << start;
  _textField->setText ( os.str().c_str() );

  this->layout();
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

SliderTextField::~SliderTextField ()
{
  delete _slider;
  delete _textField;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Value
//
///////////////////////////////////////////////////////////////////////////////

int SliderTextField::getValue() const
{
  return _slider->getValue();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set Value
//
///////////////////////////////////////////////////////////////////////////////

void SliderTextField::setValue ( int value )
{
  _slider->setValue( value );

  std::ostringstream os;
  os << value;
  _textField->setText( os.str().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  New text field value entered
//
///////////////////////////////////////////////////////////////////////////////

long SliderTextField::onCommandText   ( FX::FXObject *, FX::FXSelector, void * )
{
  std::istringstream in ( ( char* ) ( _textField->getText().text() ) );
  int value;
  in >> value;

  if( value > _finish )
    value = _finish;

  if( value < _start )
    value = _start;

  _slider->setValue( value );
  
  std::ostringstream os;
  os << value;
  _textField->setText( os.str().c_str() );


  if(target) 
    target->tryHandle(this,FXSEL(FX::SEL_COMMAND,message),(void*) value );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Command handler for slider
//
///////////////////////////////////////////////////////////////////////////////

long SliderTextField::onCommandSlider   ( FX::FXObject *, FX::FXSelector, void * )
{
  if(target) 
    return target->tryHandle(this,FXSEL(FX::SEL_COMMAND,message),(void*) _slider->getValue() );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  The slider has changed
//
///////////////////////////////////////////////////////////////////////////////

long SliderTextField::onChangeSlider ( FX::FXObject *, FX::FXSelector, void * )
{
  std::ostringstream os;
  os << _slider->getValue();

  _textField->setText( os.str().c_str() );

  if(target) 
    target->tryHandle(this,FXSEL(FX::SEL_CHANGED,message),(void*) _slider->getValue() );

  // Handled
  return 1;
}

