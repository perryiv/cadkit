
#include "QuickTimeHelper.h"

#include <stdexcept>

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

QuickTimeHelper::QuickTimeHelper() :
_movie ( 0x0 ),
_resRef ( 0 ),
_track ( 0x0 ),
_media ( 0x0 )
{
}


QuickTimeHelper::~QuickTimeHelper()
{
  if  ( _resRef )
    ::CloseMovieFile( _resRef );

  ::ExitMovies();
  ::TerminateQTML();
}

void QuickTimeHelper::initialize()
{
  OSErr err ( ::InitializeQTML( /*kInitQTMLUseDefault*/ 0L ) );

  this->checkError ( err );

  // These were in an example, but they are unresolved when trying to link
  //::InitGraf ( &qd.thePort );
  //::InitFonts ();
  //::InitWindows ();
  //::InitMenus ();
  //::TEInit();
  //::InitDialogs ( nil );
  err = ::EnterMovies();

  this->checkError ( err );
}


void QuickTimeHelper::checkError ( OSErr error )
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


void QuickTimeHelper::createMovie ( const std::string& filename )
{
  Str255		fileName;
  FSSpec		fileSpec;

  c2pstrcpy( fileName, filename.c_str() );
  FSMakeFSSpec(0,0, fileName, &fileSpec );

  OSErr err ( ::CreateMovieFile(&fileSpec,FOUR_CHAR_CODE('TVOD'),smCurrentScript,createMovieFileDeleteCurFile | createMovieFileDontCreateResFile, &_resRef, &_movie) );

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

  this->checkError ( err );
}


void QuickTimeHelper::createTrackAndMedia ( unsigned int width, unsigned int height )
{
  _track = NewMovieTrack ( 
    _movie, 
    FixRatio( width,1),
    FixRatio( height,1), 
		0);

  this->checkError  ( ::GetMoviesError() );

  if ( 0x0 == _track )
  {
    throw std::runtime_error ( "Could not create Quick Time track." );
  }

  _media = NewTrackMedia ( _track, VideoMediaType,
							  kVideoTimeScale, /* Video Time Scale */
							  nil, 0);

  this->checkError ( ::GetMoviesError() );

  if ( 0x0 == _media )
  {
    throw std::runtime_error ( "Could not create Quick Time media." );
  }

}


void QuickTimeHelper::insertTrackIntoMedia ()
{
  OSErr err ( ::InsertMediaIntoTrack ( _track, 0, 0, GetMediaDuration ( _media ), 0x00010000 ) );
  this->checkError ( err );
}


void QuickTimeHelper::initImageAddition()
{
}


void QuickTimeHelper::addImage ( unsigned char *data, unsigned int size )
{
  OSErr err ( ::AddMediaSample2 ( _media, data, size, 60, 0, nil, 1, 0, nil ) );
  this->checkError ( err );
}


QuickTimeHelper::ScopedMediaEdits::ScopedMediaEdits ( QuickTimeHelper& qt ) :
_qt ( qt )
{
}

QuickTimeHelper::ScopedMediaEdits::~ScopedMediaEdits ()
{
  OSErr err ( ::EndMediaEdits ( _qt._media ) );
  _qt.checkError ( err );
}

void QuickTimeHelper::ScopedMediaEdits::init()
{
  OSErr err ( ::BeginMediaEdits ( _qt._media ) );
  _qt.checkError ( err );
}
