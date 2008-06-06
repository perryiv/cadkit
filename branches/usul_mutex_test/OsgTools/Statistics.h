
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for gathering statistics about a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_SCENE_STATISTICS_H_
#define _OSG_TOOLS_SCENE_STATISTICS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include <map>


namespace OsgTools {


class OSG_TOOLS_EXPORT Statistics
{
public:

  /// Possible values to get statistics for.
  enum Type
  {
    PRIMITIVE_SETS, VERTICES, NORMALS, COLORS, POLYGON,
    POINTS, LINES, LINE_LOOPS, LINE_STRIPS, TRIANGLES, QUADS,
    TRIANGLE_STRIP, TRIANGLE_FAN, QUAD_SET, QUAD_STRIP,
    TRANSFORM, GROUP, DRAWABLES, GEOMETRY, DRAWABLE
  };

  /// Typedefs.
  typedef std::map < Type, unsigned int > Counts;

  /// Construction and destruction.
  Statistics();
  Statistics ( const Statistics & );
  ~Statistics();

  /// Accumulate statistics.
  void                    accumulate ( const osg::Node * );

  /// Clear accumulated state.
  void                    clear();

  /// Get the count of a particular type.
  unsigned int            count ( Type ) const;

  /// Pass true if triangles and quads should be counted for tri-strips and quad-strips.
  void                    greedy ( bool g ) { _greedy = g; }
  bool                    greedy() const { return _greedy; }

  /// Assignment.
  Statistics &            operator = ( const Statistics & );

protected:

  void                    _countGeodes     ( osg::Geode * );
  void                    _countGroups     ( osg::Group * );
  void                    _countModes      ( const osg::PrimitiveSet * );
  void                    _countTransforms ( osg::Transform * );

private:

  Counts _counts;
  bool _greedy;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SCENE_STATISTICS_H_
