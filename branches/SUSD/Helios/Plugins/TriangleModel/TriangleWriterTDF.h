
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TDF writer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_TDF_WRITER_CLASS_H_
#define _TRIANGLE_MODEL_TDF_WRITER_CLASS_H_

#include "TriangleDocument.h"

#include <string>


class TriangleWriterTDF
{
public:

  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Construction/destruction.
  TriangleWriterTDF ( const std::string &file, Unknown *caller, const TriangleDocument *doc );
  ~TriangleWriterTDF();

  // Write the file.
  void operator()();

private:

  // No copying.
  TriangleWriterTDF ( const TriangleWriterTDF & );
  TriangleWriterTDF &operator = ( const TriangleWriterTDF & );

  std::string _file;
  Unknown::RefPtr _caller;
  TriangleDocumentPtr _document;
};


#endif // _TRIANGLE_MODEL_TDF_WRITER_CLASS_H_
