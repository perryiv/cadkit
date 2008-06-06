
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

#ifndef _OSG_TOOLS_READER_CLASS_
#define _OSG_TOOLS_READER_CLASS_

#include "OsgTools/Export.h"

#include "Usul/File/StreamBuffer.h"

#include "osg/ref_ptr"
#include "osg/Node"

#include <string>
#include <fstream>
#include <vector>

namespace osgDB { class ReaderWriter; }


namespace OsgTools {
namespace IO {


class OSG_TOOLS_EXPORT Reader
{
public:
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;
  typedef Usul::File::StreamBuffer::Callback    Callback;

  Reader();
  ~Reader();

  static Filters        filters();

  static bool           hasReader ( const std::string &file );

  osg::Node *           node() { return _node.get(); }

  void                  read ( const std::string &file );

  /// Callback.
  template < class Fun >
  struct ReaderCallback : public Callback
  {
    ReaderCallback ( Fun callback ) : _callback ( callback ) {}
    virtual void operator () ( const std::string &file, unsigned long bytes, unsigned long total )
    {
      _callback ( file, bytes, total );
    }
  private:
    Fun _callback;
  };

  /// Typedefs for backwards compatibilty.
  typedef void (*ProgressCallback) ( const std::string &, unsigned long, unsigned long );
  typedef ReaderCallback < ProgressCallback > FunctionCallback;

  void                  callback ( Callback *cb ) { _progress = cb; }

protected:

  osgDB::ReaderWriter   *_findReader ( const std::string &file );

  void                  _read ( const std::string &file, osgDB::ReaderWriter &reader );
  void                  _read ( Usul::File::StreamBuffer::BaseClass &buf, osgDB::ReaderWriter &reader );

private:

  osg::ref_ptr<osg::Node> _node;
  Callback *_progress;
};


}
}


#endif // _OSG_TOOLS_READER_CLASS_
