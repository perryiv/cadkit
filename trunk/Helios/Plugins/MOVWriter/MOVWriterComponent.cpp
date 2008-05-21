
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////


#include "Helios/Plugins/MOVWriter/MOVWriterComponent.h"
#include "Helios/Plugins/MOVWriter/MOVWriter.h"

#include "Usul/Strings/Case.h"
#include "Usul/File/Path.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MOVWriterComponent, MOVWriterComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

MOVWriterComponent::MOVWriterComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

MOVWriterComponent::~MOVWriterComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MOVWriterComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IWriteMovieFile::IID:
    return static_cast < Usul::Interfaces::IWriteMovieFile*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string MOVWriterComponent::getPluginName() const 
{
  return "MOV Writer";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Filters that the component can write.
//
///////////////////////////////////////////////////////////////////////////////

MOVWriterComponent::Filters MOVWriterComponent::filtersWrite() const
{
  Filters filters;
  filters.push_back( Filter ( "QuickTime (*.mov)", "*.mov" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can this extension be written?
//
///////////////////////////////////////////////////////////////////////////////

bool MOVWriterComponent::canWrite   ( const std::string &filename ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  return ( "mov" == ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job to write the movie file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class WriteMovJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;
    typedef MOVWriterComponent::Filenames Filenames;

    WriteMovJob ( const std::string& filename, const Filenames& filenames, Usul::Interfaces::IUnknown* caller ) :
			BaseClass ( caller ),
			_filename ( filename ),
			_filenames ( filenames )
    {
    }

    virtual ~WriteMovJob() { }

  protected:
    virtual void _started ()
    {
      MOVWriter writer ( _filename, _filenames );
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

void MOVWriterComponent::writeMovie ( const Filename& filename, const Filenames& filenames, Usul::Interfaces::IUnknown *caller )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if ( "mov" == ext )
  {
    Usul::Jobs::Manager::instance().addJob ( new Detail::WriteMovJob ( filename, filenames, caller ) );
  }
}
