
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Qt/Widgets/OptionsDialog.h"
#include "ui_OptionsDialog.h"


using namespace Minerva::Widgets;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsDialog ( const Options& options, QWidget* parent ) : BaseClass ( parent ),
  _optionsDialog ( new Ui::OptionsDialog ),
  _optionsTreeWidget ( 0x0 )
{
  _optionsDialog->setupUi( this );
  
  // Save a handle to the widget.
  _optionsTreeWidget = _optionsDialog->_optionsTreeWidget;
  
  if ( 0x0 != _optionsTreeWidget )
  {
    for ( Options::const_iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      _optionsTreeWidget->addItem ( iter->first, iter->second );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::~OptionsDialog()
{
  _optionsTreeWidget = 0x0;
  delete _optionsDialog;
  _optionsDialog = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::Options OptionsDialog::options() const
{
  Options options;
  
  if ( 0x0 != _optionsTreeWidget )
  {
    typedef QtTools::StringsView::Items Items;
    Items values ( _optionsTreeWidget->items() );
  
    for ( Items::const_iterator iter = values.begin(); iter != values.end(); ++iter )
    {
      options.insert ( *iter );
    }
  }
  
  return options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a row.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::on_addRowButton_clicked()
{
  if ( 0x0 != _optionsTreeWidget )
    _optionsTreeWidget->addItem ( "Key", "Value" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a row.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::on_removeRowButton_clicked()
{
  if ( 0x0 != _optionsTreeWidget )
    _optionsTreeWidget->removeRow ( _optionsTreeWidget->currentRow() );
}
