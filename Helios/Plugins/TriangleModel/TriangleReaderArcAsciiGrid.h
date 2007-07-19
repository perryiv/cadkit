
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizone State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Arc ASCII Grid reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_ARC_ASCII_GRID_READER_CLASS_H_
#define _TRIANGLE_MODEL_ARC_ASCII_GRID_READER_CLASS_H_

#include "TriangleDocument.h"

#include "Usul/Types/Types.h"

#include <string>


class TriangleReaderArcAsciiGrid
{
public:

  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::pair < std::string, Usul::Types::Uint64 > FileInfo;
  typedef std::pair < unsigned int, unsigned int > Progress;

  // Construction/destruction.
  TriangleReaderArcAsciiGrid ( const std::string &file, IUnknown *caller, TriangleDocument *doc );
  ~TriangleReaderArcAsciiGrid();

  // Clear accumulated state.
  void                clear();

  // Read the file.
  void                operator()();

protected:

  void                _incrementProgress ( bool state );

  void                _read();

private:

  // No copying.
  TriangleReaderArcAsciiGrid ( const TriangleReaderArcAsciiGrid & );
  TriangleReaderArcAsciiGrid &operator = ( const TriangleReaderArcAsciiGrid & );

  FileInfo _file;
  IUnknown::RefPtr _caller;
  TriangleDocumentPtr _document;
  Progress _progress;
};


#endif // _TRIANGLE_MODEL_ARC_ASCII_GRID_READER_CLASS_H_
