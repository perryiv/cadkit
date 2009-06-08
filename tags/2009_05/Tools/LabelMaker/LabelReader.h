
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LabelReader_H
#define LabelReader_H

//#include <string>

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osgDB/ReadFile"

#include "osg/Node"



class LabelReader : public Usul::Base::Referenced
{
public:

  struct Location
  { 
    osg::Vec3f pos;
    std::string text;
  };

  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector< Location > Locations;

  USUL_DECLARE_REF_POINTERS ( LabelReader );

  LabelReader();

  void                  read( const std::string & filename );
  osg::Node *           buildScene(  bool box );

protected:

  virtual ~LabelReader();

  void                  _read( const std::string & filename );
  osg::Node*            _createText( unsigned int index );
  osg::Node*            _drawBox();
  osg::Vec3f            _min;
  osg::Vec3f            _max;
                

private:
  Locations                   _locations;
  osg::ref_ptr< osg::Group >   _scene;
};

#endif