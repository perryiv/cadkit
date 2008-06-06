
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#include "AVIWriter.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "osg/ref_ptr"
#include "osg/Image"
#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include <windows.h>
#include <vfw.h>
#include <iostream>
#include <algorithm>

struct AVIWriterHelper
{
  AVIWriterHelper() :
      stream ( 0x0 ),
      streamCompressed ( 0x0 ),
      aviFile ( 0x0 )
  {
    AVIFileInit();
  }

  ~AVIWriterHelper()
  {
    if( stream )
      AVIStreamClose( stream );

    if( streamCompressed )
      AVIStreamClose( streamCompressed );

    if( aviFile )
      AVIFileRelease ( aviFile );

    AVIFileExit();
  }


  PAVISTREAM stream;
  PAVISTREAM streamCompressed;
  PAVIFILE   aviFile;
  
};

AVIWriter::AVIWriter( const Filename& filename, const Filenames& filenames ) :
	_filename ( filename ),
	_filenames ( filenames )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Write the AVI file.
//
// See http://www.adp-gmbh.ch/win/programming/avi/avi.html
// http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c/directx/htm/avirifffilereference.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/multimed/htm/_win32_avifile_functions_and_macros.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/multimed/htm/_win32_multimedia_functions.asp
//
///////////////////////////////////////////////////////////////////////////////

void AVIWriter::operator () ( Usul::Interfaces::IUnknown *caller )
{
  // Return now if we don't have any filenames.
  if( _filenames.empty() )
    return;

  Usul::Functions::safeCall ( Usul::Adaptors::bind1<void> ( caller, Usul::Adaptors::memberFunction ( this, &AVIWriter::_write ) ), "2952324509" );

  // Remove all files.
  std::for_each ( _filenames.begin(), _filenames.end(), Usul::File::Remove() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the avi file.
//
///////////////////////////////////////////////////////////////////////////////

void AVIWriter::_write ( Usul::Interfaces::IUnknown *caller )
{
  typedef osg::ref_ptr < osg::Image > ImagePtr;

  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0.0, 1.0, caller );

  unsigned int rate ( 15 );
  unsigned int time ( 0 );

  AVIWriterHelper helper;

  // Open the avi file.
  LONG result ( ::AVIFileOpen( &helper.aviFile, _filename.c_str(), OF_WRITE | OF_CREATE, 0L) );

  if( result != 0 )
  {
		Usul::Exceptions::Thrower< std::runtime_error > ( "Error 2783715167: Unable to create movie: ", _filename );
  }

  // Open the first image.
  ImagePtr image ( osgDB::readImageFile ( _filenames.at( 0 ) ) );

  const unsigned int pixelSize ( image->getPixelSizeInBits() );
  const unsigned int numComponents ( osg::Image::computeNumComponents ( image->getPixelFormat() ) );

  if ( 3 != numComponents )
		Usul::Exceptions::Thrower< std::runtime_error > ( "Error 2536623597: Invalid number of components for avi writing." );

	if ( 24 != pixelSize )
		Usul::Exceptions::Thrower< std::runtime_error > ( "Error 3968712159: Invalid number of bits per pixel for avi writing." );

  AVISTREAMINFO strhdr;

  // Init header to all zeros
  ::memset(&strhdr, 0, sizeof(strhdr));

  // Fill in the header.
  strhdr.fccType                = streamtypeVIDEO;// stream type
  //strhdr.fccHandler             = 0; // No compression.
  //strhdr.fccHandler             = mmioFOURCC('D','I','B',' '); // Uncompressed
	//strhdr.fccHandler             = mmioFOURCC('C','V','I','D'); // Cinpak
	//strhdr.fccHandler             = mmioFOURCC('I','V','3','2'); // Intel video 3.2
	//strhdr.fccHandler             = mmioFOURCC('M','S','V','C'); // Microsoft video 1
	//strhdr.fccHandler             = mmioFOURCC('I','V','5','0'); // Intel video 5.0
  strhdr.dwScale                = 1;
  strhdr.dwRate                 = rate;
  strhdr.dwQuality              = 10000;
  strhdr.dwSuggestedBufferSize  = 0; //image->getImageSizeInBytes();

  SetRect(&strhdr.rcFrame, 0, 0, image->s(), image->t() );

  ::AVIFileCreateStream( helper.aviFile,  // file pointer
                        &helper.stream,  // returned stream pointer
                        &strhdr);      // stream header


  AVICOMPRESSOPTIONS opts;
  ::memset(&opts, 0, sizeof(opts));  

  opts.fccType = 0;
  //opts.fccHandler=mmioFOURCC('m','s','v','c');
  //opts.fccHandler             = mmioFOURCC('D','I','B',' '); // Uncompressed
	//opts.fccHandler             = mmioFOURCC('C','V','I','D'); // Cinpak
  opts.dwQuality = 10000;
  opts.dwBytesPerSecond = 0;
  opts.dwFlags = 8;

  result = ::AVIMakeCompressedStream(&helper.streamCompressed, 
                                     helper.stream, 
                                     &opts, NULL);

  if ( result != 0 )
  {
    Usul::Exceptions::Thrower< std::runtime_error > ( "Unable to compress movie: ", _filename );
  }

  LPBITMAPINFOHEADER  lpbi;
  HANDLE              hDIB;

  DWORD               dwLen;      // size of memory block
  
	// Size of a fow in the image.
	const unsigned int dataWidth ( image->getRowSizeInBytes() );
  
  dwLen = sizeof(BITMAPINFOHEADER) + dataWidth * image->t();
  hDIB = ::GlobalAlloc(GHND, dwLen);
  lpbi = (LPBITMAPINFOHEADER) ::GlobalLock( hDIB );
  
  lpbi->biSize = sizeof(BITMAPINFOHEADER);
  lpbi->biWidth = image->s();
  lpbi->biHeight = image->t();
  lpbi->biPlanes = 1;
  lpbi->biBitCount = 24;
  lpbi->biCompression = BI_RGB;
  lpbi->biClrUsed = 0;
  lpbi->biClrImportant = 0;
  lpbi->biSizeImage = image->getImageSizeInBytes();

  result = ::AVIStreamSetFormat( helper.streamCompressed, 0, lpbi, lpbi->biSize);

  if( result != 0 )
  {
    Usul::Exceptions::Thrower< std::runtime_error > ( "Unable to set format for file: ", _filename );
  }

  // Loop through the images
  for ( unsigned int i = 0; i < _filenames.size(); ++i )
  {
    ImagePtr image ( osgDB::readImageFile ( _filenames.at( i ) ) );

		// Not sure what this is supposed to be for.
    //unsigned char *dest = reinterpret_cast < unsigned char * > ( lpbi + lpbi->biSize );

    std::vector< unsigned char > buffer;
    buffer.reserve( image->getImageSizeInBytes() );

    // Copy the data.
    for ( int t = 0; t < image->t(); ++t )
    {
      for ( int s = 0 ; s < image->s(); ++s )
      {
        unsigned char* source ( image->data( s, t, 0 ) );
        buffer.push_back ( *(source+2) );
        buffer.push_back ( *(source+1) );
        buffer.push_back ( *(source+0) );
      }
    }

    // write out the image
    ::AVIStreamWrite( helper.streamCompressed,  // stream pointer
                      time, // time of this frame
                      1,        // number to write
                      &buffer.front(), // pointer to data
                      image->getImageSizeInBytes(), // size of this frame
                      AVIIF_KEYFRAME,       // flags....
                      NULL, NULL);

    ++time;

    progress ( i, _filenames.size() );
  }

  ::GlobalUnlock( hDIB );
}

#endif
