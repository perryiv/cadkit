
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

#ifndef __MOVWRITERCOMPONENT_H__
#define __MOVWRITERCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IWriteMovieFile.h"


class MOVWriterComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IWriteMovieFile,
                           public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MOVWriterComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  MOVWriterComponent();

protected:    
  /// Destructor
  ~MOVWriterComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  virtual Filters             filtersWrite() const;
  virtual bool                canWrite   ( const std::string &file ) const;
  virtual void                writeMovie ( const Filename& filename, const Filenames& filenames, Usul::Interfaces::IUnknown* caller = 0x0 );
};

#endif /* __MOVWRITERCOMPONENT_H__  */		
	
