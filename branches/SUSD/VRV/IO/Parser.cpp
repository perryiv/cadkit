
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Restart-file parser.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Parser.h"

#include "VRV/Interfaces/IRequestRead.h"
#include "VRV/Interfaces/IModelCaching.h"

#include "Usul/Adaptors/MemberFunction.h"

#include "Units/Length.h"

#include "XmlTools/Reader.h"
#include "XmlTools/Policy.h"

using namespace VRV;
using namespace VRV::IO;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Parser, Parser::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser() : BaseClass(), 
  _caller   ( 0x0 ),
  _files    (),
  _dirs     (),
  _matrices (),
  _rotation ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::~Parser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Parser::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case VRV::Interfaces::IParseRestart::IID:
    return static_cast<VRV::Interfaces::IParseRestart*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<VRV::Interfaces::IParseRestart*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore default values.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_reset()
{
  // Reset all members.
  _caller = 0x0;
  _files.reset();
  _dirs.reset();
  _matrices.reset();
  _rotation.reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the restart-file. The caller should support IRequestRead.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::parseRestartFile ( const std::string &filename, Unknown *caller )
{
  // Reset all state variables to defaults.
  this->_reset();

  // Make sure we can get the interface we need.
  VRV::Interfaces::IRequestRead::ValidQueryPtr append ( caller );

  // Set these members.
  _files.restart ( filename );
  _caller = caller;

  // Call the other one.
  this->_read();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add multiple root nodes. This is to support legacy 
//  "CadViewer" restart files.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template
  <
    class XmlReaderType, 
    class KindOfCallback, 
    class TheCallbackType
  > 
  inline void addCallback ( XmlReaderType &reader,   const KindOfCallback &kind, 
                            const std::string &path, const TheCallbackType &callback )
  {
    reader.add ( kind, std::string ( "CadViewer/" ) + path, callback );
    reader.add ( kind, std::string ( "cv/" )        + path, callback );
    reader.add ( kind, std::string ( "pgv/" )       + path, callback );
    reader.add ( kind, std::string ( "vrv/" )       + path, callback );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read the data from file.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_read()
{
  // Define the xml-reader type.
  typedef void (Parser::*Function) ( const std::string & );
  typedef Usul::Adaptors::MemberFunction < Parser::RefPtr, Function > MemFun;
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorPolicy;
  typedef XML::Reader < MemFun, XML::ValueCB, ErrorPolicy > XmlReader;

  // Make some member function adaptors.
  MemFun setEnvDir    ( this, &Parser::_setEnvDir    );
  MemFun setEnvUnits  ( this, &Parser::_setEnvUnits  );
  MemFun setEnvCache  ( this, &Parser::_setEnvCache  );
  MemFun modelStart   ( this, &Parser::_modelStart   );
  MemFun setModelDir  ( this, &Parser::_setModelDir  );
  MemFun preMult      ( this, &Parser::_preMult      );
  MemFun setRotDeg    ( this, &Parser::_setRotDeg    );
  MemFun setRotRad    ( this, &Parser::_setRotRad    );
  MemFun setRotAxis   ( this, &Parser::_setRotAxis   );
  MemFun rotate       ( this, &Parser::_rotate       );
  MemFun translate    ( this, &Parser::_translate    );
  MemFun scale        ( this, &Parser::_scale        );
  MemFun setModelFile ( this, &Parser::_setModelFile );
  MemFun modelFinish  ( this, &Parser::_modelFinish  );

  // Declare the reader and add the callbacks.
  XmlReader reader;
  Helper::addCallback ( reader, XmlReader::NODE_START,  "environment/directory", setEnvDir    );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "environment/units",     setEnvUnits  );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "environment/cache",     setEnvCache  );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model",                 modelStart   );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/directory",       setModelDir  );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/matrix",          preMult      );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/rotate/degrees",  setRotDeg    );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/rotate/radians",  setRotRad    );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/rotate/axis",     setRotAxis   );
  Helper::addCallback ( reader, XmlReader::NODE_FINISH, "model/rotate",          rotate       );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/translate",       translate    );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/scale",           scale        );
  Helper::addCallback ( reader, XmlReader::NODE_START,  "model/filename",        setModelFile );
  Helper::addCallback ( reader, XmlReader::NODE_FINISH, "model",                 modelFinish  );

  // User feedback.
  this->_progress ( "Reading file: " + _files.restart() );

  // Read the file.
  try
  {
    reader.read ( _files.restart() );
  }

  // Catch exceptions from the reader and throw a better one.
  catch ( const XML::Error &e )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 3304520755, failed to read file.",
        "\n\tFile:     ", _files.restart(),
        "\n\tLine:     ", e.line(),
        "\n\tPosition: ", e.position(),
        "\n\tMessage:  ", e.message() );
  }

  // User feedback.
  this->_progress ( "Done reading: " + _files.restart() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called at the start of a model section.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_modelStart ( const std::string & )
{
  _files.modelStart();
  _dirs.modelStart();
  _matrices.modelStart();
  _rotation.modelStart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called at the end of a model section.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_modelFinish ( const std::string & )
{
  // Get the interface we need.
  VRV::Interfaces::IRequestRead::ValidQueryPtr request ( _caller );

  // Append the request.
  std::string path ( _dirs.dir() + _files.model() );
  request->requestRead ( path, _matrices.product() );

  // Tell these members that we've reached the end of a model.
  _files.modelFinish();
  _dirs.modelFinish();
  _matrices.modelFinish();
  _rotation.modelFinish();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the environment's directory.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setEnvDir ( const std::string &d )
{
  _dirs.env ( d );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model's directory.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setModelDir ( const std::string &d )
{
  _dirs.model ( d );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the matrix and pre-multiply the current model matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_preMult ( const std::string &s )
{
  // Convert the string to a matrix.
  std::istringstream in ( s );
  Parser::Matrix m;
  for ( unsigned int i = 0; i < 16; ++i )
  {
    // Make sure there is more.
    if ( in.fail() || in.eof() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 2094140020, corrupt matrix in file.",
          "\n\tFile:   ", _files.restart(),
          "\n\tMatrix: ", s );
    }

    // Set this element.
    in >> m[i];
  }

  // Pre-multiply the current model matrix.
  this->_preMult ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-multiply the current model matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_preMult ( const Parser::Matrix &m )
{
  _matrices.model ( m * _matrices.model() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the units into a scale matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_convertUnits ( const std::string &units, Matrix &matrix )
{
  // Get the conversion to feet.
  float scale ( Units::Length::convert ( units, "ft", 1.0f ) );

  // Make a scale-matrix.
  matrix.makeScale ( Vec3 ( scale, scale, scale ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the units.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setEnvUnits ( const std::string &units )
{
  // Get the scale matrix.
  Matrix scale;
  this->_convertUnits ( units, scale );

  // We post-multiply because we want the unit-scale to always come first.
  _matrices.env ( _matrices.env() * scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model-caching mode. Model-caching is the association between 
//  filename and model. If caching is turned on, the second time there is a 
//  request to read the same filename, the first "cached" model will be 
//  cloned rather than pulled off the disk again. There may be times when 
//  this is undesirable, e.g., if the model has changed since the last read.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setEnvCache ( const std::string &str )
{
  // See if the caller supports caching.
  VRV::Interfaces::IModelCaching::QueryPtr cache ( _caller );
  if ( cache.valid() )
  {
    // Make a copy and make it lower-case.
    std::string s ( str );
    std::transform ( s.begin(), s.end(), s.begin(), ::tolower );

    // Read the value and set the mode.
    if      ( ( s == "true" )  || ( s == "yes" ) || ( s == "1" ) )
      cache->setModelCachingMode ( true );
    else if ( ( s == "false" ) || ( s == "no"  ) || ( s == "0" ) )
      cache->setModelCachingMode ( false );
    else
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 1298049675, invalid caching mode.",
          "\n\tFile: ", _files.restart(),
          "\n\tMode: ", str );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the angle of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setRotDeg ( const std::string &s )
{
  float deg ( ( s.empty() ) ? 0 : ::atof ( s.c_str() ) );
  _rotation.degrees ( deg );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the angle of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setRotRad ( const std::string &s )
{
  float rad ( ( s.empty() ) ? 0 : ::atof ( s.c_str() ) );
  _rotation.radians ( rad );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string to a 3D vector.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_convert ( const std::string &s, Vec3 &v ) const
{
  // Convert the string to a vector.
  std::istringstream in ( s );
  for ( unsigned int i = 0; i < 3; ++i )
  {
    // Make sure there is more.
    if ( in.fail() || in.eof() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 4288370898, corrupt 3D vector in file.",
          "\n\tFile:   ", _files.restart(),
          "\n\tVector: ", s );
    }

    // Set this element.
    in >> v[i];
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the axis of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setRotAxis ( const std::string &s )
{
  // Convert the string to a vector.
  Vec3 axis;
  this->_convert ( s, axis );

  // Set the axis.
  _rotation.axis ( axis );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotate the model.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_rotate ( const std::string & )
{
  // Get the rotation matrix.
  Matrix rotation;
  _rotation.matrix ( rotation );

  // Pre-multiply the current matrix so that transformations happen in the 
  // order they appear in the file.
  _matrices.model ( rotation * _matrices.model() );

  // Reset the rotation.
  _rotation.reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the model.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_translate ( const std::string &s )
{
  // Convert the string to a vector.
  Vec3 t;
  this->_convert ( s, t );

  // Make a translation matrix.
  Matrix trans;
  trans.makeTranslation ( t );

  // Pre-multiply the current matrix so that transformations happen in the 
  // order they appear in the file.
  _matrices.model ( trans * _matrices.model() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale the model.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_scale ( const std::string &s )
{
  // Convert the string to a vector.
  Vec3 v;
  this->_convert ( s, v );

  // Make a scale matrix.
  Matrix scale;
  scale.makeScale ( v );

  // Pre-multiply the current matrix so that transformations happen in the 
  // order they appear in the file.
  _matrices.model ( scale * _matrices.model() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model filename.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setModelFile ( const std::string &filename )
{
  _files.model ( filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Report progress.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_progress ( const std::string &message )
{
  // TODO
}
