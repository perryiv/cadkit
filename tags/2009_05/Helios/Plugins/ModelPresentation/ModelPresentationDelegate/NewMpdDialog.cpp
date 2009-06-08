
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
#include "MpdDialogDefinitions.h"

#include "Usul/Convert/Convert.h"

#include "QtTools/StringsView.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NewMpdDialog::NewMpdDialog ( QWidget *parent ) : BaseClass ( parent ),
_currentSetNumber( 0 )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  QStringList modelTitles;
  modelTitles.push_back( "Name" );
  modelTitles.push_back( "Path");
  _modelsList->setHeaderLabels ( modelTitles );

  QStringList setTitles;
  setTitles.push_back( "Name" );
  setTitles.push_back( "Menu Name");

  _setsList->setHeaderLabels ( setTitles );
  _timesetsList->setHeaderLabels ( setTitles );
  _dynamicList->setHeaderLabels ( setTitles );

  
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
    QtTools::StringsView::Items items = dialog.getItems();

    for( unsigned int i = 0; i < items.size(); ++i )
    {
      QTreeWidgetItem* item ( new QTreeWidgetItem ( _modelsList ) );

      std::string name = items.at( i ).first;
      std::string path = items.at( i ).second;

      item->setText ( 0, name.c_str() );
      item->setText ( 1, path.c_str() );

      MpdDialogDefinitions::Model model;

      model.first = name;
      model.second = path;

      _models.push_back( model );
      _modelsList->addTopLevelItem( item );   

    }

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
  {
    TreeWidgetItems listItems ( _modelsList->selectedItems() );

    _models.clear();
    for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
    {
      QTreeWidgetItem* item ( *iter );
      const int index ( _modelsList->indexOfTopLevelItem( item ) );
      _modelsList->takeTopLevelItem( index );
      delete item;
    }
  }

  {
    TreeWidgetItems listItems ( _modelsList->findItems( "*", Qt::MatchWildcard ) );

    for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
    {
      QTreeWidgetItem* item ( *iter );
      MpdDialogDefinitions::Model model;
      
      model.first = item->text( 0 ).toStdString();
      model.second = item->text( 1 ).toStdString();

      _models.push_back( model );;
      
    }

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

  dialog.setModels( _models );

  std::string setName ( "set" + Usul::Convert::Type< unsigned int, std::string >::convert( _currentSetNumber ) );
  dialog.setName( setName );

  dialog.setMenuName( "Models" );

  if( QDialog::Accepted == dialog.exec() )
  {
    QTreeWidgetItem* item ( new QTreeWidgetItem ( _setsList ) );
    MpdDialogDefinitions::Set set = dialog.getSet();
    item->setText ( 0, set.name.c_str() );
    item->setText ( 1, set.menuName.c_str() );
    _setsList->addTopLevelItem( item );
    _sets.push_back( set );

    ++_currentSetNumber;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewMpdDialog::on_setsRemoveButton_clicked()
{
  //typedef QList< QTreeWidgetItem* > TreeWidgetItems;

  //TreeWidgetItems listItems ( _modelsList->selectedItems() );

  //for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
  //{
  //  QTreeWidgetItem* item ( *iter );
  //  const int index ( _modelsList->indexOfTopLevelItem( item ) );
  //  _setsList->takeTopLevelItem( index );
  //  delete item;
  //}
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

MpdDialogDefinitions::SetList NewMpdDialog::getSets()
{
  return _sets;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

MpdDialogDefinitions::ModelList NewMpdDialog::getModels()
{
  return _models;
}
