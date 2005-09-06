/*
 *  LoopSplitter.cpp
 *  OsgTools
 *
 *  Created by Michael A Jackson on 9/1/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

/*

   *------------*
   |            |
   |            |
   |            






*/
#include "LoopSplitter.h"
#include "OsgTools/Triangles/Loop.h"
#include "Usul/Interfaces/IAddSharedVertex.h"
#include "Usul/Interfaces/IUnknown.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////




namespace Detail
{

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Function to build a co planer loop.  Returns false if it reaches a
  //  transition point that is incorrect.
  //
  ///////////////////////////////////////////////////////////////////////////////

  bool buildLoop ( const Loop& loop, Loop &fixedLoop, unsigned int start, unsigned int stop, const LoopSplitter::TransitionPoints &transitions, LoopSplitter::UsedIndices& used )
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
  
  float avgSegmentLength( const Loop& loop, Usul::Interfaces::IUnknown *_caller ) 
  {
    float l = 0.0f;
    unsigned int size = loop.size();
    for( unsigned int i = 0; i < size -1; ++i)
    {
     osg::Vec3 v1 ( loop.vertex( i, _caller ) );
     osg::Vec3 v2 ( loop.vertex( i+1, _caller ) );
     l += (v1-v2).length();
    }
    l = l/size;
    return l;
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
// Returns the corner point of the bounding box based on the total of the 
//  three edges
//
///////////////////////////////////////////////////////////////////////////////

int LoopSplitter::_getBoundBoxIndex(int edgeTotal) const 
{
  switch(edgeTotal) 
  {
      case CE_XMIN_YMIN_ZMIN: 
        return XMIN_YMIN_ZMIN;
      case CE_XMAX_YMIN_ZMIN: 
        return XMAX_YMIN_ZMIN;
      case CE_XMIN_YMAX_ZMIN: 
        return XMIN_YMAX_ZMIN;
      case CE_XMAX_YMAX_ZMIN: 
        return XMAX_YMAX_ZMIN;
      case CE_XMIN_YMIN_ZMAX: 
        return XMIN_YMIN_ZMAX;
      case CE_XMAX_YMIN_ZMAX: 
        return XMAX_YMIN_ZMAX;
      case CE_XMIN_YMAX_ZMAX: 
        return XMIN_YMAX_ZMAX;
      case CE_XMAX_YMAX_ZMAX:
        return XMAX_YMAX_ZMAX;
      default:
        throw std::runtime_error("Error 94630293930: _getCornerPoint() - Invalid Edge Total.");
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
// The main entry point for this class. This will split a parent loop into smaller
//  loops that are split on the edges of the bounding box.
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
// This will find the transition points where the loop moves from one plane 
//  to another.
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
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMIN].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.z() == zMin && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMAX ].push_back(v);
       tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.z() == zMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.z() == zMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.z() == zMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       }  else if (v.x() == xMax && v.z() == zMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
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
  
  /// There is only 1 Edge Involved in the loop
  if ( edgeMap.size() == 1 ) {
    _singleEdge(loop, edgeMap, tPointIndices, loops);
  }
  
  /// There are two edges involved in this loop.
  if ( edgeMap.size() == 2 ) {
    _doubleEdge(loop, edgeMap, tPointIndices, loops );
  }
  
  //If there are 3 edges involved and only 3 transition points, then we are
  // going to have to add the corner point into the mix as a transition point
  if ( edgeMap.size() == 3) {
    _tripleEdge(loop, edgeMap, tPointIndices, loops);
  }


}

///////////////////////////////////////////////////////////////////////////////
//
// This was experimental code to add addition points to the edges so the 
//  triangulation produces better triangles
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_addNewPoints(int edge, int corner, float length, osg::Vec3 & tp, std::vector<osg::Vec3> & verts) 
{
    /// Now we know which corner point to add, Add a new Shared Vertex to the Document
  Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( _caller );
  float min = -2.0f;
  float max = 0.0f;
  
  switch(edge) 
  {
      case ZMIN_YMIN:  //1
        if ( corner == XMIN_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).x();
          max = tp.x(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          }
        } else  if ( corner == XMAX_YMIN_ZMIN ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (max, tp.y(), tp.z());
            verts.push_back( v  );
            max -= length;
          }
        }
        break;
      case XMIN_YMIN:  //2
        if ( corner == XMIN_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          }    
        } else  if ( corner == XMIN_YMIN_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          } 
        }
        break;
      case ZMAX_YMIN: //3
        if ( corner == XMIN_YMIN_ZMAX ) {
          min = _boundBox.corner(corner).x();
          max = tp.x(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMIN_ZMAX ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
            while (min < max ) {
              osg::Vec3 v (max, tp.y(), tp.z());
              verts.push_back( v  );
              max -= length;
            }  
        }
    
        break;
      case XMAX_YMIN:  //4
        if ( corner == XMAX_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMIN_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          } 
        }
       
        break;
      case ZMIN_YMAX: //5
        if ( corner == XMIN_YMAX_ZMIN ) {
          min = _boundBox.corner(corner).x();
          max = tp.x();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          } 
        
        } else  if ( corner == XMAX_YMAX_ZMIN ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (max, tp.y(), tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
   
        break;
      case XMIN_YMAX:  //6
        if ( corner == XMIN_YMAX_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          }
        } else  if ( corner == XMIN_YMAX_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          }    
        }
      
        break;
      case ZMAX_YMAX:  //7
        if ( corner == XMIN_YMAX_ZMAX ) {
          min = _boundBox.corner(corner).x();
          max = tp.x();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMAX_ZMAX ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (max, tp.y(), tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
           
        break;
      case XMAX_YMAX:  //8
        if ( corner == XMAX_YMAX_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMAX_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          } 
        }
                
        break;
      case XMAX_ZMIN:  //9
        if ( corner == XMAX_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          }
        } else  if ( corner == XMAX_YMAX_ZMIN ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          }
        }
           
        break;      
      case XMIN_ZMIN: //10
        if ( corner == XMIN_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMIN_YMAX_ZMIN ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
                 
        break;      
      case XMIN_ZMAX:  //11
        if ( corner == XMIN_YMIN_ZMAX ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMIN_YMAX_ZMAX ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
            
        break;      
      case XMAX_ZMAX:  //12
        if ( corner == XMAX_YMIN_ZMAX ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          }  
        } else  if ( corner == XMAX_YMAX_ZMAX ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          }  
        }
           
        break;      
      default:
        throw std::runtime_error("Error 94630293940: _addNewPoints() Invalid Edge Value");
    }

}

