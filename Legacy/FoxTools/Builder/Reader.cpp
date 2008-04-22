
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Window builder class.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Builder/Reader.h"
#include "FoxTools/Builder/Factory.h"

#include "XmlDom/Policy.h"
#include "XmlDom/Reader.h"
#include "XmlDom/File.h"
#include "XmlDom/Convert.h"

#include "Usul/Strings/Case.h"
#include "Usul/Cast/Cast.h"

#include "fx.h"
#include "fx3d.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

#include <stdexcept>
#include <list>

using namespace FoxTools;
using namespace FoxTools::Builder;


///////////////////////////////////////////////////////////////////////////////
//
//  Handy macro to register factories.
//
///////////////////////////////////////////////////////////////////////////////

#define REGISTER_FACTORY(factory_type,parent_type,class_type)\
  _factories[#class_type] = new factory_type < parent_type, class_type >


///////////////////////////////////////////////////////////////////////////////
//
//  Handy macro to register layout hints.
//
///////////////////////////////////////////////////////////////////////////////

#define REGISTER_LAYOUT_HINT(hint_name)\
  _hints[#hint_name] = hint_name


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Reader::Reader() :
  _file(),
  _stack(),
  _factories(),
  _hints()
{
  // Factories.
  REGISTER_FACTORY ( NeedsOwnerAndString, FX::FXComposite, FX::FXDialogBox );
  REGISTER_FACTORY ( NeedsOwnerAndString, FX::FXComposite, FX::FXLabel );
  REGISTER_FACTORY ( NeedsParent, FX::FXComposite, FX::FXHorizontalFrame );
  REGISTER_FACTORY ( NeedsParent, FX::FXComposite, FX::FXVerticalFrame );
  REGISTER_FACTORY ( NeedsParent, FX::FXComposite, FX::FXHorizontalSeparator );
  REGISTER_FACTORY ( NeedsParent, FX::FXComposite, FX::FXVerticalSeparator );

  // Layout hints.
  REGISTER_LAYOUT_HINT ( LAYOUT_NORMAL );
  REGISTER_LAYOUT_HINT ( LAYOUT_SIDE_TOP );
  REGISTER_LAYOUT_HINT ( LAYOUT_SIDE_BOTTOM );
  REGISTER_LAYOUT_HINT ( LAYOUT_SIDE_LEFT );
  REGISTER_LAYOUT_HINT ( LAYOUT_SIDE_RIGHT );
  REGISTER_LAYOUT_HINT ( LAYOUT_FILL_COLUMN );
  REGISTER_LAYOUT_HINT ( LAYOUT_FILL_ROW );
  REGISTER_LAYOUT_HINT ( LAYOUT_LEFT );
  REGISTER_LAYOUT_HINT ( LAYOUT_RIGHT );
  REGISTER_LAYOUT_HINT ( LAYOUT_CENTER_X );
  REGISTER_LAYOUT_HINT ( LAYOUT_FIX_X );
  REGISTER_LAYOUT_HINT ( LAYOUT_TOP );
  REGISTER_LAYOUT_HINT ( LAYOUT_BOTTOM );
  REGISTER_LAYOUT_HINT ( LAYOUT_CENTER_Y );
  REGISTER_LAYOUT_HINT ( LAYOUT_FIX_Y );
  REGISTER_LAYOUT_HINT ( LAYOUT_DOCK_SAME );
  REGISTER_LAYOUT_HINT ( LAYOUT_DOCK_NEXT );
  REGISTER_LAYOUT_HINT ( LAYOUT_FIX_WIDTH );
  REGISTER_LAYOUT_HINT ( LAYOUT_FIX_HEIGHT );
  REGISTER_LAYOUT_HINT ( LAYOUT_MIN_WIDTH );
  REGISTER_LAYOUT_HINT ( LAYOUT_MIN_HEIGHT );
  REGISTER_LAYOUT_HINT ( LAYOUT_FILL_X );
  REGISTER_LAYOUT_HINT ( LAYOUT_FILL_Y );
  REGISTER_LAYOUT_HINT ( LAYOUT_FILL );
  REGISTER_LAYOUT_HINT ( LAYOUT_EXPLICIT );
  REGISTER_LAYOUT_HINT ( FRAME_NONE );
  REGISTER_LAYOUT_HINT ( FRAME_SUNKEN );
  REGISTER_LAYOUT_HINT ( FRAME_RAISED );
  REGISTER_LAYOUT_HINT ( FRAME_THICK );
  REGISTER_LAYOUT_HINT ( FRAME_GROOVE );
  REGISTER_LAYOUT_HINT ( FRAME_RIDGE );
  REGISTER_LAYOUT_HINT ( FRAME_LINE );
  REGISTER_LAYOUT_HINT ( FRAME_NORMAL );
  REGISTER_LAYOUT_HINT ( PACK_NORMAL );
  REGISTER_LAYOUT_HINT ( PACK_UNIFORM_HEIGHT );
  REGISTER_LAYOUT_HINT ( PACK_UNIFORM_WIDTH );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Reader::~Reader()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the xml file and build the widget tree.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::read ( const std::string &name, FX::FXWindow *parent )
{
  // Typedefs.
  typedef XML::Config::Policy<>                     Policy;
  typedef XML::Reader < Policy, Reader::Callback >  XmlReader;

  // Clear existing state.
  this->_clear();

  // Set the root.
  _stack.push ( parent );

  // Open the file.
  XML::File < Policy > file ( name );

  // Declare a reader and read the file.
  XmlReader reader;
  reader.callback().set ( this );
  reader.read ( file.begin(), file.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor for callback for all nodes.
//
///////////////////////////////////////////////////////////////////////////////

Reader::Callback::Callback() : _r ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback's reader.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::Callback::set ( Reader *r )
{
  if ( 0x0 == r )
    throw std::runtime_error ( "Error 1548387055: Null reader given" );
  _r = r;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear any internal state.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::Callback::clear()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called at the start of each node.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::Callback::start ( const std::string &name, const std::string &value )
{
  _r->_start ( name, value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called at the end of each node.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::Callback::end ( const std::string &name )
{
  _r->_end ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the internal state.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_clear()
{
  _file.clear();
  while ( false == _stack.empty() )
    _stack.pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called at the start of each node.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_start ( const std::string &name, const std::string &value )
{
  // Look for a factory.
  Factory *f ( this->_factory ( name ) );
  if ( f )
  {
    _stack.push ( this->_create ( name, f ) );
    return;
  }

  // It must be a parameter.
  const std::string param ( Usul::Strings::lowerCase ( name ) );
  if ( "layout" == param )
    this->_layout ( value );
  if ( "text" == param )
    this->_text ( value );
  if ( "title" == param )
    this->_title ( value );
  if ( "color" == param )
    this->_color ( value );
  else if ( "width" == param )
    this->_current()->setWidth ( XML::Convert<FX::FXint>() ( value ) );
  else if ( "height" == param )
    this->_current()->setHeight ( XML::Convert<FX::FXint>() ( value ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called at the end of each node.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_end ( const std::string &name )
{
  if ( this->_factory ( name ) )
    _stack.pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current window.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXWindow *Reader::_current()
{
  return ( _stack.empty() ) ? 0x0 : _stack.top();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new window of the given type.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXWindow *Reader::_create ( const std::string &name, Factory *f )
{
  FX::FXWindow *parent ( this->_current() );
  return ( f && parent ) ? f->create ( parent ) : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the factory.
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Builder::Factory *Reader::_factory ( const std::string &name )
{
  Factories::iterator i = _factories.find ( name );
  return ( _factories.end() == i ) ? 0x0 : i->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layout.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_layout ( const std::string &value )
{
  // Split the string.
  typedef std::list<std::string> Hints;
  const std::string layout ( Usul::Strings::upperCase ( value ) );
  Hints hints;
  boost::algorithm::split ( hints, layout, boost::algorithm::is_any_of ( " |" ) );

  // Loop through the hints and add them to the window.
  for ( Hints::const_iterator i = hints.begin(); i != hints.end(); ++i )
    this->_current()->setLayoutHints ( this->_current()->getLayoutHints() | _hints[*i] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_text ( const std::string &value )
{
  FX::FXWindow *window ( this->_current() );
  FX::FXLabel *label ( SAFE_CAST_FOX ( FX::FXLabel, window ) );
  if ( label )
    label->setText ( value.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the title.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_title ( const std::string &value )
{
  FX::FXWindow *window ( this->_current() );
  FX::FXTopWindow *top ( SAFE_CAST_FOX ( FX::FXTopWindow, window ) );
  if ( top )
    top->setTitle ( value.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_color ( const std::string &value )
{
  this->_current()->setBackColor ( XML::ConvertVec3<FX::FXVec3f>() ( value ) );
}
