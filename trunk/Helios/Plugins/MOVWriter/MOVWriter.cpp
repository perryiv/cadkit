
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MOVWriter.h"
#include "QuickTimeHelper.h"

// Quick Time headers.
#include "QTML.h"
#include "Movies.h"


#include "osg/ref_ptr"
#include "osg/Image"
#include "osgDB/ReadFile"


#define		kVideoTimeScale 	600
#define		kNumVideoFrames 	70
#define		kPixelDepth 		32	/* use 32-bit depth */
#define		kNoOffset 			0
#define		kMgrChoose			0
#define		kSyncSample 		0
#define		kAddOneVideoSample	1
#define		kSampleDuration 	60	/* frame duration = 1/10 sec */
#define		kTrackStart			0
#define		kMediaStart			0

#ifndef		VIDEO_CODEC_TYPE
#define		VIDEO_CODEC_TYPE	kJPEGCodecType
#endif
#ifndef		VIDEO_CODEC_QUALITY
#define		VIDEO_CODEC_QUALITY	codecNormalQuality
#endif


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

void MOVWriter::operator () ()
{
  if( _filenames.empty() )
    return;

  QuickTimeHelper helper;
  helper.initialize();

  OSErr err;

  // Create the movie.
  helper.createMovie ( _filename );

  typedef osg::ref_ptr < osg::Image > ImagePtr;

  // Open the first image.
  ImagePtr image ( osgDB::readImageFile ( _filenames.at( 0 ) ) );

  // Create the track.
  helper.createTrackAndMedia ( image->s(), image->t() );

  Rect trackFrame;

  trackFrame.left = 0;
  trackFrame.top = 0;
	trackFrame.right = image->s();
  trackFrame.bottom= image->t();

  {
    QuickTimeHelper::ScopedMediaEdits edit ( helper );
    edit.init();
    
    long                            maxCompressedSize;
    GWorldPtr                       theGWorld = nil;
    Handle                          compressedData = nil;
    Ptr                             compressedDataPtr;
    ImageDescriptionHandle          imageDesc = nil;
    CGrafPtr                        oldPort;
    GDHandle                        oldGDeviceH;

    err = NewGWorld (&theGWorld, 
                    24,             /* pixel depth */
                    &trackFrame, 
                    nil, 
                    nil, 
                    (GWorldFlags) 0 );

    helper.checkError ( err );

    LockPixels (theGWorld->portPixMap);

    err = GetMaxCompressionSize (theGWorld->portPixMap,
                                &trackFrame, 
                                0, /* let ICM choose depth */
                                codecNormalQuality, 
                                'rle ', 
                                (CompressorComponent) anyCodec,
                                &maxCompressedSize);

    helper.checkError ( err );

    compressedData = NewHandle(maxCompressedSize);

    MoveHHi( compressedData );
    HLock( compressedData );
    compressedDataPtr = StripAddress( *compressedData );

    imageDesc = (ImageDescriptionHandle)NewHandle(4);

    GetGWorld (&oldPort, &oldGDeviceH);
    SetGWorld (theGWorld, nil);

    // Loop through the images
    for ( unsigned int i = 0; i < _filenames.size(); ++i )
    {
      ImagePtr image ( osgDB::readImageFile ( _filenames.at( i ) ) );

      helper.addImage ( image->data( 0, 0, 0 ), image->getImageSizeInBytes() );
    }

    SetGWorld (oldPort, oldGDeviceH);

    if (imageDesc) DisposeHandle ((Handle)imageDesc);
    if (compressedData) DisposeHandle (compressedData);
    if (theGWorld) DisposeGWorld (theGWorld);
  }

  helper.insertTrackIntoMedia();
  
}
