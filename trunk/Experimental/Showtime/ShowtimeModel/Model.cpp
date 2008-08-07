
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Model for showtime.
//
///////////////////////////////////////////////////////////////////////////////

#include "ShowtimeModel/Model.h"

#include "OsgTools/Group.h"

#include "XmlTree/Node.h"

#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Trace/Trace.h"

using namespace Showtime;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Model::Model ( const std::string &file, const XmlTree::Node *node ) : BaseClass(),
  _file       ( file ),
  _root       ( new osg::Group ),
  _model      ( new osg::Group ),
  _document   ( 0x0 ),
  _attributes ()
{
  USUL_TRACE_SCOPE;

  // Check input.
  if ( 0x0 == node )
    throw std::invalid_argument ( "Error 2040720371: null node passed when initializing model" );

  // Set members.
  this->name ( node->attribute ( "name" ) );
  _attributes = node->attributes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Model::~Model()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Model::_destroy ), "5133058710" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Model::_destroy()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Clear members.
  _file.clear();
  _root = 0x0;
  _model = 0x0;
  _document = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene root.
//
///////////////////////////////////////////////////////////////////////////////

Model::GroupPtr Model::root()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _root;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the file now if needed.
//
///////////////////////////////////////////////////////////////////////////////

void Model::load ( Usul::Interfaces::IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle repeated calls.
  if ( _model->getNumChildren() > 0 )
    return;

  // Check state.
  if ( true == _file.empty() )
    throw std::invalid_argument ( "Error 4214169324: given file name is empty" );
  if ( false == Usul::Predicates::FileExists::test ( _file ) )
    throw std::invalid_argument ( Usul::Strings::format ( "Error 1132431083: given file '", _file, "' does not exist" ) );

  // Find a document that can open the file.
  if ( false == _document.valid() )
  {
    Usul::Documents::Manager::DocumentInfo info ( Usul::Documents::Manager::instance().find ( _file, 0x0 ) );
    _document = info.document;
  }

  // Make sure a document was found.
  if ( false == _document.valid() )
    throw std::runtime_error ( Usul::Strings::format ( "Error 3573132664: failed to find document for file: ", _file ) );

  // Open the document.
  _document->open ( _file, 0x0, progress );

  // Build the scene.
  Usul::Interfaces::IBuildScene::QueryPtr build ( _document );
  osg::ref_ptr<osg::Node> scene ( ( true == build.valid() ) ? build->buildScene ( Usul::Interfaces::IBuildScene::Options(), progress ) : 0x0 );
  if ( false == scene.valid() )
    throw std::runtime_error ( Usul::Strings::format ( "Error 3368131465: failed to build scene for file: ", _file ) );

  // Add the new scene.
  OsgTools::Group::removeAllChildren ( _model.get() );
  _model->addChild ( scene.get() );

  // Should the model be wireframe?
  if ( "wireframe" == _attributes["polygons"] )
  {
    OsgTools::State::StateSet::setPolygonsLines ( _model.get(), true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unload the model.
//
///////////////////////////////////////////////////////////////////////////////

void Model::unload()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Remove the scene.
  OsgTools::Group::removeAllChildren ( _model.get() );

  // Remove the document.
  if ( true == _document.valid() )
  {
    _document->clear();
    _document = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the model.
//
///////////////////////////////////////////////////////////////////////////////

void Model::show()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make sure it's loaded.
  this->load();

  // Attach the scene if we should.
  if ( true == _model->getParents().empty() )
  {
    _root->addChild ( _model.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the model.
//
///////////////////////////////////////////////////////////////////////////////

void Model::hide()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  OsgTools::Group::removeAllChildren ( _root.get() );
}