///////////////////////////////////////////////////////////////////////////////
// 
// This method takes care of at least one case where the loop crosses 3 edges.
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_tripleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{
 // std::cout << "----- _tripleEdge -----" << std::endl;
  bool ok = true;
  int edgeTotal = 0;
  int bbIndex = -1;
  std::vector<osg::Vec3f> v; //Create a new Vector to hold the Vec3f
  std::vector<unsigned int> edges; //Create a Vector to hold the Edge Numbers
  
  //Now test the size of each vector to make sure it is one.
  for (EdgeMap::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i) {
     if (i->second.size() != 1) { ok = false; }
     else { 
      edgeTotal += i->first;
      //Good place to add extra points
      v.push_back( i->second.front() );
      edges.push_back(i->first); //add the edge number
     }
  }
  if (ok == false) {
     // We have some other configuration that needs to be reduced to 3 points on three edges first
     throw std::runtime_error("Error 293480987 - 3 Edges with Transition Point Size > 1 on an edge.");
  } 
  
  //Make a new Loop
  bbIndex = _getBoundBoxIndex(edgeTotal);
  osg::Vec3f cornerVert (_boundBox.corner(bbIndex) );
  
  /// Now we know which corner point to add, Add a new Shared Vertex to the Document
  Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( _caller );
  SharedVertex* sv ( addSharedVertex->addSharedVertex( cornerVert ) );  
 // std::cout << "Adding Vertex succeeded" << std::endl;
  /// Add some more points along the edge if necessary
  float length = Detail::avgSegmentLength(loop, _caller);
  
    // We now know the transition point indices and which edge they fall on.
    // We now need to create vectors of Shared Vertices to add to the edges
    // between the transition point and the corner point.
  std::vector<Points> theNewPoints;
<<<<<<< LoopSplitter.cpp
  for (unsigned int i = 0; i < v.size(); ++i) {
=======
  for ( unsigned int i = 0; i < v.size(); ++i )
  {
>>>>>>> 1.5
    std::vector<osg::Vec3> vertices;
<<<<<<< LoopSplitter.cpp
    _addNewPoints(edges[i], bbIndex, length, v[i], vertices);
   // std::cout << "New points Size: " << vertices.size() << std::endl;
=======
    _addNewPoints ( edges.at(i), bbIndex, length, v.at(i), vertices);
    std::cout << "New points Size: " << vertices.size() << std::endl;
>>>>>>> 1.5
    Points something;
<<<<<<< LoopSplitter.cpp
    for (unsigned int j = 0; j < vertices.size();j++) {
      //std::cout << vertices[j].x() << " , " << vertices[j].y() << " , " << vertices[j].z() << std::endl;
      SharedVertex *sv ( addSharedVertex->addSharedVertex( vertices[j] ) );  
      //std::cout << sv << std::endl;
=======
    for ( unsigned int j = 0; j < vertices.size(); ++j)
    {
      std::cout << vertices.at(j).x() << " , " << vertices.at(j).y() << " , " << vertices.at(j).z() << std::endl;
      SharedVertex *sv ( addSharedVertex->addSharedVertex( vertices.at(j) ) );  
      std::cout << sv << std::endl;
>>>>>>> 1.5
      something.push_back(sv);
    }
    theNewPoints.push_back(something);
  }
  
  /// Now lets make some loops
  /** Probably need a helper function.....  */
  TransitionPoints transitionPoints;
  // Place all transition points in a vector to test against.
  for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
    transitionPoints.push_back( iter->second );

  // Start and stop values for building the loop.
  int startIndex = 0;
  int stopIndex = 0;
  
//  std::cout << "Pairs that should be together:" << std::endl;
  for ( unsigned int i = 0; i < v.size();++i) 
  {
    Loop fixedLoop;
    
    TPointIndices::iterator it = tPointIndices.find ( v[i] );
    if ( tPointIndices.end() != it ) 
    {
      startIndex = it->second;
    }
    
    if (i == v.size()-1) {
      TPointIndices::iterator ite = tPointIndices.find ( v[0] );
      if ( tPointIndices.end() != ite ) 
      {
        stopIndex = ite->second;
      }
    } else {
      TPointIndices::iterator ite = tPointIndices.find ( v[i+1] );
      if ( tPointIndices.end() != ite ) 
      {
        stopIndex = ite->second;
      }
    }

   // std::cout << "[ " << startIndex << ", " << stopIndex << " ]" << std::endl;
    UsedIndices local;
    Points & refSomething ( theNewPoints[i] ); //get a reference to the vector of Points
    fixedLoop.push_back(sv); //Add the corner point to the loop

    if ( Detail::buildLoop( loop, fixedLoop, startIndex, stopIndex, transitionPoints, local ) )
    {
       loops.push_back(fixedLoop);
    }
    else
    {
      local.clear();
      fixedLoop.clear();
      fixedLoop.push_back(sv); //Add the corner point to the loop first
      // try it again with opposite starting points.
      if ( false == Detail::buildLoop( loop, fixedLoop, stopIndex, startIndex, transitionPoints, local ) )
        throw std::runtime_error ("Cannot split this loop." );
      loops.push_back( fixedLoop );
    }

  }  //End for loop 

}

///////////////////////////////////////////////////////////////////////////////
//
// This method handles 2 edges on a loop
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_doubleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{
    //std::cout << "----- _doubleEdge ----" << std::endl;
    UsedIndices usedIndices; //Store indices that need to be erased from the loop object
   /// This is where it will break. we need this in a loop
    for (EdgeMap::iterator edgeIter = edgeMap.begin(); edgeIter != edgeMap.end(); ++edgeIter)
    {
      std::vector<osg::Vec3f> & v = edgeIter->second;
      int edge = edgeIter->first;

      if (v.size()%2 == 0) 
      {      
        _edgeHelper(loop,edge,v,tPointIndices, loops, usedIndices);
       } else {
        //This is really bad.
        throw std::runtime_error("Error:2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
       }
      }
      //Erase the indices from a copy of the parent loop
      _parentLoopCopier(loop,loops, usedIndices);
 
}


///////////////////////////////////////////////////////////////////////////////
//
// This method handles the case where there is one edge involved in the loop
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_singleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{
  //std::cout << "------ _singleEdge --------" << std::endl;
  std::vector<osg::Vec3f> & v = edgeMap.begin()->second;
  int edge = edgeMap.begin()->first;

  UsedIndices usedIndices; //Store indices that need to be erased from the loop object
  if (v.size()%2 == 0) 
  {   

   /// Trying to move common code out to another method
    _edgeHelper(loop, edge, v ,tPointIndices, loops, usedIndices);
    
    //Erase the indices from a copy of the parent loop
    _parentLoopCopier(loop, loops, usedIndices);
    
  } else {
    //This is really bad.
    throw std::runtime_error("Error: 2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
  }

}
  
///////////////////////////////////////////////////////////////////////////////
//
// This method will copy from the parent loop just the indices that it needs 
//  to make a coplanar loop and then push that new loop onto the vector of
//  loops
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_parentLoopCopier(const OsgTools::Triangles::Loop & loop, Loops & loops, UsedIndices & usedIndices) 
{
  //std::cout << "------     _parentLoopCopier --------" << std::endl;
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

}


///////////////////////////////////////////////////////////////////////////////
//
// Helper method to actually split up some types of loops
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_edgeHelper(const OsgTools::Triangles::Loop& loop, const unsigned int edge, std::vector<osg::Vec3> & v, TPointIndices & tPointIndices, Loops & loops, UsedIndices & usedIndices  )
{
  //std::cout << "------     _edgeHelper --------" << std::endl;
  /// The number of transition points must be even
    if (v.size()%2 != 0) 
    {
      throw std::runtime_error("Error: 2039402350 - _edgeHelper() Non even number of Transition Points");
    }
    
    std::sort(v.begin(), v.end(), AxisSort( _getSortAxis(edge) ) );

    TransitionPoints transitionPoints;

    // Place all transition points in a vector to test against.
    for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
      transitionPoints.push_back( iter->second );

    // Start and stop values for building the loop.
    int startIndex = 0;
    int stopIndex = 0;
    
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
    }
}

/*
 *  LoopSplitter.cpp
 *  OsgTools
 *
 *  Created by Michael A Jackson on 9/1/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

/*

   *------------*
   |            |
   |            |
   |            






*/
#include "LoopSplitter.h"
#include "OsgTools/Triangles/Loop.h"
#include "Usul/Interfaces/IAddSharedVertex.h"
#include "Usul/Interfaces/IUnknown.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////




namespace Detail
{

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Function to build a co planer loop.  Returns false if it reaches a
  //  transition point that is incorrect.
  //
  ///////////////////////////////////////////////////////////////////////////////

  bool buildLoop ( const Loop& loop, Loop &fixedLoop, unsigned int start, unsigned int stop, const LoopSplitter::TransitionPoints &transitions, LoopSplitter::UsedIndices& used )
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
  
  float avgSegmentLength( const Loop& loop, Usul::Interfaces::IUnknown *_caller ) 
  {
    float l = 0.0f;
    unsigned int size = loop.size();
    for( unsigned int i = 0; i < size -1; ++i)
    {
     osg::Vec3 v1 ( loop.vertex( i, _caller ) );
     osg::Vec3 v2 ( loop.vertex( i+1, _caller ) );
     l += (v1-v2).length();
    }
    l = l/size;
    return l;
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
// Returns the corner point of the bounding box based on the total of the 
//  three edges
//
///////////////////////////////////////////////////////////////////////////////

int LoopSplitter::_getBoundBoxIndex(int edgeTotal) const 
{
  switch(edgeTotal) 
  {
      case CE_XMIN_YMIN_ZMIN: 
        return XMIN_YMIN_ZMIN;
      case CE_XMAX_YMIN_ZMIN: 
        return XMAX_YMIN_ZMIN;
      case CE_XMIN_YMAX_ZMIN: 
        return XMIN_YMAX_ZMIN;
      case CE_XMAX_YMAX_ZMIN: 
        return XMAX_YMAX_ZMIN;
      case CE_XMIN_YMIN_ZMAX: 
        return XMIN_YMIN_ZMAX;
      case CE_XMAX_YMIN_ZMAX: 
        return XMAX_YMIN_ZMAX;
      case CE_XMIN_YMAX_ZMAX: 
        return XMIN_YMAX_ZMAX;
      case CE_XMAX_YMAX_ZMAX:
        return XMAX_YMAX_ZMAX;
      default:
        throw std::runtime_error("Error 94630293930: _getCornerPoint() - Invalid Edge Total.");
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
// The main entry point for this class. This will split a parent loop into smaller
//  loops that are split on the edges of the bounding box.
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
// This will find the transition points where the loop moves from one plane 
//  to another.
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
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMIN].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.z() == zMin && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMAX ].push_back(v);
       tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.z() == zMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.z() == zMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.z() == zMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       }  else if (v.x() == xMax && v.z() == zMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
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
  
  /// There is only 1 Edge Involved in the loop
  if ( edgeMap.size() == 1 ) {
    _singleEdge(loop, edgeMap, tPointIndices, loops);
  }
  
  /// There are two edges involved in this loop.
  if ( edgeMap.size() == 2 ) {
    _doubleEdge(loop, edgeMap, tPointIndices, loops );
  }
  
  //If there are 3 edges involved and only 3 transition points, then we are
  // going to have to add the corner point into the mix as a transition point
  if ( edgeMap.size() == 3) {
    _tripleEdge(loop, edgeMap, tPointIndices, loops);
  }


}

///////////////////////////////////////////////////////////////////////////////
//
// This was experimental code to add addition points to the edges so the 
//  triangulation produces better triangles
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_addNewPoints(int edge, int corner, float length, osg::Vec3 & tp, std::vector<osg::Vec3> & verts) 
{
    /// Now we know which corner point to add, Add a new Shared Vertex to the Document
  Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( _caller );
  float min = -2.0f;
  float max = 0.0f;
  
  switch(edge) 
  {
      case ZMIN_YMIN:  //1
        if ( corner == XMIN_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).x();
          max = tp.x(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          }
        } else  if ( corner == XMAX_YMIN_ZMIN ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (max, tp.y(), tp.z());
            verts.push_back( v  );
            max -= length;
          }
        }
        break;
      case XMIN_YMIN:  //2
        if ( corner == XMIN_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          }    
        } else  if ( corner == XMIN_YMIN_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          } 
        }
        break;
      case ZMAX_YMIN: //3
        if ( corner == XMIN_YMIN_ZMAX ) {
          min = _boundBox.corner(corner).x();
          max = tp.x(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMIN_ZMAX ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
            while (min < max ) {
              osg::Vec3 v (max, tp.y(), tp.z());
              verts.push_back( v  );
              max -= length;
            }  
        }
    
        break;
      case XMAX_YMIN:  //4
        if ( corner == XMAX_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMIN_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          } 
        }
       
        break;
      case ZMIN_YMAX: //5
        if ( corner == XMIN_YMAX_ZMIN ) {
          min = _boundBox.corner(corner).x();
          max = tp.x();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          } 
        
        } else  if ( corner == XMAX_YMAX_ZMIN ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (max, tp.y(), tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
   
        break;
      case XMIN_YMAX:  //6
        if ( corner == XMIN_YMAX_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          }
        } else  if ( corner == XMIN_YMAX_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          }    
        }
      
        break;
      case ZMAX_YMAX:  //7
        if ( corner == XMIN_YMAX_ZMAX ) {
          min = _boundBox.corner(corner).x();
          max = tp.x();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (min, tp.y(), tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMAX_ZMAX ) {
          min = tp.x();
          max = _boundBox.corner(corner).x();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (max, tp.y(), tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
           
        break;
      case XMAX_YMAX:  //8
        if ( corner == XMAX_YMAX_ZMIN ) {
          min = _boundBox.corner(corner).z();
          max = tp.z();
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), min);
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMAX_YMAX_ZMAX ) {
          min = tp.z();
          max = _boundBox.corner(corner).z();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), tp.y(), max);
            verts.push_back( v  );
            max -= length;
          } 
        }
                
        break;
      case XMAX_ZMIN:  //9
        if ( corner == XMAX_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          }
        } else  if ( corner == XMAX_YMAX_ZMIN ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          }
        }
           
        break;      
      case XMIN_ZMIN: //10
        if ( corner == XMIN_YMIN_ZMIN ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMIN_YMAX_ZMIN ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
                 
        break;      
      case XMIN_ZMAX:  //11
        if ( corner == XMIN_YMIN_ZMAX ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          } 
        } else  if ( corner == XMIN_YMAX_ZMAX ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          } 
        }
            
        break;      
      case XMAX_ZMAX:  //12
        if ( corner == XMAX_YMIN_ZMAX ) {
          min = _boundBox.corner(corner).y();
          max = tp.y(); 
          min += length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), min, tp.z());
            verts.push_back( v  );
            min += length;
          }  
        } else  if ( corner == XMAX_YMAX_ZMAX ) {
          min = tp.y();
          max = _boundBox.corner(corner).y();
          max -= length; //do not want to start at the TransPoint
          while (min < max ) {
            osg::Vec3 v (tp.x(), max, tp.z());
            verts.push_back( v  );
            max -= length;
          }  
        }
           
        break;      
      default:
        throw std::runtime_error("Error 94630293940: _addNewPoints() Invalid Edge Value");
    }

}

