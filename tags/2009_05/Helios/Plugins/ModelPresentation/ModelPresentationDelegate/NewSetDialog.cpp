
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

#include "NewSetDialog.h"
#include "NewGroupDialog.h"

#include "Usul/Convert/Convert.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NewSetDialog::NewSetDialog ( QWidget *parent ) : BaseClass ( parent ),
_currentGroupNumber( 0 )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  QStringList titles;
  titles.push_back( "Name" );
  titles.push_back( "Models");
  _groupsList->setHeaderLabels ( titles );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

NewSetDialog::~NewSetDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewSetDialog::on_groupsAddButton_clicked()
{
  NewGroupDialog dialog( this );

  dialog.setModels( _models );
  
  std::string groupName ( "group" + Usul::Convert::Type< unsigned int, std::string >::convert( _currentGroupNumber ) );
  dialog.setName( groupName );

  if( QDialog::Accepted == dialog.exec() )
  {
    // Get the group that was created
    MpdDialogDefinitions::Group group = dialog.getGroup();

    // create a tree widget item to store the returns
    QTreeWidgetItem* item ( new QTreeWidgetItem );

    // get the group name
    item->setText( 0, QString( group.first.c_str() ) );

    // parse and set the models list
    std::string modelsList ( "" );
    for( unsigned int i = 0; i < group.second.size(); ++i )
    { 
      modelsList += "|" + group.second.at( i ).first;
    }
    item->setText( 1, QString( modelsList.c_str() ) );

    // add the group to the set's list of groups
    _set.groups.push_back( group );

    // add the group to the tree widget for the dialog
    _groupsList->addTopLevelItem( item );

    ++_currentGroupNumber;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewSetDialog::on_groupsRemoveButton_clicked()
{
  typedef QList< QTreeWidgetItem* > TreeWidgetItems;
  {
    TreeWidgetItems listItems ( _groupsList->selectedItems() );

    for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
    {
      QTreeWidgetItem* item ( *iter );
      const int index ( _groupsList->indexOfTopLevelItem( item ) );
      std::string name = item->text( 0 ).toStdString();

      

      _groupsList->takeTopLevelItem( index );
      delete item;
    }
  }
  {
    
    TreeWidgetItems listItems ( _groupsList->findItems( "*", Qt::MatchWildcard ) );
    std::vector< std::string > names;
    for( TreeWidgetItems::iterator iter = listItems.begin(); iter != listItems.end(); ++iter )
    {
      QTreeWidgetItem* item ( *iter );
      std::string name = item->text( 0 ).toStdString();
      names.push_back( name );
    }
   
    MpdDialogDefinitions::GroupList newGroups;
    for( unsigned int i = 0; i < _set.groups.size(); ++i )
    {
      for( unsigned int j = 0; j < names.size(); ++j )
      {
        if( names.at( j ) == _set.groups.at( i ).first )
        {
          newGroups.push_back( _set.groups.at ( i ) );
        }
      }
    }
    _set.groups.clear();
    _set.groups = newGroups;
}
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewSetDialog::setModels( MpdDialogDefinitions::ModelList models )
{
  _models = models;
}



///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

MpdDialogDefinitions::Set NewSetDialog::getSet()
{
 
  _set.name = _nameText->text().toStdString();
  _set.menuName = _menuNameText->text().toStdString();

  return _set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewSetDialog::setName( const std::string &name )
{
  _nameText->setText( name.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void NewSetDialog::setMenuName( const std::string &name )
{
  _menuNameText->setText( name.c_str() );
}
