
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML writer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/Writer.h"
#include "XmlTree/Document.h"

#include "Usul/Errors/Assert.h"

#include <fstream>
#include <stdexcept>

using namespace XmlTree;


///////////////////////////////////////////////////////////////////////////////
//
//  Small utility class to control indentions.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  const unsigned int INDENT_AMOUNT ( 2 );
  struct Indent
  {
    Indent ( std::string &s ) : _s ( s )
    {
      _s.resize ( _s.size() + INDENT_AMOUNT, ' ' );
    }
    ~Indent()
    {
      if ( _s.size() >= INDENT_AMOUNT )
        _s.resize ( _s.size() - INDENT_AMOUNT );
    }
  private:
    std::string &_s;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Writer::Writer() : _indent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Writer::~Writer()
{
  USUL_ASSERT ( _indent.empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to traverse the tree.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void traverse ( const Node *doc, std::string &indent, std::ostream &out )
  {
    if ( doc )
    {
      // Write leading tag.
      out << '\n' << indent << '<' << doc->name();

      // Write attributes.
      for ( Node::Attributes::const_iterator a = doc->attributes().begin(); a != doc->attributes().end(); ++a )
      {
        if ( false == a->first.empty() && false == a->second.empty() )
          out << ' ' << a->first << "=\"" << a->second << "\"";
      }

      // Finish leading tag.
      out << '>';

      // Write value if there is one.
      const std::string &value ( doc->value() );
      if ( false == value.empty() )
        out << value;

      // Need local scope.
      {
        // Indent.
        Helper::Indent scoped ( indent );

        // Write children.
        for ( Node::Children::const_iterator c = doc->children().begin(); c != doc->children().end(); ++c )
          Helper::traverse ( c->get(), indent, out );
      }

      // Go to new line if there are children.
      if ( false == doc->children().empty() )
        out << '\n' << indent;

      // Write trailing tag.
      out << "</" << doc->name() << '>';
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load contents of file and build the document.
//
///////////////////////////////////////////////////////////////////////////////

void Writer::write ( const std::string &file, const Document *document )
{
  // Handle bad input.
  if ( 0x0 == document )
    return;

  // Open file.
  std::ofstream out ( file.c_str() );
  if ( false == out.is_open() )
    throw std::runtime_error ( "Error 2178129023: failed to open file for writing: " + file );

  // Write header.
  out << document->header();

  // Traverse the tree.
  Helper::traverse ( document, _indent, out );

  // Finish with empty line.
  out << '\n';
}