///////////////////////////////////////////////////////////////////////////////
// 
// This method takes care of at least one case where the loop crosses 3 edges.
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_tripleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{
 // std::cout << "----- _tripleEdge -----" << std::endl;
  bool ok = true;
  int edgeTotal = 0;
  int bbIndex = -1;
  std::vector<osg::Vec3f> v; //Create a new Vector to hold the Vec3f
  std::vector<unsigned int> edges; //Create a Vector to hold the Edge Numbers
  
  //Now test the size of each vector to make sure it is one.
  for (EdgeMap::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i) {
     if (i->second.size() != 1) { ok = false; }
     else { 
      edgeTotal += i->first;
      //Good place to add extra points
      v.push_back( i->second.front() );
      edges.push_back(i->first); //add the edge number
     }
  }
  if (ok == false) {
     // We have some other configuration that needs to be reduced to 3 points on three edges first
     throw std::runtime_error("Error 293480987 - 3 Edges with Transition Point Size > 1 on an edge.");
  } 
  
  //Make a new Loop
  bbIndex = _getBoundBoxIndex(edgeTotal);
  osg::Vec3f cornerVert (_boundBox.corner(bbIndex) );
  
  /// Now we know which corner point to add, Add a new Shared Vertex to the Document
  Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( _caller );
  SharedVertex* sv ( addSharedVertex->addSharedVertex( cornerVert ) );  
 // std::cout << "Adding Vertex succeeded" << std::endl;
  /// Add some more points along the edge if necessary
  float length = Detail::avgSegmentLength(loop, _caller);
  
    // We now know the transition point indices and which edge they fall on.
    // We now need to create vectors of Shared Vertices to add to the edges
    // between the transition point and the corner point.
  std::vector<Points> theNewPoints;
  for (unsigned int i = 0; i < v.size(); ++i) {
    std::vector<osg::Vec3> vertices;
    _addNewPoints(edges[i], bbIndex, length, v[i], vertices);
   // std::cout << "New points Size: " << vertices.size() << std::endl;
    Points something;
    for (unsigned int j = 0; j < vertices.size();j++) {
      //std::cout << vertices[j].x() << " , " << vertices[j].y() << " , " << vertices[j].z() << std::endl;
      SharedVertex *sv ( addSharedVertex->addSharedVertex( vertices[j] ) );  
      //std::cout << sv << std::endl;
      something.push_back(sv);
    }
    theNewPoints.push_back(something);
  }
  
  /// Now lets make some loops
  /** Probably need a helper function.....  */
  TransitionPoints transitionPoints;
  // Place all transition points in a vector to test against.
  for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
    transitionPoints.push_back( iter->second );

  // Start and stop values for building the loop.
  int startIndex = 0;
  int stopIndex = 0;
  
//  std::cout << "Pairs that should be together:" << std::endl;
  for ( unsigned int i = 0; i < v.size();++i) 
  {
    Loop fixedLoop;
    
    TPointIndices::iterator it = tPointIndices.find ( v[i] );
    if ( tPointIndices.end() != it ) 
    {
      startIndex = it->second;
    }
    
    if (i == v.size()-1) {
      TPointIndices::iterator ite = tPointIndices.find ( v[0] );
      if ( tPointIndices.end() != ite ) 
      {
        stopIndex = ite->second;
      }
    } else {
      TPointIndices::iterator ite = tPointIndices.find ( v[i+1] );
      if ( tPointIndices.end() != ite ) 
      {
        stopIndex = ite->second;
      }
    }

   // std::cout << "[ " << startIndex << ", " << stopIndex << " ]" << std::endl;
    UsedIndices local;
    Points & refSomething ( theNewPoints[i] ); //get a reference to the vector of Points
    fixedLoop.push_back(sv); //Add the corner point to the loop

    if ( Detail::buildLoop( loop, fixedLoop, startIndex, stopIndex, transitionPoints, local ) )
    {
       loops.push_back(fixedLoop);
    }
    else
    {
      local.clear();
      fixedLoop.clear();
      fixedLoop.push_back(sv); //Add the corner point to the loop first
      // try it again with opposite starting points.
      if ( false == Detail::buildLoop( loop, fixedLoop, stopIndex, startIndex, transitionPoints, local ) )
        throw std::runtime_error ("Cannot split this loop." );
      loops.push_back( fixedLoop );
    }

  }  //End for loop 

}

