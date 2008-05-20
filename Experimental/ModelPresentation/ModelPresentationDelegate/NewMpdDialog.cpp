
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
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

#include "NewMpdDialog.h"
#include "AddModelDialog.h"
#include "NewSetDialog.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NewMpdDialog::NewMpdDialog ( QWidget *parent ) : BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  QStringList titles;
  titles.push_back( "Name" );
  titles.push_back( "Path");
  _modelsList->setHeaderLabels ( titles );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

NewMpdDialog::~NewMpdDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewMpdDialog::on_modelsAddButton_clicked()
{
  AddModelDialog dialog( this );

  if( QDialog::Accepted == dialog.exec() )
  {
    QTreeWidgetItem* item ( new QTreeWidgetItem ( _modelsList ) );
    item->setText ( 0, dialog.getNameText().c_str() );
    item->setText ( 1, dialog.getFilePath().c_str() );
    _modelsList->addTopLevelItem( item );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewMpdDialog::on_modelsRemoveButton_clicked()
{
  typedef QList< QTreeWidgetItem* > TreeWidgetItems;

  TreeWidgetItems listItems ( _modelsList->selectedItems() );

  for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
  {
    QTreeWidgetItem* item ( *iter );
    const int index ( _modelsList->indexOfTopLevelItem( item ) );
    _modelsList->takeTopLevelItem( index );
    delete item;
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewMpdDialog::on_setsAddButton_clicked()
{
  NewSetDialog dialog( this );

  if( QDialog::Accepted == dialog.exec() )
  {
   /* QTreeWidgetItem* item ( new QTreeWidgetItem ( _setsList ) );
    item->setText ( 0, dialog.getNameText().c_str() );
    item->setText ( 1, dialog.getFilePath().c_str() );
    _modelsList->addTopLevelItem( item );*/
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewMpdDialog::on_setsRemoveButton_clicked()
{
  typedef QList< QTreeWidgetItem* > TreeWidgetItems;

  TreeWidgetItems listItems ( _modelsList->selectedItems() );

  for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
  {
    QTreeWidgetItem* item ( *iter );
    const int index ( _modelsList->indexOfTopLevelItem( item ) );
    _setsList->takeTopLevelItem( index );
    delete item;
  }
  
}

