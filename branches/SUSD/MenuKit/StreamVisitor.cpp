
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor that prints the hierarchy to the given stream.
//
///////////////////////////////////////////////////////////////////////////////

#include "StreamVisitor.h"
#include "Menu.h"
#include "Button.h"

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

StreamVisitor::StreamVisitor ( std::ostream &s ): BaseClass ( BaseClass::ALL ),
  _stream ( s ),
  _level  ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StreamVisitor::~StreamVisitor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the visitor to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void StreamVisitor::apply ( Menu &m )
{
  for ( unsigned int i = 0; i < _level; ++i )
    _stream << " ";

  char display;
  if ( m.expanded() )
    display = '-';
  else
    display = '+';

  if ( m.items().size() < 1 )
    display = '=';

  _stream << display << m.text() << std::endl;

  ++_level;

  if ( m.expanded() )
    this->traverse ( m );

  --_level;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the visitor to the button.
//
///////////////////////////////////////////////////////////////////////////////

void StreamVisitor::apply ( Button &b )
{
  for ( unsigned int i = 0; i < _level; ++i )
    _stream << " ";

  if( b.separator() )
    _stream << "----" << std::endl;
  else
    _stream << "<" << b.text() << ">" << std::endl;
}