///////////////////////////////////////////////////////////////////////////////
//
// This method handles 2 edges on a loop
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_doubleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{
    //std::cout << "----- _doubleEdge ----" << std::endl;
    UsedIndices usedIndices; //Store indices that need to be erased from the loop object
   /// This is where it will break. we need this in a loop
    for (EdgeMap::iterator edgeIter = edgeMap.begin(); edgeIter != edgeMap.end(); ++edgeIter)
    {
      std::vector<osg::Vec3f> & v = edgeIter->second;
      int edge = edgeIter->first;

      if (v.size()%2 == 0) 
      {      
        _edgeHelper(loop,edge,v,tPointIndices, loops, usedIndices);
       } else {
        //This is really bad.
        throw std::runtime_error("Error:2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
       }
      }
      //Erase the indices from a copy of the parent loop
      _parentLoopCopier(loop,loops, usedIndices);
 
}


///////////////////////////////////////////////////////////////////////////////
//
// This method handles the case where there is one edge involved in the loop
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_singleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{
  //std::cout << "------ _singleEdge --------" << std::endl;
  std::vector<osg::Vec3f> & v = edgeMap.begin()->second;
  int edge = edgeMap.begin()->first;

  UsedIndices usedIndices; //Store indices that need to be erased from the loop object
  if (v.size()%2 == 0) 
  {   

   /// Trying to move common code out to another method
    _edgeHelper(loop, edge, v ,tPointIndices, loops, usedIndices);
    
    //Erase the indices from a copy of the parent loop
    _parentLoopCopier(loop, loops, usedIndices);
    
  } else {
    //This is really bad.
    throw std::runtime_error("Error: 2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
  }

}
  
///////////////////////////////////////////////////////////////////////////////
//
// This method will copy from the parent loop just the indices that it needs 
//  to make a coplanar loop and then push that new loop onto the vector of
//  loops
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_parentLoopCopier(const OsgTools::Triangles::Loop & loop, Loops & loops, UsedIndices & usedIndices) 
{
  //std::cout << "------     _parentLoopCopier --------" << std::endl;
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

}


///////////////////////////////////////////////////////////////////////////////
//
// Helper method to actually split up some types of loops
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_edgeHelper(const OsgTools::Triangles::Loop& loop, const unsigned int edge, std::vector<osg::Vec3> & v, TPointIndices & tPointIndices, Loops & loops, UsedIndices & usedIndices  )
{
  //std::cout << "------     _edgeHelper --------" << std::endl;
  /// The number of transition points must be even
    if (v.size()%2 != 0) 
    {
      throw std::runtime_error("Error: 2039402350 - _edgeHelper() Non even number of Transition Points");
    }
    
    std::sort(v.begin(), v.end(), AxisSort( _getSortAxis(edge) ) );

    TransitionPoints transitionPoints;

    // Place all transition points in a vector to test against.
    for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
      transitionPoints.push_back( iter->second );

    // Start and stop values for building the loop.
    int startIndex = 0;
    int stopIndex = 0;
    
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
    }
}

