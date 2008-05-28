
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

#include "NewGroupDialog.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NewGroupDialog::NewGroupDialog ( QWidget *parent ) : BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  QStringList titles;
  titles.push_back( "Name" );
  titles.push_back( "Path");
  _modelsList->setHeaderLabels ( titles );
  _groupModelsList->setHeaderLabels( titles );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

NewGroupDialog::~NewGroupDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewGroupDialog::setModels( MpdDialogDefinitions::ModelList models )
{
  _models = models;
  for( unsigned int i = 0; i < _models.size(); ++i )
  {
    QTreeWidgetItem* item ( new QTreeWidgetItem ( _modelsList ) );
    item->setText ( 0, _models.at( i ).first.c_str() );
    item->setText ( 1, _models.at( i ).second.c_str() );

    _modelsList->addTopLevelItem( item );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewGroupDialog::on_modelsAddButton_clicked()
{
  typedef QList< QTreeWidgetItem* > TreeWidgetItems;

  TreeWidgetItems listItems ( _modelsList->selectedItems() );

  for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
  {
    QTreeWidgetItem* item ( *iter );
    const int index ( _modelsList->indexOfTopLevelItem( item ) );
   
    _groupModelsList->addTopLevelItem( _modelsList->takeTopLevelItem( index ) );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewGroupDialog::on_groupsRemoveButton_clicked()
{
  typedef QList< QTreeWidgetItem* > TreeWidgetItems;

  TreeWidgetItems listItems ( _groupModelsList->selectedItems() );

  for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
  {
    QTreeWidgetItem* item ( *iter );
    const int index ( _groupModelsList->indexOfTopLevelItem( item ) );
    
    _modelsList->addTopLevelItem( _groupModelsList->takeTopLevelItem( index ) );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

std::string NewGroupDialog::getName()
{
  return _nameText->text().toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

QTreeWidget* NewGroupDialog::getGroupModels()
{
  return _groupModelsList;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

MpdDialogDefinitions::Group NewGroupDialog::getGroup()
{
  typedef QList< QTreeWidgetItem* > TreeWidgetItems;

  MpdDialogDefinitions::Group group;
  group.first = this->getName();
  MpdDialogDefinitions::ModelList modelList;

  TreeWidgetItems listItems ( _groupModelsList->findItems( "*", Qt::MatchWildcard ) );

  for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
  {
    QTreeWidgetItem* item ( *iter );
    MpdDialogDefinitions::Model model;
    
    model.first = item->text( 0 ).toStdString();
    model.second = item->text( 1 ).toStdString();

    modelList.push_back( model );
    
  }

  group.second = modelList;
  return group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewGroupDialog::setName( const std::string &name )
{
  _nameText->setText( name.c_str() );
}
