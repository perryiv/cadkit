
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For processing the command line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMMAND_LINE_PARSER_CLASS_H_
#define _USUL_COMMAND_LINE_PARSER_CLASS_H_

#include "Usul/Algorithms/Extract.h"

#include "Usul/Predicates/EndsWith.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Predicates/UnaryPair.h"

#include <list>
#include <string>


namespace Usul {
namespace CommandLine {


class Parser
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedef(s).
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef std::list<std::string> Args;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Parser() : _args()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Construct from arbitrary iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Iter_ > Parser ( Iter_ b, Iter_ e ) : _args ( b, e )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Parser ( const Parser &p ) : _args ( p._args )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Parser &operator = ( const Parser &p )
  {
    _args = p._args;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get/set the arguments.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Args &args() const 
  {
    return _args;
  }
  Args &args()
  {
    return _args;
  }
  void args ( const Args &args )
  {
    _args = args;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Extract arguments that are files with the given extension.
  //
  /////////////////////////////////////////////////////////////////////////////

  Args files ( const std::string &ext, bool remove )
  {
    // The files.
    Args f;

    // Extract the files with the given extension.
    Usul::Algorithms::extract ( Usul::Predicates::unaryPair ( Usul::Predicates::EndsWith ( ext ), // if it has correct extension
                                                              std::logical_and<bool>(),           // ... and ...
                                                              Usul::Predicates::FileExists() ),   // there's anything there at all
                                _args,              // look here for files to extract
                                f,                  // put extracted files here
                                remove );           // remove extracted files from _args?

    // Return the files.
    return f;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Extract arguments that are files.
  //
  /////////////////////////////////////////////////////////////////////////////

  Args files ( bool remove )
  {
    // The files.
    Args f;

    // Extract the files with the given extension.
    Usul::Algorithms::extract ( Usul::Predicates::FileExists(),
                                _args,
                                f,
                                remove );

    // Return the files.
    return f;
  }


private:

  Args _args;
};


}; // namespace CommandLine
}; // namespace Usul


#endif // _USUL_COMMAND_LINE_PARSER_CLASS_H_
