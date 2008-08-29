
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "GosgReader.h"

#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Data/Model.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "osgDB/ReadFile"

///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  namespace mf = Minerva::Core::Factory;
  mf::RegisterReader < mf::TypeCreator < GosgReader > > _creator_for_GosgReader ( "Geo OpenSceneGraph (*.gosg)", "*.gosg" );
}


USUL_IMPLEMENT_IUNKNOWN_MEMBERS( GosgReader, GosgReader::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GosgReader::GosgReader() :
  BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GosgReader::~GosgReader()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* GosgReader::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IRead::IID:
    return static_cast < Usul::Interfaces::IRead* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void GosgReader::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Set our name (for the GUI).
  this->name ( filename );

  // Make sure Xerces is initialized.
  XmlTree::XercesLife life;

  // Parse the xml file.
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );

  // Get the components of the file.
  XmlTree::Node::RefPtr name ( doc->child ( "file" ) );
  XmlTree::Node::RefPtr lat ( doc->child ( "latitude" ) );
  XmlTree::Node::RefPtr lon ( doc->child ( "longitude" ) );
  XmlTree::Node::RefPtr alt ( doc->child ( "altitude" ) );

  // Return now if invalid file.
  if ( false == name.valid() || false == lat.valid() || false == lon.valid() )
    return;

  // Get the actual values from the nodes.
  const std::string osgFile ( name->value() );
  const double latitude  ( Usul::Convert::Type<std::string,double>::convert ( lat->value() ) );
  const double longitude ( Usul::Convert::Type<std::string,double>::convert ( lon->value() ) );
  const double altitude  ( alt.valid() ? Usul::Convert::Type<std::string,double>::convert ( alt->value() ) : 0.0 );

  // Create the model class.
  Minerva::Core::Data::Model::RefPtr model ( new Minerva::Core::Data::Model );
 
  // Read the node file and add to the model.
  model->model ( osgDB::readNodeFile ( osgFile ) );

  // Note that Vec3 components are ( longitude, latitude, altitude );
  model->location ( osg::Vec3 ( longitude, latitude, altitude ) );

  // Set the altitude mode to relative to ground.
  model->altitudeMode ( Minerva::Core::Data::Geometry::RELATIVE_TO_GROUND );

  // Conversion to meters.  This could be moved to the xml file.
  model->toMeters ( 1.0 );

  // Add the model.
  this->addGeometry ( model );
}
