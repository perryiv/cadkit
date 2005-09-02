/*
 *  LoopSplitter.cpp
 *  OsgTools
 *
 *  Created by Michael A Jackson on 9/1/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "LoopSplitter.h"
#include "OsgTools/Triangles/Loop.h"


#include <iostream>
#include <string>
#include <algorithm>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::vector< unsigned int > UsedIndices;
typedef std::vector< unsigned int > TransitionPoints;


namespace Detail
{

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Function to build a co planer loop.  Returns false if it reaches a
  //  transition point that is incorrect.
  //
  ///////////////////////////////////////////////////////////////////////////////

  bool buildLoop ( const Loop& loop, Loop &fixedLoop, unsigned int start, unsigned int stop, const TransitionPoints &transitions, UsedIndices& used )
  {
    //Copy from old loop into new loop
    for ( Loop::const_iterator iter = loop.begin() + start; iter != loop.begin() + stop + 1; iter++) 
    {
      if( iter == loop.end() )
        iter = loop.begin();
      
      unsigned int index ( iter - loop.begin() );

      fixedLoop.push_back(*iter);
      
      if ( start != index && stop != index ) 
      {
        // Is index a transition point that we aren't looking for?
        if ( transitions.end() != std::find ( transitions.begin(), transitions.end(), index ) )
        {
          std::cout << "Unexpected transition point " << index << std::endl << "Expecting " << stop << std::endl;
          return false;
        }

        used.push_back( index );
      }
    }

    return true;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the axis (x, y, or z) to use for the sorting
//
///////////////////////////////////////////////////////////////////////////////

int LoopSplitter::_getSortAxis(int edge) const
{
  switch (edge) 
  {
    case ZMIN_YMIN:
    case ZMAX_YMIN:
    case ZMIN_YMAX:
    case ZMAX_YMAX:
      return X_AXIS;
              
    case XMIN_YMIN:
    case XMAX_YMIN:
    case XMIN_YMAX:
    case XMAX_YMAX:
      return Z_AXIS;

    case XMAX_ZMIN:
    case XMIN_ZMIN:
    case XMIN_ZMAX:    
    case XMAX_ZMAX:
      return Y_AXIS;
      
    default:
      throw std::runtime_error("Error 94630293920: getSortAxis() - Invalid Edge value.");
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Returns the corner point of the bounding box based on a pair of edges
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f LoopSplitter::_getCornerPoint(int edge1, int edge2) const
{
 
  if( _makeCornerDecision(ZMIN_YMIN, XMIN_YMIN, XMIN_ZMIN, edge1, edge2) ) {
      return _boundBox.corner(0);
  } else if (_makeCornerDecision(XMIN_YMIN,ZMAX_YMIN,XMIN_ZMAX , edge1, edge2 ) ) {
      return _boundBox.corner(4);
  } else if (_makeCornerDecision(ZMAX_YMIN, XMAX_YMIN, XMAX_ZMAX , edge1, edge2 ) ) {
      return _boundBox.corner(5);  
  } else if (_makeCornerDecision(XMAX_YMIN,ZMIN_YMIN ,XMAX_ZMIN , edge1, edge2 ) ) {
      return _boundBox.corner(1);  
  } else if (_makeCornerDecision(ZMIN_YMAX,XMIN_YMAX ,XMIN_ZMIN , edge1, edge2 ) ) {
      return _boundBox.corner(2);  
  } else if (_makeCornerDecision(XMIN_YMAX,ZMAX_YMAX ,XMIN_ZMAX , edge1, edge2 ) ) {
      return _boundBox.corner(6);  
  } else if (_makeCornerDecision(ZMAX_YMAX,XMAX_YMAX,XMAX_ZMAX , edge1, edge2 ) ) {
      return _boundBox.corner(7);  
  } else if (_makeCornerDecision(XMAX_YMAX,XMAX_ZMIN ,ZMIN_YMAX , edge1, edge2 ) ) {
      return _boundBox.corner(3);  
  }
         
  throw std::runtime_error ( "Error 94630293921: getCornerPoint() - Invalid edge values." );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper method for getCornerPoint
//
///////////////////////////////////////////////////////////////////////////////

bool LoopSplitter::_makeCornerDecision(int a, int b, int c,int e1, int e2) const
{
  
  if ( (e1 == a && e2 == b) || (e1 == b && e2 == c) || (e1 == c && e2 == a) ) 
  {
    return true;
  }
  
  if ( (e2 == a && e1 == b) || (e2 == b && e1 == c) || (e2 == c && e1 == a) )
  {
  return true;
  }
  return false;
}

void LoopSplitter::_printEdgeMap(EdgeMap edgeMap)
{
  std::cout << "EdgeMap" << std::endl;
  for (EdgeMap::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i) {
    std::cout << i->first << std::endl;
  }
  std::cout << "---" << std::endl;
}




///////////////////////////////////////////////////////////////////////////////
//
// Sort Predicate that will compare the osg::Vec3f based on a single Axis
// component.
//
///////////////////////////////////////////////////////////////////////////////

struct AxisSort
{
  
  AxisSort(int a) {
    axis = a;
  }
  
  bool operator () ( const osg::Vec3f& p1, const osg::Vec3f &p2 ) const
  {
      if (axis == LoopSplitter::X_AXIS) {
        return (p1.x()<p2.x());
      } else if (axis == LoopSplitter::Y_AXIS ) {
        return (p1.y()<p2.y());
      } else if (axis == LoopSplitter::Z_AXIS ) {
        return (p1.z()<p2.z());
      } 
        throw std::runtime_error("Error 894587302 - Axis invalid for AxisSort");
  }
  
  private:
    int axis;
};


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

LoopSplitter::LoopSplitter(osg::BoundingBox box, Usul::Interfaces::IUnknown *caller) :
   _boundBox ( box ), _caller ( caller )
{
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

std::vector<OsgTools::Triangles::Loop> LoopSplitter::split( const OsgTools::Triangles::Loop& loop )
{

  //add in a vector to get the shared vertex objects that are transition points 
  /**
  * Key = the Enumeration value for the edge that the SharedVertex is on
  * Value = A vector of SharedVertex objects that lie on the given edge
  */
  std::map<int, std::vector<osg::Vec3f> >  edgeMap;
  /**
  * Key = SharedVertex that lies on an edge
  * Value = The index in the Loop object where that SharedVertex is
  */
  std::map<osg::Vec3f, unsigned int> tPointIndices;
  
  std::vector<OsgTools::Triangles::Loop> loops;
  
  _findTransitionPoints(loop, edgeMap, tPointIndices);
  
  _createLoops(loop, edgeMap, tPointIndices, loops);
  
  return loops;
}



