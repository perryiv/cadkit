
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_BODY_CLASS_H_
#define _STAR_SYSTEM_BODY_CLASS_H_

#include "StarSystem/Node.h"

namespace osgDB { class DatabasePager; }
class ossimPlanet;


namespace StarSystem {


class STAR_SYSTEM_EXPORT Body : public Node
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef osgDB::DatabasePager Pager;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( Body );

  // Constructor
  Body();

  // Build the scene.
  osg::Node *               buildScene ( const BuildOptions &options, Usul::Interfaces::IUnknown *caller );

  // Get the database pager.
  Pager *                   databasePager();

protected:

  // Use reference counting.
  virtual ~Body();

private:

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  ossimPlanet *_planet;
  Pager *_pager;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BODY_CLASS_H_
