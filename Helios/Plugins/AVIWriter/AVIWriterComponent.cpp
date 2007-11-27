
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "AVIWriterComponent.h"

#include "Usul/Strings/Case.h"
#include "Usul/File/Path.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

#include "AVIWriter.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AVIWriterComponent, AVIWriterComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AVIWriterComponent::AVIWriterComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AVIWriterComponent::~AVIWriterComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AVIWriterComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IWriteMovieFile::IID:
    return static_cast < Usul::Interfaces::IWriteMovieFile* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string AVIWriterComponent::getPluginName() const 
{
  return "AVI Writer";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Filters that the component can write.
//
///////////////////////////////////////////////////////////////////////////////

AVIWriterComponent::Filters AVIWriterComponent::filtersWrite() const
{
  Filters filters;
  filters.push_back( Filter ( "Audio-Video Interleaved (*.avi)", "*.avi" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can this extension be written?
//
///////////////////////////////////////////////////////////////////////////////

bool AVIWriterComponent::canWrite   ( const std::string &filename ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  return ( "avi" == ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job to write avi.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class WriteAVIJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;
    typedef AVIWriterComponent::Filenames Filenames;

    WriteAVIJob ( const std::string& filename, const Filenames& filenames, Usul::Interfaces::IUnknown* caller ) :
      BaseClass ( caller ),
      _filename ( filename ),
      _filenames ( filenames )
    {
    }

    virtual ~WriteAVIJob()
    {
    }

  protected:
    virtual void _started ()
    {
      AVIWriter writer ( _filename, _filenames );
      writer( this->progress() );
    }
  private:
    std::string _filename;
    Filenames _filenames;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the movie file.
//
///////////////////////////////////////////////////////////////////////////////

void AVIWriterComponent::writeMovie ( const Filename& filename, const Filenames& filenames, Usul::Interfaces::IUnknown *caller )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if ( "avi" == ext )
  {
#if 1
    Usul::Jobs::Manager::instance().addJob ( new Detail::WriteAVIJob ( filename, filenames, caller ) );
#else
    AVIWriter writer ( filename, filenames );
    writer();
#endif
  }
}
