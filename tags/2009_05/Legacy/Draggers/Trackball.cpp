
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  2D translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/Trackball.h"

#include "osgDB/Registry"

#include "osg/ShapeDrawable"
#include "osg/Geode"

#include <sstream>

#include <iostream>

#include "Usul/Interfaces/IGetBoundingBox.h"

#ifdef _WIN32
# include "windows.h"
#endif

using namespace OsgTools;
using namespace OsgTools::Draggers;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Trackball::Trackball( Usul::Interfaces::IUnknown *caller  ) : BaseClass(),
  _diff ( 0, 0, 0 ),
  _trackballSize ( 0.8f ),
  _viewer( caller ),
  _v0( 0, 0, 0 )
{
  // Make a skinny box.
  osg::ref_ptr<osg::Box> box ( new osg::Box );
  box->setCenter      ( osg::Vec3 ( 0.0f, 0.0f, 0.0f ) );
  box->setHalfLengths ( osg::Vec3 ( 0.5f, 0.5f, 0.01f ) );
  osg::ref_ptr<osg::ShapeDrawable> sd ( new osg::ShapeDrawable ( box.get() ) );

  // Add the box to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( sd.get() );

  // Add the geode.
  this->geometry ( Trackball::DRAGGING, geode.get() );
  this->geometry ( Trackball::NORMAL,   geode.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Trackball::Trackball ( const Trackball &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op ),
  _diff ( dragger._diff ),
  _trackballSize ( dragger._trackballSize ),
  _viewer ( dragger._viewer ),
  _v0 ( dragger._v0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Trackball::~Trackball()
{
  _viewer = static_cast < Usul::Interfaces::IViewport* > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Trackball::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
//  Translate2 *t = static_cast < Translate2 * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Trackball::write ( const osg::Object &object, osgDB::Output &fw )
{
 // const Translate2 *t = static_cast < const Translate2 * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register read/write functions with osgDB::Registry.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterDotOsgWrapperProxy io_proxy_trackball
(
  new Trackball,
  "Trackball",
  "Object Node Group Transform MatrixTransform",
  &Trackball::read,
  &Trackball::write,
  osgDB::DotOsgWrapper::READ_AND_WRITE
);


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Trackball::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Call the appropriate function.
  switch ( c )
  {
    case Dragger::START:
      this->_start ( pt0, pt1 );
      break;
    case Dragger::MOVE:
      this->_move ( pt0, pt1 );
      break;
    case Dragger::FINISH:
      this->_finish ( pt0, pt1 );
      break;
  };

  // Call the base class's function.
  BaseClass::execute ( c, pt0, pt1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//	This gets called when the user clicks on a dragger.
//
///////////////////////////////////////////////////////////////////////////////

void Trackball::_start ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Find the closest point. It will fail if they are parallel lines.
  osg::Vec3d initial;
  if ( !this->_closest ( pt0, pt1, initial ) )
    return;

  // Notify the client.
  this->notify ( Dragger::START );

  // Get the current translation.
  const osg::Matrixd &M = this->getMatrix();
  osg::Vec3d trans ( M.getTrans() );

  // Save the initial difference in the click location and the dragger's 
  // location. This is to prevent it from "jumping".
  _diff = trans - initial;

  _v0 = pt1;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Move and finish both call this.
//
///////////////////////////////////////////////////////////////////////////////

void Trackball::_common ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Find the closest point. It will fail if they are parallel lines.
  osg::Vec3d point;
  if ( !this->_closest ( pt0, pt1, point ) )
    return;

  // Set the matrix.
  osg::Vec3 axis;
  float angle;

#if 1

  if ( !_viewer.valid () )
    return;

  float p0x ( 2.0f*(_v0.x())/(_viewer->width())-1.0f );
  float p0y ( 2.0f*(_v0.y())/(_viewer->height())-1.0f );
  
  float p1x ( 2.0f*(pt1.x())/(_viewer->width())-1.0f );
  float p1y ( 2.0f*(pt1.y())/(_viewer->height())-1.0f );

  //_v0 = pt1;
#else
  float p0x ( _v0.x() );
  float p0y ( _v0.y() );
  
  float p1x ( pt1.x() );
  float p1y ( pt1.y() );

  std::cout << "Point: " << pt1[0] << " " << pt1[1] << " " << pt1[2] << std::endl << std::endl;
            //<< "Vector 2: " << v2[0] << " " << v2[1] << " " << v2[2] << std::endl
    
#endif
  this->trackball( axis, angle, p0x, p0y, p1x, p1y );
  osg::MatrixTransform::setMatrix ( osg::Matrixd::rotate( angle, axis ) );

#if 0
  std::ostringstream out;
  const osg::Matrixf M = this->getMatrix();
  out << "translation = " << M.getTrans()[0] << " " << M.getTrans()[1] << " " << M.getTrans()[2] << std::endl;
  ::OutputDebugString ( out.str().c_str() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//	This gets called when the user moves a dragger.
//
///////////////////////////////////////////////////////////////////////////////

void Trackball::_move ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Delegate.
  this->_common ( pt0, pt1 );

  // Notify the client.
  this->notify ( Dragger::MOVE );
}


///////////////////////////////////////////////////////////////////////////////
//
//	This gets called when the user releases a dragger.
//
///////////////////////////////////////////////////////////////////////////////

void Trackball::_finish ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Delegate.
  this->_common ( pt0, pt1 );

  // Notify the client.
  this->notify ( Dragger::FINISH );
}


/////////////////////////////////////////////////////////////////////////////
//
//	Get the closest point on the draggers plane to the given line. Return
//	false if they are parallel.
//
/////////////////////////////////////////////////////////////////////////////

bool Trackball::_closest ( const osg::Vec3d &wp0, const osg::Vec3d &wp1, osg::Vec3d &point )
{
  // Grab the matrices.
  const osg::Matrixd &C = Trackball::dragging().value().cumulative;
  osg::Matrixd IC;
  if ( !IC.invert ( C ) )
    return false;

  // Define a local xy-plane.
  osg::Vec3d planeOrigin ( 0, 0, 0 );
  osg::Vec3d planeNormal ( 0, 0, 1 );

  // Transform the given line to local space.
  const osg::Vec3d lp0 ( wp0 * IC );
  const osg::Vec3d lp1 ( wp1 * IC );

  // Define a line as a point and direction vector.
  osg::Vec3d linePoint ( lp0 );
  osg::Vec3d lineDir   ( lp1 - linePoint );
  lineDir.normalize();

  // Find the location on the plane that is closest to this line.
  // It will fail if they are parallel.
  return this->_intersect ( planeOrigin, planeNormal, linePoint, lineDir, point );
}


void Trackball::trackball(osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y)
{
    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */

  osg::Quat rotation;
  this->getMatrix().get( rotation );
    osg::Matrix rotation_matrix(rotation);

#if 1
    osg::Vec3 uv = osg::Vec3(0.0f,1.0f,0.0f)*rotation_matrix;
    osg::Vec3 sv = osg::Vec3(1.0f,0.0f,0.0f)*rotation_matrix;
    osg::Vec3 lv = osg::Vec3(0.0f,0.0f,-1.0f)*rotation_matrix;

    osg::Vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(_trackballSize, p1x, p1y);
    osg::Vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(_trackballSize, p2x, p2y);

    /*
     *  Now, we want the cross product of P1 and P2
     */

// Robert,
//
// This was the quick 'n' dirty  fix to get the trackball doing the right 
// thing after fixing the Quat rotations to be right-handed.  You may want
// to do something more elegant.
//   axis = p1^p2;
axis = p2^p1;
    axis.normalize();

    /*
     *  Figure out how much to rotate around that axis.
     */
    float t = (p2 - p1).length() / (2.0 * _trackballSize);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    angle = osg::inRadians(asin(t));
#else

    this->getBound();
    Usul::Interfaces::IGetBoundingBox::ValidQueryPtr getBB ( _viewer );
    osg::BoundingBox bb ( getBB->getBoundingBox() );
    osg::BoundingSphere bs ( bb );
    float trackballSize ( this->getBound().radius() );


    osg::Vec3 v1, v2;

    v1[0] = p1x; v1[1] = p1y; 
    v1[2] = tb_project_to_sphere(trackballSize,p1x,p1y);

    v2[0] = p2x; v2[1] = p2y; 
    v2[2] = tb_project_to_sphere(trackballSize,p2x,p2y);

        
    /* v is the normal of rotating plane */
    axis = v2 ^ v1;
    axis.normalize();

    /* compute the angle it rotates */

    float f = (v1 * v2 )/ ( v1.length() * v2.length() );
    if(f<-1.0) f=-1.0;
    else if(f>1.0) f=1.0;

    angle = osg::inRadians(acos(f));

    //std::cout << "Vector 1: " << v1[0] << " " << v1[1] << " " << v1[2] << std::endl
    //          << "Vector 2: " << v2[0] << " " << v2[1] << " " << v2[2] << std::endl
    //          << "Angle: " << angle << std::endl << std::endl;

#endif
}


/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
float Trackball::tb_project_to_sphere(float r, float x, float y)
{
#if 1
    float d, t, z;

    d = sqrt(x*x + y*y);
                                 /* Inside sphere */
    if (d < r * 0.70710678118654752440)
    {
        z = sqrt(r*r - d*d);
    }                            /* On hyperbola */
    else
    {
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
#else
  double z;

    double RadiusSqr=r*r;
    double Cutoff=RadiusSqr*0.5;
    const double Rho=x*x+y*y;
    z=(Rho<Cutoff)?sqrt(RadiusSqr-Rho):(Cutoff/sqrt(Rho));
    return z;
#endif

}

