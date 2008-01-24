
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base dragger class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAGGER_CLASS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"
#include "OsgTools/Draggers/Callback.h"

#include <osg/MatrixTransform>
#include <osg/ref_ptr>

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Variable.h"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Dragger : public osg::MatrixTransform
{
public:

  // Useful typedefs
  typedef osg::MatrixTransform BaseClass;
  typedef osg::ref_ptr<Dragger> Ptr;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  // Small struct to hold data for the dragger being dragged.
  struct Data
  {
    Data ( Dragger *d, const osg::Matrixd &m = osg::Matrixd::identity() );
    Data ( const Data &d );
    Data &operator = ( const Data & );
    Dragger::Ptr dragger;
    osg::Matrixd cumulative;
  };
  typedef Usul::Threads::Variable<Data> Dragging;

  // Plumbing.
  META_Node ( OsgTools, Dragger );

  // Different states.
  enum State { DRAGGING = 0, NORMAL };

  // Commands.
  enum Command { START, MOVE, FINISH };

  // Set/get the callback.
  void                      callback ( Command, Callback * );
  Callback *                callback ( Command );

  // Get/set the dragger currently being dragged. Could be null.
  static Dragging &         dragging();
  static void               dragging ( Dragger *, const osg::Matrixd & );

  // Execute the command.
  virtual void              execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // Get/set the geometry drawn while dragging/normal.
  const osg::Node *         geometry ( State s ) const { return this->getChild ( s ); }
  osg::Node *               geometry ( State s )       { return this->getChild ( s ); }
  void                      geometry ( State s, osg::Node *n );

  // Is this dragger currently being dragged?
  bool                      isDragging() const;

  // Notify the client that something happened.
  void                      notify ( Command c );

  // Traverse the correct child.
  virtual void              traverse ( osg::NodeVisitor & );

  void                      useWindowCoords ( bool b ) { _useWindowCoords = b; }
  bool                      useWindowCoords () const   { return _useWindowCoords; }

  // File I/O.
  static bool               read  (       osg::Object &, osgDB::Input  & );
  static bool               write ( const osg::Object &, osgDB::Output & );

protected:

  // Constructors.
  Dragger();
  Dragger ( const Dragger &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Use reference counting.
  virtual ~Dragger();

  void                      _resize ( Command c );

  bool                      _intersect ( const osg::Vec3d &planeOrigin, 
                                         const osg::Vec3d &planeNormal, 
                                         const osg::Vec3d &linePoint, 
                                         const osg::Vec3d &lineDir, 
                                         osg::Vec3d &point ) const;

private:

  typedef osg::ref_ptr<Callback> CB;
  std::vector<CB> _callbacks;
  bool _useWindowCoords;
  static Dragging _dragging;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAGGER_CLASS_H_
