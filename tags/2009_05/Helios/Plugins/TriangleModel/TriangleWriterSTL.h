
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  STL writer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_STL_WRITER_CLASS_H_
#define _TRIANGLE_MODEL_STL_WRITER_CLASS_H_

#include "TriangleDocument.h"

#include <string>


class TriangleWriterSTL
{
public:

  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Construction/destruction.
  TriangleWriterSTL ( const std::string &file, bool binary, Unknown *caller, const TriangleDocument *doc );
  ~TriangleWriterSTL();

  // Write the file.
  void operator()();

private:

  // No copying.
  TriangleWriterSTL ( const TriangleWriterSTL & );
  TriangleWriterSTL &operator = ( const TriangleWriterSTL & );

  std::string _file;
  Unknown::RefPtr _caller;
  TriangleDocumentPtr _document;
  bool _binary;
};


#endif // _TRIANGLE_MODEL_STL_WRITER_CLASS_H_
