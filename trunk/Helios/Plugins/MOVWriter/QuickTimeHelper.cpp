
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "QuickTimeHelper.h"

#include <stdexcept>

#define		kVideoTimeScale 	600
#define		kNumVideoFrames 	70
#define		kPixelDepth 		32	/* use 32-bit depth */
#define		kNoOffset 			0
#define		kMgrChoose			0
#define		kSyncSample 		0
#define		kAddOneVideoSample	1
#define		kVideoFrameDuration 	60	/* frame duration = 1/10 sec */
#define		kTrackStart			0
#define		kMediaStart			0

#ifndef		VIDEO_CODEC_TYPE
#define		VIDEO_CODEC_TYPE	kJPEGCodecType
#endif
#ifndef		VIDEO_CODEC_QUALITY
#define		VIDEO_CODEC_QUALITY	codecNormalQuality
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

QuickTimeHelper::QuickTimeHelper() :
	_width ( 0 ),
	_height( 0 ),
	_movie ( 0x0 ),
	_resRef ( 0 ),
	_file(),
	_track ( 0x0 ),
	_media ( 0x0 ),
	_gWorld ( 0x0 ),
	_pixMap ( 0x0 ),
	_savedPort ( 0x0 ),
	_savedDevice ( 0x0 ),
	_trackFrame(),
	_imageDescription( 0x0 ),
	_compressedDataHandle ( 0x0 ),
	_compressedDataPtr ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