///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_findTransitionPoints( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices) 
{

    float xMin, xMax, yMin, yMax, zMin, zMax;
    xMin = _boundBox.xMin();
    xMax = _boundBox.xMax();
    yMin = _boundBox.yMin();
    yMax = _boundBox.yMax();
    zMin = _boundBox.zMin();
    zMax = _boundBox.zMax();

    for( unsigned int i = 0; i < loop.size(); ++i)
    {
       const osg::Vec3 & v ( loop.vertex( i, _caller ) );
       if (v.z() == zMin && v.y() == yMin ) { 
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMIN].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.y() == yMin ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMin ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMin ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.z() == zMin && v.y() == yMax ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.y() == yMax ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMax ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMax ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMAX ].push_back(v);
       tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.z() == zMin ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.z() == zMin ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.z() == zMax ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       }  else if (v.x() == xMax && v.z() == zMax ) {
        std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_ZMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       }
    }
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the loop up into n co-planer loops.
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_createLoops( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{  
  //Start figuring out what we have. There are a few special cases that we know how to handle.
  
  //There are an EVEN number of transition points, and they are all on the same edge
  
  if ( edgeMap.size() == 1 ) {
    std::vector<osg::Vec3f> & v = edgeMap.begin()->second;
    int edge = edgeMap.begin()->first;

    UsedIndices usedIndices; //Store indices that need to be erased from the loop object
    
    std::cout << "Edge Number: " << edge << std::endl;
    std::cout << "Vector size: " << v.size() << std::endl;
    if (v.size()%2 == 0) 
    {      
      //Even number of points
      std::sort(v.begin(), v.end(), AxisSort( _getSortAxis(edge) ) );

      TransitionPoints transitionPoints;

      // Place all transition points in a vector to test against.
      for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
        transitionPoints.push_back( iter->second );

      // Start and stop values for building the loop.
      int startIndex = 0;
      int stopIndex = 0;
      
      std::cout << "Pairs that should be together:" << std::endl;
      for ( unsigned int i = 0; i < v.size();i+=2) 
      {
        Loop fixedLoop;
        TPointIndices::iterator it = tPointIndices.find ( v[i] );
        if ( tPointIndices.end() != it ) 
        {
          startIndex = it->second;
        }
        TPointIndices::iterator ite = tPointIndices.find ( v[i+1] );
        if ( tPointIndices.end() != ite ) 
        {
          stopIndex = ite->second;
        }

        std::cout << "[ " << startIndex << ", " << stopIndex << " ]" << std::endl;

        UsedIndices local;

        if ( Detail::buildLoop( loop, fixedLoop, startIndex, stopIndex, transitionPoints, local ) )
        {
          loops.push_back(fixedLoop);
        }
        else
        {
          local.clear();
          fixedLoop.clear();

          // try it again with opposite starting points.
          if ( false == Detail::buildLoop( loop, fixedLoop, stopIndex, startIndex, transitionPoints, local ) )
            throw std::runtime_error ("Cannot split this loop." );
          loops.push_back( fixedLoop );
        }

        usedIndices.insert( usedIndices.end(), local.begin(), local.end() );
        //std::copy( local.begin(), local.end(), usedIndices.end() );
      }  //End for loop
     
      std::cout << "UsedIndices Size: " << usedIndices.size() << std::endl;
      //Erase the indices from a copy of the parent loop
      Loop parentCopy ( loop ); //Copy Constructor
      std::sort(usedIndices.begin(), usedIndices.end() );
      for ( UsedIndices::reverse_iterator it = usedIndices.rbegin();it != usedIndices.rend(); it++ ) 
      {
        unsigned int index ( *it );

        if( index >= parentCopy.size() || index < 0 )
        {
          std::ostringstream os;
          os << "Index: " << index << " is invalid. " << std::endl << "Number of indices: " << parentCopy.size() << std::endl;
          throw std::runtime_error ( os.str() );
        }
        parentCopy.erase( *it );
      }
      loops.push_back(parentCopy);
      //There is one more loop to find.
    } else {
      //This is really bad.
      throw std::runtime_error("Error:2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
    }
  }
  
  //If there are 3 edges involved and only 3 transition points, then we are
  // going to have to add the corner point into the mix as a transition point
  if ( edgeMap.size() == 3) {
    bool ok = true;
    //Now test the size of each vector to make sure it is one.
    for (EdgeMap::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i) {
       if (i->second.size() != 1) { ok = false; }
    }
    if (ok == true) {
      //Make a new Loop
      std::cout << "This is config is NOT Handled Yet" << std::endl;
    } else {
       // We have some other configuration that needs to be reduced to this first
       throw std::runtime_error("Error 293480987 - Unknow Transition point configuration.");
    }
  }
  
}


