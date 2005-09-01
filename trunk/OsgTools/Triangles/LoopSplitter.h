///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __LOOP_SPLITTER_H__
#define __LOOP_SPLITTER_H__

#include "OsgTools/Triangles/Loop.h"

#include "OsgTools/Export.h"
#include "OsgTools/Triangles/SharedVertex.h"

#include "Usul/Interfaces/IUnknown.h"


#include <vector>
#include <list>

class LoopSplitter
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

    // Typedefs.
    typedef OsgTools::Triangles::SharedVertex::ValidRefPtr  SharedVertexPtr;
    typedef SharedVertexPtr                                 value_type;
    typedef std::vector< value_type >                       Points;
    typedef Points::iterator                                iterator;
    typedef Points::const_iterator                          const_iterator;

    typedef std::map<int, std::vector<osg::Vec3f> >   EdgeMap;
    typedef std::map<osg::Vec3f, unsigned int>        TPointIndices;
    typedef std::vector<OsgTools::Triangles::Loop>    Loops;
    
    /// Constructor
    LoopSplitter(osg::BoundingBox box, Usul::Interfaces::IUnknown *caller);
  
    /// This is the main entry point to split loops up
    Loops split( const OsgTools::Triangles::Loop& loop );
    
  protected:
    
    void              _findTransitionPoints( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices);
    void              _createLoops( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops);
    bool              _makeCornerDecision(int a, int b, int c,int e1, int e2) const;
    osg::Vec3f        _getCornerPoint(int edge1, int edge2) const;
    int               _getSortAxis(int edge1) const ;
    void              _printEdgeMap(EdgeMap edgeMap);
    
  private:
    LoopSplitter();
    osg::BoundingBox _boundBox;
    Usul::Interfaces::IUnknown::ValidAccessQueryPtr  _caller;
};


#endif //__LOOP_SPLITTER_H__