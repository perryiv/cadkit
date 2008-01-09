
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OFF reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_OFF_READER_CLASS_H_
#define _TRIANGLE_MODEL_OFF_READER_CLASS_H_

#include "TriangleDocument.h"

#include <string>
#include <iosfwd>


class TriangleReaderOFF
{
public:

  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Construction/destruction.
  TriangleReaderOFF ( const std::string &file, Unknown *caller, TriangleDocument *doc );
  ~TriangleReaderOFF();

  // Read the file.
  void                  operator()();

private:

  // No copying.
  TriangleReaderOFF ( const TriangleReaderOFF & );
  TriangleReaderOFF &operator = ( const TriangleReaderOFF & );

  std::string _file;
  Unknown::RefPtr _caller;
  TriangleDocumentPtr _document;
};


#endif // _TRIANGLE_MODEL_OFF_READER_CLASS_H_
