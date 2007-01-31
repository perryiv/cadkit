
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

#ifndef _MENUKIT_STREAM_VISITOR_H_
#define _MENUKIT_STREAM_VISITOR_H_

#include "MenuKit/Visitor.h"

#include <ostream>


namespace MenuKit {

class Menu;
class Button;

class MENUKIT_EXPORT StreamVisitor : public Visitor
{
public:

  // Typedefs and smart-pointers.
  typedef Visitor BaseClass;
  MENUKIT_DECLARE_POINTER ( StreamVisitor );

  // Construction.
  StreamVisitor ( std::ostream &s );

  // Apply the visitor to the items.
  virtual void      apply ( Menu & );
  virtual void      apply ( Button & );

protected:

  // Use reference counting.
  virtual ~StreamVisitor();

private:

  // No copying.
  StreamVisitor ( const StreamVisitor & );
  StreamVisitor &operator = ( const StreamVisitor & );

  std::ostream &_stream;
  unsigned int _level;
};


}; // namespace MenuKit


#endif // _MENUKIT_STREAM_VISITOR_H_
