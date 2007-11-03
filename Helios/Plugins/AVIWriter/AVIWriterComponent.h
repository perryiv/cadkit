
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component to write .avi files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AVIWRITERCOMPONENT_H__
#define __AVIWRITERCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IWriteMovieFile.h"

class AVIWriterComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IPlugin,
                           public Usul::Interfaces::IWriteMovieFile
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AVIWriterComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  AVIWriterComponent();

protected:    
  /// Destructor
  ~AVIWriterComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string         getPluginName() const;
    
  /// Usul::Interfaces::IWriteMovieFile
  virtual Filters             filtersWrite() const;
  virtual bool                canWrite   ( const std::string &file ) const;
  virtual void                writeMovie ( const Filename& filename, const Filenames& filenames, Usul::Interfaces::IUnknown *caller = 0x0 );
};

#endif /* __AVIWRITERCOMPONENT_H__  */		
	
