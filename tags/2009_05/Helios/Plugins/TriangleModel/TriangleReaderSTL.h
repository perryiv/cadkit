
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  STL reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_STL_READER_CLASS_H_
#define _TRIANGLE_MODEL_STL_READER_CLASS_H_

#include "TriangleDocument.h"

#include <string>
#include <iosfwd>


class TriangleReaderSTL
{
public:

  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Construction/destruction.
  TriangleReaderSTL ( const std::string &file, Unknown *caller, TriangleDocument *doc );
  ~TriangleReaderSTL();

  // Read the file.
  void                  operator()();

protected:

  bool                  _isAscii() const;

  unsigned int          _countAcsiiFacets();

  void                  _readAscii();
  void                  _readBinary();

private:

  // No copying.
  TriangleReaderSTL ( const TriangleReaderSTL & );
  TriangleReaderSTL &operator = ( const TriangleReaderSTL & );

  std::string _file;
  Unknown::RefPtr _caller;
  TriangleDocumentPtr _document;
};


#endif // _TRIANGLE_MODEL_STL_READER_CLASS_H_
