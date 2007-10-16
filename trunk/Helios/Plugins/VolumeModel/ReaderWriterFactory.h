
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_VOLUME_MODEL_READER_WRITER_FACTORY_H__
#define __HELIOS_VOLUME_MODEL_READER_WRITER_FACTORY_H__

#include "Helios/Plugins/VolumeModel/IReaderWriter.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <vector>

class ReaderWriterFactory
{
public:
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;
  typedef IReaderWriter::RefPtr                 ReaderWriterPtr;
  typedef std::vector < ReaderWriterPtr >       ReaderWriters;
  typedef Usul::Threads::RecursiveMutex         Mutex;
  typedef Usul::Threads::Guard<Mutex>           Guard;

  static ReaderWriterFactory&  instance();

  /// Get all the filters that the registered reader/writers can open.
  //Filters               filtersOpen()   const;

  /// Is there a reader/writer that can open given file.
  //bool                  canOpen   ( const std::string &file ) const;

  /// Register a reader/writer.
  void                  add ( IReaderWriter* );

  /// Create a reader/writer.
  IReaderWriter*        create ( const std::string& file );

  Mutex &               mutex() const;
private:

  ReaderWriterFactory ();
  ~ReaderWriterFactory ();

  static ReaderWriterFactory *_instance;

  mutable Mutex *_mutex;
  ReaderWriters _readerWriters;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Class for automatically registering reader writer.
//
///////////////////////////////////////////////////////////////////////////////

struct RegisterReaderWriter
{
  RegisterReaderWriter ( IReaderWriter *rw )
  {
    ReaderWriterFactory::instance().add ( rw );
  }
};


#endif //__HELIOS_VOLUME_MODEL_READER_WRITER_FACTORY_H__
