
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Displays text.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_
#define _APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_

#include "AppFrameWork/Core/Window.h"

#include <sstream>
#include <list>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT TextWindow : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;
  typedef std::list < std::string > Lines;
  typedef Lines::const_iterator ConstIterator;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TextWindow );

  // Enumeration to make output similar to std::cout, etc.
  enum Flusher { ENDL, FLUSH };

  // Constructor
  TextWindow();

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Append the text.
  void                                append ( const std::string & );
  void                                append ( const char * );
  void                                append ( Flusher );
  template < class T > void           append ( T value );

  // Iterators to the lines.
  ConstIterator                       begin() const;
  ConstIterator                       end() const;

  // Set/get the maximum number of lines.
  void                                maxLines ( unsigned int );
  unsigned int                        maxLines() const;

protected:

  // Use reference counting.
  virtual ~TextWindow();

  void                                _oneLineMin();
  void                                _keepUnderMax();

private:

  // No copying.
  TextWindow ( const TextWindow & );
  TextWindow &operator = ( const TextWindow & );

  Lines _lines;
  unsigned int _maxLines;
};


// Append generic type.
template < class T > inline void TextWindow::append ( T value )
{
  std::ostringstream s;
  s << value;
  this->append ( s.str() );
  return *this;
}


// Output operator to add text.
template < class T > inline TextWindow &operator << ( TextWindow &text, T value )
{
  text.append ( value );
  return text;
}


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_
