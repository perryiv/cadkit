
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

#include "Usul/Math/Vector3.h"

namespace osg { class MatrixTransform; }
class ossimPlanet;


namespace StarSystem {


class STAR_SYSTEM_EXPORT Body : public Node
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef Usul::Math::Vec3d Vec3d;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( Body );

  // Constructor
  Body();

  // Set/get the center.
  void                      center ( Vec3d & );
  Vec3d                     center() const;

  // Get the scene.
  const osg::Node *         scene() const;
  osg::Node *               scene();

protected:

  // Use reference counting.
  virtual ~Body();

private:

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  ossimPlanet *_planet;
  osg::MatrixTransform *_transform;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BODY_CLASS_H_
