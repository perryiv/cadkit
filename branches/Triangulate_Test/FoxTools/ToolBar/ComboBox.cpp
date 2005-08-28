

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/ToolBar/ComboBox.h"

#include "FoxTools/Headers/ComboBox.h"

using namespace FoxTools::ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ComboBox::ComboBox ( const std::string &name, 
                     FX::FXObject *target, 
                     unsigned int selector, 
                     unsigned int token ) : 
  BaseClass ( name, "", "", target, selector, token ),
  _comboBox ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ComboBox::~ComboBox()
{
  if( _comboBox )
  {
    _comboBox->setUserData ( 0x0 );
    delete _comboBox;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the button.
//
///////////////////////////////////////////////////////////////////////////////

void ComboBox::clear()
{
  if( _comboBox )
  {
    _comboBox->setUserData ( 0x0 );
    delete _comboBox;
  }
  _comboBox = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the button.
//
///////////////////////////////////////////////////////////////////////////////

void ComboBox::_build ( FX::FXComposite *parent )
{
  if ( 0x0 == _comboBox )
  {
    std::ostringstream text;
    text << "\t" << this->name() << '\t' << this->description();

    // Make the Combo Box.
    unsigned int layout ( COMBOBOX_REPLACE|FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT );
    _comboBox = new FX::FXComboBox ( parent, 0, this->target(), this->selector(), layout );
    _comboBox->setNumColumns( 12 );
    
    _comboBox->setUserData ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void ComboBox::_create ()
{
  // Create the combo box if we can.
  if ( _comboBox && !_comboBox->id() )
    _comboBox->create();
}

