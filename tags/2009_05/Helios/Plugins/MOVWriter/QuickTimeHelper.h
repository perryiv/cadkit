
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QUICK_TIME_HELPER_H__
#define __QUICK_TIME_HELPER_H__

#include "Usul/Interfaces/GUI/IProgressBar.h"

// Quick Time headers.
#include "QTML.h"
#include "Movies.h"

#include <string>
#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }

class QuickTimeHelper
{
public:

  typedef std::string Filename;
  typedef std::vector< Filename > Filenames;

  QuickTimeHelper();
  ~QuickTimeHelper();

  void initialize();

  void createMovie ( const std::string& filename );

  void createTrackAndMedia ( unsigned int width, unsigned int height );

  void insertTrackIntoMedia ();

  void addImages ( const Filenames& filenames, Usul::Interfaces::IUnknown* caller );

  void flattenMovieFile();

private:

  struct ScopedMediaEdits
  {
    ScopedMediaEdits ( QuickTimeHelper& qt );
    ~ScopedMediaEdits ();

    void init();
  private:
    ScopedMediaEdits(); // Do not use.

    QuickTimeHelper &_qt;
  };

  void _checkError ( OSErr error );
  void _drawFame ( const std::string& filename );

  short _width;
  short _height;

  Movie _movie;
  short _resRef;
  FSSpec _file;

  Track                   _track;
  Media                   _media;
  GWorldPtr               _gWorld;
  PixMapHandle            _pixMap;
  CGrafPtr                _savedPort;
  GDHandle                _savedDevice;
  Rect                    _trackFrame;
  ImageDescriptionHandle  _imageDescription;
  Handle                  _compressedDataHandle;
  Ptr                     _compressedDataPtr;
};

#endif
