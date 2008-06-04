///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __LOOP_SPLITTER_H__
#define __LOOP_SPLITTER_H__

#include "OsgTools/Export.h"
#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Triangles/Loop.h"
#include "OsgTools/Triangles/SharedVertex.h"

#include "Usul/Interfaces/IUnknown.h"

namespace Detail { struct Edge; }

#include <vector>
#include <list>


namespace OsgTools {
namespace Triangles {


class OSG_TOOLS_EXPORT LoopSplitter
{
  public:


    enum AXIS_LIST 
    {
      X_AXIS = 0,
      Y_AXIS,
      Z_AXIS
    };

    enum  EDGE_LIST 
    {
      ZMIN_YMIN = 1,
      XMIN_YMIN = 2,
      ZMAX_YMIN = 3,
      XMAX_YMIN = 4,
      ZMIN_YMAX = 5,
      XMIN_YMAX = 6,
      ZMAX_YMAX = 7,
      XMAX_YMAX = 8,
      XMAX_ZMIN = 9,
      XMIN_ZMIN = 10,
      XMIN_ZMAX = 11,
      XMAX_ZMAX = 12
    };   
    
    enum OSG_BOUNDBOX
    {
      XMIN_YMIN_ZMIN = 0,
      XMAX_YMIN_ZMIN = 1,
      XMIN_YMAX_ZMIN = 2,
      XMAX_YMAX_ZMIN = 3,
      XMIN_YMIN_ZMAX = 4,
      XMAX_YMIN_ZMAX = 5,
      XMIN_YMAX_ZMAX = 6,
      XMAX_YMAX_ZMAX = 7     
    };

///////////////////////////////////////////////////////////////////////////////
//
// These values were created by adding the values of the 3 edges that connect
//  to form the corner point. Nicely enough, these values are unique
//
///////////////////////////////////////////////////////////////////////////////

    enum CORNER_EDGE
    {
      CE_XMIN_YMIN_ZMIN = 13,
      CE_XMAX_YMIN_ZMIN = 14,
      CE_XMIN_YMAX_ZMIN = 21,
      CE_XMAX_YMAX_ZMIN = 22,
      CE_XMIN_YMIN_ZMAX = 16,
      CE_XMAX_YMIN_ZMAX = 19,
      CE_XMIN_YMAX_ZMAX = 24,
      CE_XMAX_YMAX_ZMAX = 27     
    };

    // Typedefs.
    typedef OsgTools::Triangles::SharedVertex::ValidRefPtr  SharedVertexPtr;
    typedef SharedVertexPtr                                 value_type;
    typedef std::vector< value_type >                       Points;
    typedef Points::iterator                                iterator;
    typedef Points::const_iterator                          const_iterator;

    typedef std::map<int, std::vector<osg::Vec3f> >       EdgeMap;
    typedef std::map<osg::Vec3f, unsigned int>            TPointIndices;
    typedef std::vector<OsgTools::Triangles::Loop>        Loops;
    typedef std::map<int, Points>                         EdgeVertMap;
    typedef std::vector< Detail::Edge >                   Edges;

    typedef std::vector< unsigned int >                   UsedIndices;
    typedef std::vector< unsigned int >                   TransitionPoints;
    /// Constructor
    LoopSplitter(osg::BoundingBox box, Usul::Interfaces::IUnknown *caller);
  
    /// This is the main entry point to split loops up
    Loops split( const OsgTools::Triangles::Loop& loop );
    
  protected:
    
    void              _findTransitionPoints( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices);
    void              _createLoops( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops);
    bool              _makeCornerDecision(int a, int b, int c,int e1, int e2) const;
    osg::Vec3f        _getCornerPoint(int edge1, int edge2) const;
    int               _getBoundBoxIndex(int edgeTotal) const ;
    int               _getSortAxis(int edge1) const ;
    void              _printEdgeMap(EdgeMap edgeMap);
    void              _singleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Edges &edges, Loops & loops);
    void              _doubleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Edges &edges, Loops & loops);
    void              _tripleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Edges &edges, Loops & loops);
    void              _addNewPoints(int edge, int corner, float length, osg::Vec3 & tp, std::vector<osg::Vec3> & verts);
    void              _edgeHelper(const OsgTools::Triangles::Loop& loop, const unsigned int edge, std::vector<osg::Vec3> & transitionPoints, TPointIndices & tPointIndices, Edges &e, Loops & loops, UsedIndices & usedIndices  );
    void              _parentLoopCopier(const OsgTools::Triangles::Loop & loop, Loops & loops, UsedIndices & usedIndices);
  private:
    LoopSplitter();
    osg::BoundingBox _boundBox;
    Usul::Interfaces::IUnknown::ValidAccessQueryPtr  _caller;
};


} // namespace Triangles
} // namespace OsgTools


#endif //__LOOP_SPLITTER_H__
