
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MOVWriter.h"
#include "QuickTimeHelper.h"

#include "Usul/File/Remove.h"

#include "osg/ref_ptr"
#include "osg/Image"
#include "osgDB/ReadFile"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MOVWriter::MOVWriter( const Filename& filename, const Filenames& filenames ) :
	_filename ( filename ),
	_filenames ( filenames )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the quick time movie.
//  See: http://developer.apple.com/referencelibrary/GettingStarted/GS_QuickTime/index.html
//  See: http://developer.apple.com/documentation/QuickTime/INMAC/QT/iqMovieToolbox.16.htm
//
//  It would be cool to export quick time vrs also:
//  http://developer.apple.com/documentation/QuickTime/InsideQT_QTVR/index.html
//
///////////////////////////////////////////////////////////////////////////////

void MOVWriter::operator () ( Usul::Interfaces::IUnknown *caller )
{
  if( _filenames.empty() )
    return;

  QuickTimeHelper helper;
  helper.initialize();

  // Create the movie.
  helper.createMovie ( _filename );

  typedef osg::ref_ptr < osg::Image > ImagePtr;

  // Open the first image.
  ImagePtr image ( osgDB::readImageFile ( _filenames.at( 0 ) ) );

  // Create the track.
  helper.createTrackAndMedia ( image->s(), image->t() );

  // Add the images
  helper.addImages ( _filenames, caller );

  helper.insertTrackIntoMedia();

  helper.flattenMovieFile();

  // Delete all images.
  std::for_each ( _filenames.begin(), _filenames.end(), Usul::File::Remove() );
}