QuickTimeHelper::~QuickTimeHelper()
{
  if  ( _resRef )
    ::CloseMovieFile( _resRef );

  ::ExitMovies();
  ::TerminateQTML();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize QuickTime.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::initialize()
{
  OSErr err ( ::InitializeQTML( /*kInitQTMLUseDefault*/ 0L ) );

  this->_checkError ( err );

  err = ::EnterMovies();

  this->_checkError ( err );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flatten the movie file.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::flattenMovieFile()
{
  _movie = ::FlattenMovieData( _movie,
                               flattenAddMovieToDataFork | flattenForceMovieResourceBeforeMovieData,
                               &_file,
                               VideoMediaType,
                               smSystemScript,
                               createMovieFileDeleteCurFile | createMovieFileDontCreateResFile );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if there is an error.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::_checkError ( OSErr error )
{
  if( error == noErr )
    return;
  else if ( error == memFullErr )
    throw std::runtime_error ( "Error in writing quick time movie: Could not allocate memory." );
  else if ( error == paramErr )
    throw std::runtime_error ( "Error in writing quick time movie: Invalid parameter." );
  //else if ( error == errMediaDoesNotSupportDisplayOffsets )
  //  throw std::runtime_error ( "Error in writing quick time movie: The media does not support nonzero display offsets." );
  //else if ( error == errDisplayTimeAlreadyInUse )
  //  throw std::runtime_error ( "Error in writing quick time movie: The display time is already in use." );
  //else if ( error == errDisplayTimeTooEarly )
  //  throw std::runtime_error ( "Error in writing quick time movie: The display time is too early." );
  else
    throw std::runtime_error ( "Error in writing quick time movie." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the movie with the given filename.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::createMovie ( const std::string& filename )
{
  Str255		fileName;

  c2pstrcpy( fileName, filename.c_str() );
  FSMakeFSSpec(0,0, fileName, &_file );

  OSErr err ( ::CreateMovieFile( &_file,
                                 VideoMediaType,
                                 smCurrentScript,
                                 createMovieFileDeleteCurFile | createMovieFileDontCreateResFile, 
                                 &_resRef, 
                                 &_movie) );

  /*
  CreateMovieFile has been superceded by:

  OSErr CreateMovieStorage (
  Handle dataRef,
  OSType dataRefType,
  OSType creator,
  ScriptCode scriptTag,
  long createMovieFileFlags,
  DataHandler *outDataHandler,
  Movie *newmovie );
  */

  this->_checkError ( err );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the track and media.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::createTrackAndMedia ( unsigned int width, unsigned int height )
{
  _width = static_cast < short > ( width );
  _height = static_cast < short > ( height );

  _track = NewMovieTrack ( 
    _movie, 
    FixRatio( width,1),
    FixRatio( height,1), 
		kNoVolume );

  this->_checkError  ( ::GetMoviesError() );

  if ( 0x0 == _track )
  {
    throw std::runtime_error ( "Could not create Quick Time track." );
  }

  _media = NewTrackMedia ( _track, VideoMediaType,
							  kVideoTimeScale, /* Video Time Scale */
							  nil, 0);

  this->_checkError ( ::GetMoviesError() );

  if ( 0x0 == _media )
  {
    throw std::runtime_error ( "Could not create Quick Time media." );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the track into media.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::insertTrackIntoMedia ()
{
  OSErr err ( ::InsertMediaIntoTrack ( _track, 0, 0, GetMediaDuration ( _media ), 0x00010000 ) );
  this->_checkError ( err );

  short resID ( movieInDataForkResID );

  err = ::AddMovieResource( _movie, _resRef, &resID, NULL );

  this->_checkError ( err );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the images to the movie.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::addImages ( const Filenames& filenames, Usul::Interfaces::IUnknown* caller )
{
  QuickTimeHelper::ScopedMediaEdits edit ( *this );
  edit.init();
 
  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0.0, 1.0, caller, false );

  for ( unsigned int i = 0; i < filenames.size(); ++i )
  {
    OSErr err;

    EraseRect ( &_trackFrame );

    // Draw frame.
    this->_drawFame ( filenames.at(i).c_str() );

    err = CompressImage ( _pixMap,
                          &_trackFrame,
                          codecNormalQuality,
                          kJPEGCodecType,
                          _imageDescription,
                          _compressedDataPtr );

    this->_checkError( err );

    err = AddMediaSample ( _media,
                           _compressedDataHandle,
                           0,
                           (**_imageDescription).dataSize,
                           kVideoFrameDuration,
                           (SampleDescriptionHandle)_imageDescription,
                           1,
                           0,
                           NULL );

    this->_checkError( err );

    // Update progress.
    progress ( i, filenames.size() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the image into the frame.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::_drawFame ( const std::string& filename )
{
  MacSetRect ( &_trackFrame, 0, 0, _width, _height );

  GraphicsImportComponent myImporter ( 0x0 );

  Str255		fileName;
  FSSpec		fileSpec;

  c2pstrcpy( fileName, filename.c_str() );
  FSMakeFSSpec(0,0, fileName, &fileSpec );

  GetGraphicsImporterForFile ( &fileSpec, &myImporter );

  GraphicsImportSetGWorld( myImporter, _gWorld, 0 );
  GraphicsImportDraw ( myImporter );
  CloseComponent ( myImporter );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper to scope media edit.
//
///////////////////////////////////////////////////////////////////////////////

QuickTimeHelper::ScopedMediaEdits::ScopedMediaEdits ( QuickTimeHelper& qt ) :
_qt ( qt )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor for helper.
//
///////////////////////////////////////////////////////////////////////////////

QuickTimeHelper::ScopedMediaEdits::~ScopedMediaEdits ()
{
  SetGWorld ( _qt._savedPort, _qt._savedDevice );

  // Dispose the image description handle
  if ( _qt._imageDescription ) 
    DisposeHandle ( (Handle) _qt._imageDescription );

  // Dispose the compressed data handle
  if ( _qt._compressedDataHandle ) 
    DisposeHandle ( _qt._compressedDataHandle );
  
  // Dispose the GWorld if we have one.
  if ( _qt._gWorld ) 
    DisposeGWorld ( _qt._gWorld );

  OSErr err ( ::EndMediaEdits ( _qt._media ) );
  _qt._checkError ( err );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Init scope media edits.
//
///////////////////////////////////////////////////////////////////////////////

void QuickTimeHelper::ScopedMediaEdits::init()
{
  OSErr err ( ::BeginMediaEdits ( _qt._media ) );
  _qt._checkError ( err );

  MacSetRect ( &_qt._trackFrame, 0, 0, _qt._width, _qt._height );

  long                            maxCompressedSize;


  err = NewGWorld (&_qt._gWorld, 
                  24,             /* pixel depth */
                  &_qt._trackFrame, 
                  nil, 
                  nil, 
                  (GWorldFlags) 0 );

  _qt._checkError ( err );

  _qt._pixMap = GetGWorldPixMap( _qt._gWorld );

  LockPixels ( _qt._pixMap );

  err = GetMaxCompressionSize ( _qt._pixMap,
                              &_qt._trackFrame, 
                              0, /* let ICM choose depth */
                              codecNormalQuality, 
                              kJPEGCodecType, 
                              (CompressorComponent) anyCodec,
                              &maxCompressedSize);

  _qt._checkError ( err );

  _qt._compressedDataHandle = NewHandle(maxCompressedSize);

  //MoveHHi( compressedData );
  HLock( _qt._compressedDataHandle );

  _qt._compressedDataPtr = *_qt._compressedDataHandle;

  _qt._imageDescription = (ImageDescriptionHandle)NewHandle(4);

  GetGWorld ( &_qt._savedPort, &_qt._savedDevice );
  SetGWorld ( _qt._gWorld, nil );
}
