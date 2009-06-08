
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//	Reader class.
//
/////////////////////////////////////////////////////////////////////////////

#include "OsgTools/IO/Reader.h"

#include "Usul/File/Find.h"
#include "Usul/File/Path.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Node"
#include "osgDB/ReadFile"
#include "osgDB/FileNameUtils"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>


namespace OsgTools {
namespace IO {


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the filters.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct InitFilters
  {
    InitFilters()
    {
    }

    Reader::Filters filters;
  } filters;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Reader::Reader() : _node ( 0x0 ), _progress ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Reader::~Reader()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::read ( const std::string &file )
{
  USUL_TRACE_SCOPE;

  osg::ref_ptr<osgDB::ReaderWriter> reader ( this->_findReader ( file ) );
  this->_read ( file, *reader );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if there is an appropriate reader.
//
///////////////////////////////////////////////////////////////////////////////

bool Reader::hasReader ( const std::string &file )
{
  try
  {
    const std::string ext ( osgDB::getLowerCaseFileExtension ( file ) );
    osg::ref_ptr<osgDB::ReaderWriter> reader ( osgDB::Registry::instance()->getReaderWriterForExtension ( ext ) );
    return reader.valid();
  }
  catch ( const std::exception &e )
  {
    std::cout << "Error 3336526142: Failed to determine if there is an appropriate reader for file: " << file << ". ";
    std::cout << ( ( 0x0 != e.what() ) ? e.what() : "" ) << std::endl;
    return false;
  }
  catch ( ... )
  {
    std::cout << "Error 2035008591: Failed to determine if there is an appropriate reader for file: " << file << std::endl;
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the appropriate reader.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::ReaderWriter *Reader::_findReader ( const std::string &file )
{
  // Get file extension.
  const std::string ext ( osgDB::getLowerCaseFileExtension ( file ) );
  osg::ref_ptr<osgDB::ReaderWriter> reader ( osgDB::Registry::instance()->getReaderWriterForExtension ( ext ) );

  if ( false == reader.valid() )
    throw std::runtime_error ( std::string ( "Error 1010869612: Could not find reader for file: " ) + file );

  return reader.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osgDB::ReaderWriter::ReadResult read ( const std::string &file, osgDB::ReaderWriter &rw, Reader::Callback *callback )
  {
    // Open the stream.
    Usul::File::StreamBuffer buf ( file );

    // Set the callback.
    buf.callback ( callback );

    // Read the file.
    {
      std::istream in ( &buf );
      osgDB::ReaderWriter::ReadResult result = rw.readNode ( in );
      return result;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void Reader::_read ( const std::string &file, osgDB::ReaderWriter &rw )
{
  USUL_TRACE_SCOPE;

  // Try using the stream-buffer.
  osgDB::ReaderWriter::ReadResult result = Detail::read ( file, rw, _progress );

  // If the plugins doesn't handle input streams.
  if ( osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED == result.status() )
    result = rw.readNode ( file );

  // See if it worked.
  if ( true == result.success() && true == result.validNode() )
  {
    _node = result.getNode();
    return;
  }

  // If we get to here then it did not work.
  std::ostringstream out;
  out << "Error 1723158504: Failed to read file: " << file;
  if ( true == result.error() )
    out << ". " << result.message();
  throw std::runtime_error ( out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper predicate to determine if file name is an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct IsOsgPlugin : public std::unary_function<std::string,bool>
  {
    bool operator () ( const std::string &s ) const
    {
      std::string::size_type index ( s.find ( "osgdb_" ) );
      if ( std::string::npos != index )
      {
        index = s.find ( "d.dll", index );
        #ifdef _DEBUG
        return ( std::string::npos != index );
        #else
        return ( std::string::npos == index );
        #endif
      }
      return false;
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the filter.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void addFilter ( const std::string &name, Reader::Filters &filters )
  {
    try
    {
      std::string ext ( Usul::File::base ( name ) );
      ext.erase ( 0, 6 ); // Erase "osgdb_"
      #ifdef _DEBUG
      ext.erase ( ext.size() - 1 ); // Erase the last "d"
      #endif
      osg::ref_ptr<osgDB::ReaderWriter> rw ( osgDB::Registry::instance()->getReaderWriterForExtension ( ext ) );
      if ( rw.valid() )
      {
        std::ostringstream out;
        out << rw->className() << " (*." << ext << ')';
        filters.push_back ( Reader::Filter ( out.str(), "*." + ext ) );
      }
    }
    catch ( ... )
    {
      // Eat all exceptions.
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return list of filters.
//
///////////////////////////////////////////////////////////////////////////////

Reader::Filters Reader::filters()
{
  // Return the cached filters.
  if ( false == Helper::filters.filters.empty() )
    return Helper::filters.filters;

  // File names.
  typedef std::vector<std::string> Names;
  Names names;

  // Get the path that OSG searches.
  const osgDB::FilePathList &path ( osgDB::Registry::instance()->getLibraryFilePathList() );

  // Predicate for checking if the file is an OSG plugin.
  Helper::IsOsgPlugin pred;

  // Loop through path...
  for ( osgDB::FilePathList::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    // Find all dlls in the directory.
    const std::string &dir ( *i );
    Usul::File::find ( dir, "dll", names );

    // Remove all names that are not OSG plugins.
    names.erase ( std::remove_if ( names.begin(), names.end(), std::not1 ( pred ) ), names.end() );
  }

  // Make the filters.
  Filters filters;
  for ( Names::iterator i = names.begin(); i != names.end(); ++i )
  {
    Detail::addFilter ( *i, filters );
  }

  // Make a filter for all files types.
  if ( filters.size() > 1 )
  {
    std::string description ( "All Scene Files (" );
    std::string ext;
    for ( Filters::const_iterator i = filters.begin(); i != filters.end(); ++i )
    {
      description += ( i->second + " " );
      ext += ( i->second + ";" );
    }
    description += ")";
    ext.erase ( ext.size() - 1 ); // Lose the last ';'
    std::sort ( filters.begin(), filters.end() );
    filters.insert ( filters.begin(), Filter ( description, ext ) );
  }

  // Sort the filters and make sure unique.
  std::sort ( filters.begin(), filters.end() );
  filters.erase ( std::unique ( filters.begin(), filters.end() ), filters.end() );

  // Cache the results
  Helper::filters.filters = filters;

  // Clean up
  osgDB::Registry::instance()->clearObjectCache();
  osgDB::Registry::instance()->clearArchiveCache();
  osgDB::Registry::instance()->closeAllLibraries();

  // Return the filters.
  return filters;
}


}
}
