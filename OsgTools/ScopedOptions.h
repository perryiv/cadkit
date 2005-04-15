
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scoped database registry options.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_SCOPED_DATABASE_REGISTRY_OPTIONS_H_
#define _OSG_TOOLS_SCOPED_DATABASE_REGISTRY_OPTIONS_H_

#include "osgDB/Registry"
#include "osgDB/ReaderWriter"

#include <string>


namespace OsgTools {


struct ScopedOptions
{
  typedef osgDB::ReaderWriter::Options Options;

  ScopedOptions() : _original ( osgDB::Registry::instance()->getOptions() )
  {
  }

  ScopedOptions ( const std::string &options ) : _original ( osgDB::Registry::instance()->getOptions() )
  {
    this->_setNewOptions ( options );
  }

  ScopedOptions ( const osgDB::ReaderWriter::Options *options ) : _original ( osgDB::Registry::instance()->getOptions() )
  {
    this->_setNewOptions ( ( options ) ? options->getOptionString() : std::string() );
  }

  ~ScopedOptions()
  {
    osgDB::Registry::instance()->setOptions ( _original.get() );
  }

protected:

  void _setNewOptions ( const std::string &options )
  {
    if ( !options.empty() )
    {
      osg::ref_ptr<Options> newOptions ( new Options );
      newOptions->setOptionString ( options );
      osgDB::Registry::instance()->setOptions ( newOptions.get() );
    }
  }

private:

  ScopedOptions ( const ScopedOptions & );
  ScopedOptions &operator = ( const ScopedOptions & );

  osg::ref_ptr<Options> _original;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SCOPED_DATABASE_REGISTRY_OPTIONS_H_
