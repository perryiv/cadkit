
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ANIMATE_FRAME_H__
#define __ANIMATE_FRAME_H__

#include "Helios/Animate/Export.h"

#include "osg/Vec3"
#include "osg/Quat"

#include <string>
#include <list>

namespace Helios {
namespace  Animate {
  
class ANIMATE_EXPORT Frame
{
public:
  typedef std::list< std::string >  Comments;
  typedef Comments::iterator        iterator;
  typedef Comments::const_iterator  const_iterator;

  Frame() :
    _center   ( ),
    _distance (  ),
    _rotation (  ),
    _name     ( "" ),
    _comments ()
    { }

  Frame ( const osg::Vec3& center, float distance, const osg::Quat& rotation, const std::string& name = "") :
    _center   ( center ),
    _distance ( distance ),
    _rotation ( rotation ),
    _name     ( name ),
    _comments ()
    {
    }

  //Get/Set the center
  const osg::Vec3&    getCenter () const { return _center; }
  void                setCenter ( const osg::Vec3& c ) { _center = c; }

  //Get/Set the distance
  const float         getDistance () const { return _distance; }
  void                setDistance ( float d ) { _distance = d; }

  //Get/Set the rotation
  const osg::Quat&    getRotation() const { return _rotation; }
  void                setRotation( const osg::Quat& r ) { _rotation = r; }

  //Get/Set the name
  const std::string&  getName() const { return _name; }
  void                setName( const std::string& n ) { _name = n; }

  //Add a comment
  void                addComment ( const std::string& s) { _comments.push_back( s ); }

  //Iterators to access comments
  iterator            begin()       { return _comments.begin(); }
  const_iterator      begin() const { return _comments.begin(); }

  iterator            end()       { return _comments.end(); }
  const_iterator      end() const { return _comments.end(); }

private:
  osg::Vec3                _center;
  float                    _distance;
  osg::Quat                _rotation;
  std::string              _name;
  std::list< std::string > _comments;

}; //class Frame

} // namespace Animate
} // namespace Helios

#endif //__ANIMATE_FRAME_H__

