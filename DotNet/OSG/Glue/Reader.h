
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _NATIVE_OSG_READER_CLASS_
#define _NATIVE_OSG_READER_CLASS_

#include "Usul/File/StreamBuffer.h"

#include "osg/ref_ptr"
#include "osg/Node"

#include <string>
#include <fstream>

namespace osgDB { class ReaderWriter; }

namespace CadKit {
namespace OSG {
namespace Glue {


class Reader
{
public:

  typedef void (*ProgressCallback) ( unsigned long );

  Reader();
  ~Reader();

  void                  read ( const std::string &file );

  osg::Node *           node() { return _node.get(); }

  void                  callback ( ProgressCallback cb ) { _progress = cb; }

protected:

  struct Callback : public Usul::File::StreamBuffer::Callback
  {
    Callback ( Reader &reader ) : _reader ( reader ){}
    virtual void operator () ( unsigned long bytes )
    {
      _reader._notifyProgress ( bytes );
    }
  private:
    Reader &_reader;
    friend class Reader;
  };

  osgDB::ReaderWriter   *_findReader ( const std::string &file );

  void                  _notifyProgress ( unsigned long bytes );

  void                  _read ( const std::string &file, osgDB::ReaderWriter &reader );
  void                  _read ( Usul::File::StreamBuffer::BaseClass &buf, osgDB::ReaderWriter &reader );

private:

  osg::ref_ptr<osg::Node> _node;
  ProgressCallback _progress;
};


}
}
}


#endif // _NATIVE_OSG_READER_CLASS_
