///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Bond.h"
#include "Atom.h"

#include <sstream>

#include "osg/Matrixd"

using namespace osgPlugins;
using namespace osgPlugins::pdb;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Bond::Bond(Atom::Ptr lhs, Atom::Ptr rhs, int id)  :
BaseClass (),
_id       ( id ),
_point1   ( lhs->getCenter() ),
_point2   ( rhs->getCenter() ),
_matrix   ( new osg::MatrixTransform )
{
  osg::Vec3 uY (0.0, 1.0, 0.0);

  //get distance
  osg::Vec3 dist = _point2 - _point1;
  float d = dist.length();
  dist.normalize();
  
  //set up Matrix Transforms
  osg::Matrixf scale, rotate, translate, matrix;
  scale = matrix.scale( osg::Vec3(1, d, 1) );
  rotate.makeRotate(uY, dist);
  translate.setTrans(_point1);
  osg::Matrixf T = scale * rotate * translate;

  // Make a matrix-transform.
  _matrix->setMatrix ( T );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Bond::~Bond()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns data members as a string.
//
///////////////////////////////////////////////////////////////////////////////

const std::string Bond::toString() const
{
  std::ostringstream out;
	out << _id << " " << _point1[0] << " " <<  _point1[1] << " " <<  _point1[2];
	return out.str();
}
