
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
//  System class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_SYSTEM_CLASS_H_
#define _STAR_SYSTEM_SYSTEM_CLASS_H_

#include "StarSystem/Body.h"
#include "StarSystem/Group.h"


namespace StarSystem {


class STAR_SYSTEM_EXPORT System : public Node
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef Body::Vec3d Vec3d;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( System );

  // Constructor
  System();

  // Add a satellite.
  void                      add ( Node * );

  // Set/get the body.
  void                      body ( Body * );
  const Body *              body() const;
  Body *                    body();

  // Set/get the center.
  void                      center ( Vec3d & );
  Vec3d                     center() const;

  // Set/get the satellites.
  void                      satellites ( Group * );
  const Group *             satellites() const;
  Group *                   satellites();

protected:

  // Use reference counting.
  virtual ~System();

private:

  // No copying or assignment.
  System ( const System & );
  System &operator = ( const System & );

  void                      _destroy();

  Body::RefPtr _body;
  Group::RefPtr _satellites;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_SYSTEM_CLASS_H_
