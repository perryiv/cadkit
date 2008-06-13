
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/ProjectionDialog.h"

#include "Usul/Components/Manager.h"
#include "Usul/Strings/Format.h"

#include "QtGui/QHeaderView"

#include "ogr_spatialref.h"

using namespace Modflow;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProjectionDialog::ProjectionDialog ( QWidget *parent ) : 
  BaseClass ( parent ),
  _coordinateSystems()
{

  // Initialize code from designer.
  this->setupUi ( this );

  _systems->header()->setHidden ( 1 );

  this->_buildTree();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProjectionDialog::~ProjectionDialog()
{
  for ( CoordinateSystems::iterator iter = _coordinateSystems.begin(); iter != _coordinateSystems.end(); ++iter )
    delete iter->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the selected coordinate system.
//
///////////////////////////////////////////////////////////////////////////////

OGRSpatialReference* ProjectionDialog::coordinateSystem() const
{
  CoordinateSystems::const_iterator iter ( _coordinateSystems.find ( _systems->currentItem() ) );
  return ( iter != _coordinateSystems.end() ? iter->second : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add item.
//
///////////////////////////////////////////////////////////////////////////////

void ProjectionDialog::_addItem ( QTreeWidgetItem* parent, const std::string& name, OGRSpatialReference* srs )
{
  if ( 0x0 != srs )
  {
    // Make the item.
    QTreeWidgetItem *item ( new QTreeWidgetItem ( parent ) );
    item->setText ( 0, name.c_str() );

    parent->addChild ( item );

    // Save the item to code mapping.
    _coordinateSystems.insert ( CoordinateSystems::value_type ( item, srs ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void ProjectionDialog::_buildTree()
{
  // Geographic systems.
  QTreeWidgetItem *geographic ( new QTreeWidgetItem ( _systems ) );
  geographic->setText ( 0, "Geographic (lat/lon)" );

  {
    OGRSpatialReference *srs ( new OGRSpatialReference );
    srs->SetWellKnownGeogCS ( "WGS84" );
    this->_addItem ( geographic, "WGS84", srs );
  }

  {
    OGRSpatialReference *srs ( new OGRSpatialReference );
    srs->SetWellKnownGeogCS ( "WGS872" );
    this->_addItem ( geographic, "WGS72", srs );
  }

  _systems->addTopLevelItem ( geographic );

  // Geographic systems.
  QTreeWidgetItem *utm ( new QTreeWidgetItem ( _systems ) );
  utm->setText ( 0, "UTM" );

  /// WGS 84 UTM.
  for ( int i = 1; i <= 60; ++i )
  {
    OGRSpatialReference *srs ( new OGRSpatialReference );
    std::string name ( Usul::Strings::format ( "UTM ", i, " (WGS84) in northern hemisphere." ) );
    srs->SetProjCS( name.c_str() );
    srs->SetWellKnownGeogCS ( "WGS84" );
    srs->SetUTM ( i, true );

    this->_addItem ( utm, name, srs );
  }

  _systems->addTopLevelItem ( utm );
}
