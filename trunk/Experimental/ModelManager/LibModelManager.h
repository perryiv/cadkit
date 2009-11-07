
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _LIB_MODEL_MANAGER_H_
#define _LIB_MODEL_MANAGER_H_

#include "Export/Export.h"
#include "Timeline.h"

#include "Usul/Base/Object.h"
#include "Usul/Types/Types.h"

#include "osg/Geode"
#include "osg/Group"
#include "osg/Geometry"

namespace osg { class Node; }


class LIB_MODEL_MANAGER_EXPORT LibModelManager : public Usul::Base::Object
{
public:
 
  /// Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  
  /// OSG Typedefs
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;


  /// Data
  typedef std::vector< Timeline::RefPtr > Timelines;
  typedef std::map< std::string, Timelines > TimelineMap;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LibModelManager );

  /// Construction.
  LibModelManager();

  // Reader
  void                    read( const std::string &filename, const std::string &type, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  /// Build the scene.
  virtual osg::Node *     buildScene ( Unknown *caller = 0x0 );

  /// Clear any existing data.
  virtual void            clear ( Unknown *caller = 0x0 );
  
  void                    write() const;
  void                    write( const std::string &filename ) const;

  // Timeline Controls
  void                    next();
  void                    prev();
  void                    setTime( Usul::Types::Uint64 time );

  // Animation Controls
  void                    animate( bool value );
  bool                    animate();

  // Update
  void                    updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Add a group to the timeline collection
  void                    addToTimeline( const std::string& name, Timeline::RefPtr value );

  // Accessor methods for timeline data
  Timelines               getTimelineByName( const std::string& name );

 
protected:

  /// Do not copy.
  LibModelManager ( const LibModelManager & );
  LibModelManager &operator = ( const LibModelManager & );
  
  /// Use reference counting.
  virtual ~LibModelManager();

private:
  GroupPtr              _root;
  Usul::Types::Uint64   _currentTimeStep;
  Usul::Types::Uint64   _maxTimeSteps;
  bool                  _animateTimeline;
  TimelineMap           _timelines;
  
};



#endif // _LIB_MODEL_MANAGER_H_
