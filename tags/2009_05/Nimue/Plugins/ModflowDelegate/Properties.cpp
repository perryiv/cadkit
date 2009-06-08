
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Dialog for most modflow properties.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/Properties.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Named.h"
#include "Usul/Scope/Caller.h"

using namespace Modflow;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Properties::Properties ( QWidget *parent  ) : BaseClass ( parent ),
  _document ( static_cast < IModflowModel* > ( 0x0 ) )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  // Connect slots.
  this->_slotsConnect();

#ifndef _DEBUG
  // Hide cell margin control until the bug is fixed.
  _cellMarginSpinBox->hide();
  _cellMarginLabel->hide();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Properties::~Properties()
{
  _document = static_cast < IModflowModel* > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void Properties::document ( Usul::Interfaces::IUnknown *m )
{
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Disconnect the slots and make sure they are re-connected.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( 
    Usul::Adaptors::memberFunction ( this, &Properties::_slotsDisconnect ),
    Usul::Adaptors::memberFunction ( this, &Properties::_slotsConnect ) ) );

  // Set the document.
  _document = m;

  // Set the values.
  _verticalScaleSpinBox->setValue ( ( true == _document.valid() ) ? _document->getVerticalScale() : 1 );
  _cellMarginSpinBox->setValue    ( ( true == _document.valid() ) ? _document->getCellMargin()    : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots.
//
///////////////////////////////////////////////////////////////////////////////

void Properties::_slotsConnect()
{
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  QObject::connect ( _verticalScaleSpinBox, SIGNAL ( valueChanged ( double ) ), this, SLOT ( _verticalScaleChanged ( double ) ) );
  QObject::connect ( _cellMarginSpinBox,    SIGNAL ( editingFinished ( )     ), this, SLOT ( _cellMarginChanged()             ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disconnect slots.
//
///////////////////////////////////////////////////////////////////////////////

void Properties::_slotsDisconnect()
{
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  _verticalScaleSpinBox->disconnect ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The property changed.
//
///////////////////////////////////////////////////////////////////////////////

void Properties::_verticalScaleChanged ( double value )
{
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  if ( true == _document.valid() )
  {
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( _document.get(), &IModflowModel::setVerticalScale ), value, "1014248424" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The property changed.
//
///////////////////////////////////////////////////////////////////////////////

void Properties::_cellMarginChanged()
{
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  if ( true == _document.valid() )
  {
    const double value ( _cellMarginSpinBox->value() );
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( _document.get(), &IModflowModel::setCellMargin ), value, "3932338238" );
  }
}
