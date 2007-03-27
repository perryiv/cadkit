
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

#ifndef _TRIANGLE_MODEL_R3D_READER_CLASS_H_
#define _TRIANGLE_MODEL_R3D_READER_CLASS_H_

#include "TriangleDocument.h"

#include <string>


class TriangleReaderR3D
{
public:

  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Construction/destruction.
  TriangleReaderR3D ( const std::string &file, Unknown *caller, TriangleDocument *doc );
  ~TriangleReaderR3D();

  // Read the file.
  void                  operator()();

protected:

  void                    _check ( std::istream &in, const std::string &s ) const;
  void                    _skipLine ( std::istream &in ) const;
  void                    _skipWord ( std::istream &in ) const;

private:

  // No copying.
  TriangleReaderR3D ( const TriangleReaderR3D & );
  TriangleReaderR3D &operator = ( const TriangleReaderR3D & );

  std::string _file;
  Unknown::RefPtr _caller;
  TriangleDocumentPtr _document;
};


#endif // _TRIANGLE_MODEL_R3D_READER_CLASS_H_
