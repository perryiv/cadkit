
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This work is adapted from http://www.cs.unc.edu/~dm/CODE/GEM/chapter.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_TRIANGULATE_H__
#define __USUL_ALGORITHMS_TRIANGULATE_H__

#include <ctime>
#include <stdexcept>
#include <vector>
#include <limits>
#include <algorithm>

namespace Usul {
namespace Loops {

namespace Detail {

//TODO Is a tolerance value needed for making all decisions about collinearity ?


/* Return the maximum of the two points into the yval structure */
template < class Vertex >
inline void getMaximumVertex ( Vertex &yval, Vertex &v0, Vertex &v1 )
{
  if (v0.y() > v1.y() )
    yval = v0;
  else if ( v0.y() == v1.y() )
  {
    if (v0.x() > v1.x() )
	    yval = v0;
    else
	    yval = v1;
  }
  else
    yval = v1;
}


/* Return the minimum of the two points into the yval structure */
template < class Vertex >
inline void getMinimumVertex ( Vertex &yval, Vertex &v0, Vertex &v1 )
{
  if (v0.y() < v1.y() )
    yval = v0;
  else if ( v0.y() == v1.y() )
  {
    if ( v0.x() < v1.x() )
	    yval = v0;
    else
	    yval = v1;
  }
  else
    yval = v1;

}

template < class Vertex >
inline bool greaterThan( const Vertex &v0, const Vertex &v1 )
{
  if (v0.y() > v1.y() )
    return true;
  else if (v0.y() < v1.y() )
    return false;
  else
    return ( v0.x() > v1.x() );
}

template < class Vertex >
inline bool greaterThanEqualTo( const Vertex & v0, const Vertex & v1 )
{
  if (v0.y() > v1.y() )
    return true;
  else if (v0.y() < v1.y() )
    return false;
  else
    return ( v0.x() >= v1.x() );
}

template < class Vertex >
inline bool lessThan( const Vertex & v0, const Vertex & v1 )
{
  if (v0.y() < v1.y() )
    return true;
  else if (v0.y() > v1.y() )
    return false;
  else
    return ( v0.x() < v1.x() );
}



struct Point
{
  Point() { }

  double length () const { return ::sqrt( ( _x * _x ) + ( _y * _y ) ); }

  void set ( double x, double y ) { _x = x; _y = y; }

  double x() const { return _x; }
  double y() const { return _y; }

  double & x() { return _x; }
  double & y() { return _y; }

  bool operator==( const Point& that )
  {
    return ( this->_y == that._y  && this->_x == that._x );
  }

private:
  double _x, _y;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Segment attributes 
//
///////////////////////////////////////////////////////////////////////////////

template < class Vertex >
struct Segment_
{	
  Vertex v0, v1;		/* two endpoints */
  bool is_inserted;		/* inserted in trapezoidation yet ? */
  int root0, root1;		/* root nodes in Q */
  int next;			/* Next logical segment */
  int prev;			/* Previous segment */

  Segment_() :
  v0(),
  v1(),
  is_inserted ( false ),
  root0 ( 0 ),
  root1 ( 0 ),
  next ( 0 ),
  prev ( 0 )
  {
  }
};

//For now
typedef Segment_< Point > Segment;


///////////////////////////////////////////////////////////////////////////////
//
//  Trapezoid attributes 
//
///////////////////////////////////////////////////////////////////////////////

struct Trapezoid
{
  int lseg, rseg;		/* two adjoining segments */
  Point hi, lo;		/* max/min y-values */
  int u0, u1;
  int d0, d1;
  int sink;			/* pointer to corresponding in Q */
  int usave, uside;		/* I forgot what this means */

  Trapezoid() :
  lseg ( -1 ),
  rseg ( -1 ),
  hi(),
  lo(),
  u0 ( 0 ),
  u1 ( 0 ),
  d0 ( 0 ),
  d1 ( 0 ),
  sink ( 0 ),
  usave ( 0 ),
  uside ( 0 ),
  _valid ( true ) // TODO Not sure if valid should be initialized to true or false
  {
  }

  bool valid() const { return _valid; }
  void valid ( bool v ) { _valid = v; }

private:
  bool _valid;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Node attributes for every node in the query structure
//
///////////////////////////////////////////////////////////////////////////////

struct Node 
{
  enum NodeType
  {
    UNKNOWN,
    T_X,
    T_Y,
    T_SINK
  };


  NodeType nodetype;			/* Y-node or S-node */
  int segnum;
  Point yval;
  int trnum;
  int parent;			/* doubly linked DAG */
  int left, right;		/* children */

  Node() :
  nodetype ( UNKNOWN ),
  segnum (),
  yval (),
  trnum ( 0 ),
  parent ( 0 ),
  left ( 0 ), 
  right ( 0 )
  {
  }
};


struct Monchain
{
  int vnum;
  int next;			/* Circularly linked list  */
  int prev;			/* describing the monotone */
  bool marked;			/* polygon */

  Monchain () :
  vnum ( 0 ),
  next ( 0 ),
  prev ( 0 ),
  marked ( false )
  {
  }
};			


struct Vertexchain
{
  Point pt;
  int vnext[4];			/* next vertices for the 4 chains */
  int vpos[4];			/* position of v in the 4 chains */
  int nextfree;

  Vertexchain() :
  pt(),
  nextfree( 0 )
  {
    vnext[0] = vnext[1] = vnext[2] = vnext[3] = 0;
    vpos[0] = vpos[1] = vpos[2] = vpos[3] = 0;
  }
};


#define FIRSTPT 1		/* checking whether pt. is inserted */ 
#define LASTPT  2


#define S_LEFT 1		/* for merge-direction */
#define S_RIGHT 2


#define SP_SIMPLE_LRUP 1	/* for splitting trapezoids */
#define SP_SIMPLE_LRDN 2
#define SP_2UP_2DN     3
#define SP_2UP_LEFT    4
#define SP_2UP_RIGHT   5
#define SP_2DN_LEFT    6
#define SP_2DN_RIGHT   7
#define SP_NOSPLIT    -1	

#define TR_FROM_UP 1		/* for traverse-direction */
#define TR_FROM_DN 2

#define TRI_LHS 1
#define TRI_RHS 2


#define CROSS(v0, v1, v2) (((v1).x() - (v0).x() )*((v2).y() - (v0).y() ) - \
			   ((v1).y() - (v0).y() )*((v2).x() - (v0).x()))

#define DOT(v0, v1) ((v0).x() * (v1).x() + (v0).y() * (v1).y() )



double log2( double n )
{
  return log( n ) / log ( (double) 2 );
}




/* Get log*n for given n */
int math_logstar_n( int n )
{
  int i ( 0 );
  double v ( n );
  
  for ( ; v >= 1; i++)
    v = log2(v);
  
  return (i - 1);
}
  

int math_N(int n, int h)
{
  double v ( n );

  for ( int i = 0; i < h; i++)
    v = log2(v);
  
  return (int) ceil((double) 1.0*n/v);
}


  
static int q_idx = 0;
static int tr_idx = 0;

/* Return a new node to be added into the query tree */
int newnode()
{
  return q_idx++;
}

/* Return a free trapezoid */
int newtrap( std::vector< Trapezoid > &tr )
{
  tr.at ( tr_idx ).lseg = -1;
  tr.at ( tr_idx ).rseg = -1;
  tr.at ( tr_idx ).valid( true );
  return tr_idx++;

}


/* Initilialise the query structure (Q) and the trapezoid table (T) 
 * when the first segment is added to start the trapezoidation. The
 * query-tree starts out with 4 trapezoids, one S-node and 2 Y-nodes
 *    
 *                4
 *   -----------------------------------
 *  		  \
 *  	1	   \        2
 *  		    \
 *   -----------------------------------
 *                3
 */

int init_query_structure( int segnum, std::vector < Node > &qs, std::vector < Trapezoid > &tr, std::vector < Segment > &seg )
{
  int i1, i2, i3, i4, i5, i6, i7, root;
  int t1, t2, t3, t4;
  Segment *s = &seg.at(segnum);

  //3186504610
  q_idx = 1;
  tr_idx = 1;

  i1 = newnode();
  qs.at ( i1 ).nodetype = Node::T_Y;
  getMaximumVertex ( qs.at(i1).yval, s->v0, s->v1 ); /* root */
  root = i1;

  qs.at(i1).right = i2 = newnode();
  qs.at(i2).nodetype = Node::T_SINK;
  qs.at(i2).parent = i1;

  qs.at(i1).left = i3 = newnode();
  qs.at(i3).nodetype = Node::T_Y;
  getMinimumVertex ( qs.at(i3).yval, s->v0, s->v1 ); /* root */
  qs.at(i3).parent = i1;
  
  qs.at(i3).left = i4 = newnode();
  qs.at(i4).nodetype = Node::T_SINK;
  qs.at(i4).parent = i3;
  
  qs.at(i3).right = i5 = newnode();
  qs.at(i5).nodetype = Node::T_X;
  qs.at(i5).segnum = segnum;
  qs.at(i5).parent = i3;
  
  qs.at(i5).left = i6 = newnode();
  qs.at(i6).nodetype = Node::T_SINK;
  qs.at(i6).parent = i5;

  qs.at(i5).right = i7 = newnode();
  qs.at(i7).nodetype = Node::T_SINK;
  qs.at(i7).parent = i5;

  t1 = newtrap( tr );		/* middle left */
  t2 = newtrap( tr );		/* middle right */
  t3 = newtrap( tr );		/* bottom-most */
  t4 = newtrap( tr );		/* topmost */

  tr.at(t1).hi = tr.at(t2).hi = tr.at(t4).lo = qs.at(i1).yval;
  tr.at(t1).lo = tr.at(t2).lo = tr.at(t3).hi = qs.at(i3).yval;
  tr.at(t4).hi.y() = std::numeric_limits<double>::infinity();
  tr.at(t4).hi.x() = std::numeric_limits<double>::infinity();
  tr.at(t3).lo.y() = -1 * std::numeric_limits<double>::infinity();
  tr.at(t3).lo.x() = -1 * std::numeric_limits<double>::infinity();
  tr.at(t1).rseg = tr.at(t2).lseg = segnum;
  tr.at(t1).u0 = tr.at(t2).u0 = t4;
  tr.at(t1).d0 = tr.at(t2).d0 = t3;
  tr.at(t4).d0 = tr.at(t3).u0 = t1;
  tr.at(t4).d1 = tr.at(t3).u1 = t2;
  
  tr.at(t1).sink = i6;
  tr.at(t2).sink = i7;
  tr.at(t3).sink = i4;
  tr.at(t4).sink = i2;

  tr.at(t1).valid( true );
  tr.at(t2).valid( true );
  tr.at(t3).valid( true );
  tr.at(t4).valid( true );

  qs.at(i2).trnum = t4;
  qs.at(i4).trnum = t3;
  qs.at(i6).trnum = t1;
  qs.at(i7).trnum = t2;

  s->is_inserted = true;
  return root;
}


/* Retun TRUE if the vertex v is to the left of line segment no.
 * segnum. Takes care of the degenerate cases when both the vertices
 * have the same y--cood, etc.
 */

bool is_left_of( int segnum, const Point &v, std::vector< Segment > &seg )
{
  Segment *s = &seg.at( segnum );
  double area;
  
  if ( greaterThan( s->v1, s->v0) ) /* seg. going upwards */
  {
    if ( s->v1.y() == v.y() )
	  {
	    if ( v.x() < s->v1.x() )
	      area = 1.0;
	    else
	      area = -1.0;
	  }
    else if ( s->v0.y() == v.y() )
	  {
	    if (v.x() < s->v0.x() )
	      area = 1.0;
	    else
	      area = -1.0;
	  }
    else
	    area = CROSS( s->v0, s->v1, v );
  }
  else				/* v0 > v1 */
  {
    if (  s->v1.y() == v.y() )
	  {
	    if (v.x() < s->v1.x() )
	      area = 1.0;
	    else
	      area = -1.0;
	  }
    else if ( s->v0.y() == v.y() )
	  {
	    if (v.x() < s->v0.x() )
	      area = 1.0;
	    else
	      area = -1.0;
	  }
    else
	    area = CROSS( s->v1, s->v0, v );
  }
  
  if (area > 0.0)
    return true;
  else 
    return false;
}



/* Returns true if the corresponding endpoint of the given segment is */
/* already inserted into the segment tree. Use the simple test of */
/* whether the segment which shares this endpoint is already inserted */

bool inserted( int segnum, int whichpt, std::vector< Segment > &seg )
{
  if (whichpt == FIRSTPT)
    return seg.at( seg.at(segnum).prev ).is_inserted;
  else
    return seg.at( seg.at(segnum).next ).is_inserted;
}

/* This is query routine which determines which trapezoid does the 
 * point v lie in. The return value is the trapezoid number. 
 */

int locate_endpoint( Point &v, Point &vo, int r, std::vector< Node > &qs, std::vector< Segment >& seg )
{
  Node *rptr = &qs.at(r);
  
  switch ( rptr->nodetype )
  {
  case Node::T_SINK:
    return rptr->trnum;
      
  case Node::T_Y:
    {
      if ( greaterThan( v, rptr->yval ) ) /* above */
	      return locate_endpoint(v, vo, rptr->right, qs, seg );
      else if ( v == rptr->yval ) /* the point is already */
	    {			          /* inserted. */
	      if ( greaterThan( vo, rptr->yval)) /* above */
	        return locate_endpoint(v, vo, rptr->right, qs, seg );
	      else 
	        return locate_endpoint(v, vo, rptr->left, qs, seg ); /* below */	    
	    }
      else
	      return locate_endpoint(v, vo, rptr->left, qs, seg ); /* below */
    }
  case Node::T_X:
      if ( v == seg.at( rptr->segnum ).v0 || v == seg.at( rptr->segnum ).v1 )
	    {
	      if ( v.y() == vo.y() ) /* horizontal segment */
	      {
	        if (vo.x() < v.x() )
		        return locate_endpoint(v, vo, rptr->left, qs, seg ); /* left */
	        else
		        return locate_endpoint(v, vo, rptr->right, qs, seg ); /* right */
	      }
	      else if (is_left_of( rptr->segnum, vo, seg ))
	        return locate_endpoint(v, vo, rptr->left, qs, seg ); /* left */
	      else
	        return locate_endpoint(v, vo, rptr->right, qs, seg ); /* right */
	    }
      else if (is_left_of(rptr->segnum, v, seg ))
	      return locate_endpoint(v, vo, rptr->left, qs, seg ); /* left */
      else
	      return locate_endpoint(v, vo, rptr->right, qs, seg ); /* right */	

    default:
      throw std::runtime_error ("Error 3397215586: Should not be here." );
      break;
    }
  return 0;
}


/* Thread in the segment into the existing trapezoidation. The 
 * limiting trapezoids are given by tfirst and tlast (which are the
 * trapezoids containing the two endpoints of the segment. Merges all
 * possible trapezoids which flank this segment and have been recently
 * divided because of its insertion
 */

void merge_trapezoids( int segnum, int tfirst, int tlast, int side, std::vector< Node > &qs, std::vector< Trapezoid> &tr  )
{
  int t = 0, tnext = 0, cond = 0;
  int ptnext = 0;

  /* First merge polys on the LHS */
  t = tfirst;
  while ((t > 0) && greaterThanEqualTo( tr.at(t).lo, tr.at(tlast).lo))
  {
    if (side == S_LEFT)
	    cond = ((((tnext = tr.at(t).d0) > 0) && (tr.at(tnext).rseg == segnum)) || (((tnext = tr.at(t).d1) > 0) && (tr.at(tnext).rseg == segnum)));
    else
	    cond = ((((tnext = tr.at(t).d0) > 0) && (tr.at(tnext).lseg == segnum)) || (((tnext = tr.at(t).d1) > 0) && (tr.at(tnext).lseg == segnum)));
      
    if (cond)
	  {
	    if ((tr.at(t).lseg == tr.at(tnext).lseg) && (tr.at(t).rseg == tr.at(tnext).rseg)) /* good neighbours */
	    {			              /* merge them */
	      /* Use the upper node as the new node i.e. t */
	      
	      ptnext = qs.at(tr.at(tnext).sink).parent;
	      
	      if (qs.at(ptnext).left == tr.at(tnext).sink)
		      qs.at(ptnext).left = tr.at(t).sink;
	      else
		      qs.at(ptnext).right = tr.at(t).sink;	/* redirect parent */
	      
	      
	      /* Change the upper neighbours of the lower trapezoids */
	      
	      if ((tr.at(t).d0 = tr.at(tnext).d0) > 0)
        {
		      if (tr.at(tr.at(t).d0).u0 == tnext)
		        tr.at(tr.at(t).d0).u0 = t;
		      else if (tr.at(tr.at(t).d0).u1 == tnext)
		        tr.at(tr.at(t).d0).u1 = t;
        }
	      
	      if ((tr.at(t).d1 = tr.at(tnext).d1) > 0)
        {
		      if (tr.at(tr.at(t).d1).u0 == tnext)
		        tr.at(tr.at(t).d1).u0 = t;
		      else if (tr.at(tr.at(t).d1).u1 == tnext)
		        tr.at(tr.at(t).d1).u1 = t;
        }
	      
	      tr.at(t).lo = tr.at(tnext).lo;
        tr.at(tnext).valid( false ); /* invalidate the lower */
				            /* trapezium */
	    }
	    else		    /* not good neighbours */
	      t = tnext;
	  }
    else		    /* do not satisfy the outer if */
	    t = tnext;
      
  } /* end-while */
       
}



  
/* Add in the new segment into the trapezoidation and update Q and T
 * structures. First locate the two endpoints of the segment in the
 * Q-structure. Then start from the topmost trapezoid and go down to
 * the  lower trapezoid dividing all the trapezoids in between .
 */

int add_segment( int segnum, std::vector < Node > &qs, std::vector < Trapezoid > &tr, std::vector < Segment > &seg  )
{
  Segment *so = &seg.at(segnum);
  int tu, tl, sk, tfirst, tlast;
  int tfirstr = 0, tlastr = 0, tfirstl = 0, tlastl = 0;
  int i1, i2, t, tn;
  Point tpt;
  int tritop = 0, tribot = 0; 
  bool is_swapped ( false );
  int tmptriseg;

  Segment s ( seg.at(segnum) );

  if ( greaterThan( s.v1, s.v0) ) /* Get higher vertex in v0 */
  {
    tpt = s.v0;
    s.v0 = s.v1;
    s.v1 = tpt;
    int tmp ( s.root0 );
    s.root0 = s.root1;
    s.root1 = tmp;
    is_swapped = true;
  }

  if ((is_swapped) ? !inserted(segnum, LASTPT, seg) : !inserted(segnum, FIRSTPT, seg))     /* insert v0 in the tree */
  {
    int tmp_d;

    tu = locate_endpoint( s.v0, s.v1, s.root0, qs, seg );
    tl = newtrap( tr );		/* tl is the new lower trapezoid */
    tr.at(tl).valid( true );
    tr.at(tl) = tr.at(tu);
    tr.at(tu).lo.y() = tr.at(tl).hi.y() = s.v0.y();
    tr.at(tu).lo.x() = tr.at(tl).hi.x() = s.v0.x();
    tr.at(tu).d0 = tl;      
    tr.at(tu).d1 = 0;
    tr.at(tl).u0 = tu;
    tr.at(tl).u1 = 0;

    if (((tmp_d = tr.at(tl).d0) > 0) && (tr.at(tmp_d).u0 == tu))
      tr.at(tmp_d).u0 = tl;
    if (((tmp_d = tr.at(tl).d0) > 0) && (tr.at(tmp_d).u1 == tu))
      tr.at(tmp_d).u1 = tl;

    if (((tmp_d = tr.at(tl).d1) > 0) && (tr.at(tmp_d).u0 == tu))
      tr.at(tmp_d).u0 = tl;
    if (((tmp_d = tr.at(tl).d1) > 0) && (tr.at(tmp_d).u1 == tu))
      tr.at(tmp_d).u1 = tl;

      /* Now update the query structure and obtain the sinks for the */
      /* two trapezoids */ 
      
    i1 = newnode();		/* Upper trapezoid sink */
    i2 = newnode();		/* Lower trapezoid sink */
    sk = tr.at(tu).sink;
    
    qs.at ( sk ).nodetype = Node::T_Y;
    qs.at ( sk ).yval = s.v0;
    qs.at ( sk ).segnum = segnum;	/* not really reqd ... maybe later */
    qs.at ( sk ).left = i2;
    qs.at ( sk ).right = i1;

    qs.at ( i1 ).nodetype = Node::T_SINK;
    qs.at ( i1 ).trnum = tu;
    qs.at ( i1 ).parent = sk;

    qs.at ( i2 ).nodetype = Node::T_SINK;
    qs.at ( i2 ).trnum = tl;
    qs.at ( i2 ).parent = sk;

    tr.at(tu).sink = i1;
    tr.at(tl).sink = i2;
    tfirst = tl;
  }
  else				/* v0 already present */
  {       /* Get the topmost intersecting trapezoid */
    tfirst = locate_endpoint( s.v0, s.v1, s.root0, qs, seg );
    tritop = 1;
  }


  if ((is_swapped) ? !inserted(segnum, FIRSTPT, seg ) :!inserted(segnum, LASTPT, seg ))     /* insert v1 in the tree */
  {
    int tmp_d;

    tu = locate_endpoint( s.v1, s.v0, s.root1, qs, seg );

    tl = newtrap( tr );		/* tl is the new lower trapezoid */
    tr.at(tl).valid( true );
    tr.at(tl) = tr.at(tu);
    tr.at(tu).lo.y() = tr.at(tl).hi.y() = s.v1.y();
    tr.at(tu).lo.x() = tr.at(tl).hi.x() = s.v1.x();
    tr.at(tu).d0 = tl;      
    tr.at(tu).d1 = 0;
    tr.at(tl).u0 = tu;
    tr.at(tl).u1 = 0;

    if (((tmp_d = tr.at(tl).d0) > 0) && (tr.at(tmp_d).u0 == tu))
	    tr.at(tmp_d).u0 = tl;
    if (((tmp_d = tr.at(tl).d0) > 0) && (tr.at(tmp_d).u1 == tu))
	    tr.at(tmp_d).u1 = tl;

    if (((tmp_d = tr.at(tl).d1) > 0) && (tr.at(tmp_d).u0 == tu))
	    tr.at(tmp_d).u0 = tl;
    if (((tmp_d = tr.at(tl).d1) > 0) && (tr.at(tmp_d).u1 == tu))
	    tr.at(tmp_d).u1 = tl;
    
    /* Now update the query structure and obtain the sinks for the */
    /* two trapezoids */ 
    
    i1 = newnode();		/* Upper trapezoid sink */
    i2 = newnode();		/* Lower trapezoid sink */
    sk = tr.at(tu).sink;
    
    qs.at ( sk ).nodetype = Node::T_Y;
    qs.at ( sk ).yval = s.v1;
    qs.at ( sk ).segnum = segnum;	/* not really reqd ... maybe later */
    qs.at ( sk ).left = i2;
    qs.at ( sk ).right = i1;

    qs.at ( i1 ).nodetype = Node::T_SINK;
    qs.at ( i1 ).trnum = tu;
    qs.at ( i1 ).parent = sk;

    qs.at ( i2 ).nodetype = Node::T_SINK;
    qs.at ( i2 ).trnum = tl;
    qs.at ( i2 ).parent = sk;

    tr.at(tu).sink = i1;
    tr.at(tl).sink = i2;
    tlast = tu;
  }
else				/* v1 already present */
    {       /* Get the lowermost intersecting trapezoid */
      tlast = locate_endpoint( s.v1, s.v0, s.root1, qs, seg );
      tribot = 1;
    }
  
  /* Thread the segment into the query tree creating a new X-node */
  /* First, split all the trapezoids which are intersected by s into */
  /* two */

  t = tfirst;			/* topmost trapezoid */
  
  while ((t > 0) && greaterThanEqualTo( tr.at( t ).lo, tr.at( tlast ).lo))
				/* traverse from top to bot */
    {
      int t_sav, tn_sav;
      sk = tr.at( t ).sink;
      i1 = newnode();		/* left trapezoid sink */
      i2 = newnode();		/* right trapezoid sink */
      
      qs.at ( sk ).nodetype = Node::T_X;
      qs.at ( sk ).segnum = segnum;
      qs.at ( sk ).left = i1;
      qs.at ( sk ).right = i2;

      qs.at ( i1 ).nodetype = Node::T_SINK;	/* left trapezoid (use existing one) */
      qs.at ( i1 ).trnum = t;
      qs.at ( i1 ).parent = sk;

      qs.at ( i2 ).nodetype = Node::T_SINK;	/* right trapezoid (allocate new) */
      qs.at ( i2 ).trnum = tn = newtrap( tr );
      tr.at( tn ).valid( true );
      qs.at ( i2 ).parent = sk;

      if (t == tfirst)
	      tfirstr = tn;
      if ( tr.at( t ).lo == tr.at ( tlast ).lo)
	      tlastr = tn;

      tr.at( tn ) = tr.at( t );
      tr.at(t).sink = i1;
      tr.at(tn).sink = i2;
      t_sav = t;
      tn_sav = tn;

      /* error */

      if ((tr.at(t).d0 <= 0) && (tr.at(t).d1 <= 0)) /* case cannot arise */
	{
    throw std::runtime_error ( "add_segment: error\n");
	  break;
	}
      
      /* only one trapezoid below. partition t into two and make the */
      /* two resulting trapezoids t and tn as the upper neighbours of */
      /* the sole lower trapezoid */
      
      else if ((tr[t].d0 > 0) && (tr[t].d1 <= 0))
	{			/* Only one trapezoid below */
	  if ((tr[t].u0 > 0) && (tr[t].u1 > 0))
	    {			/* continuation of a chain from abv. */
	      if (tr[t].usave > 0) /* three upper neighbours */
		{
		  if (tr[t].uside == S_LEFT)
		    {
		      tr[tn].u0 = tr[t].u1;
		      tr[t].u1 = -1;
		      tr[tn].u1 = tr[t].usave;
		      
		      tr[tr[t].u0].d0 = t;
		      tr[tr[tn].u0].d0 = tn;
		      tr[tr[tn].u1].d0 = tn;
		    }
		  else		/* intersects in the right */
		    {
		      tr[tn].u1 = -1;
		      tr[tn].u0 = tr[t].u1;
		      tr[t].u1 = tr[t].u0;
		      tr[t].u0 = tr[t].usave;

		      tr[tr[t].u0].d0 = t;
		      tr[tr[t].u1].d0 = t;
		      tr[tr[tn].u0].d0 = tn;		      
		    }
		  
		  tr[t].usave = tr[tn].usave = 0;
		}
	      else		/* No usave.... simple case */
		{
		  tr[tn].u0 = tr[t].u1;
		  tr[t].u1 = tr[tn].u1 = -1;
		  tr[tr[tn].u0].d0 = tn;
		}
	    }
	  else 
	    {			/* fresh seg. or upward cusp */
	      int tmp_u = tr[t].u0;
	      int td0, td1;
	      if (((td0 = tr[tmp_u].d0) > 0) && 
		  ((td1 = tr[tmp_u].d1) > 0))
		{		/* upward cusp */
		  if ((tr[td0].rseg > 0) &&
		      !is_left_of(tr[td0].rseg, s.v1, seg ))
		    {
		      tr[t].u0 = tr[t].u1 = tr[tn].u1 = -1;
		      tr[tr[tn].u0].d1 = tn;
		    }
		  else		/* cusp going leftwards */
		    { 
		      tr[tn].u0 = tr[tn].u1 = tr[t].u1 = -1;
		      tr[tr[t].u0].d0 = t;
		    }
		}
	      else		/* fresh segment */
		{
		  tr[tr[t].u0].d0 = t;
		  tr[tr[t].u0].d1 = tn;
		}	      
	    }
	  
	  if ( ( tr[t].lo.y() == tr[tlast].lo.y() && tr[t].lo.x() == tr[tlast].lo.x() ) && tribot)
	    {		/* bottom forms a triangle */

	      if (is_swapped)	
		tmptriseg = seg.at(segnum).prev;
	      else
		tmptriseg = seg.at(segnum).next;
	      
	      if ((tmptriseg > 0) && is_left_of(tmptriseg, s.v0, seg ))
		{
				/* L-R downward cusp */
		  tr[tr[t].d0].u0 = t;
		  tr[tn].d0 = tr[tn].d1 = -1;
		}
	      else
		{
				/* R-L downward cusp */
		  tr[tr[tn].d0].u1 = tn;
		  tr[t].d0 = tr[t].d1 = -1;
		}
	    }
	  else
	    {
	      if ((tr[tr[t].d0].u0 > 0) && (tr[tr[t].d0].u1 > 0))
		{
		  if (tr[tr[t].d0].u0 == t) /* passes thru LHS */
		    {
		      tr[tr[t].d0].usave = tr[tr[t].d0].u1;
		      tr[tr[t].d0].uside = S_LEFT;
		    }
		  else
		    {
		      tr[tr[t].d0].usave = tr[tr[t].d0].u0;
		      tr[tr[t].d0].uside = S_RIGHT;
		    }		    
		}
	      tr[tr[t].d0].u0 = t;
	      tr[tr[t].d0].u1 = tn;
	    }
	  
	  t = tr[t].d0;
	}


      else if ((tr[t].d0 <= 0) && (tr[t].d1 > 0))
	{			/* Only one trapezoid below */
	  if ((tr[t].u0 > 0) && (tr[t].u1 > 0))
	    {			/* continuation of a chain from abv. */
	      if (tr[t].usave > 0) /* three upper neighbours */
		{
		  if (tr[t].uside == S_LEFT)
		    {
		      tr[tn].u0 = tr[t].u1;
		      tr[t].u1 = -1;
		      tr[tn].u1 = tr[t].usave;
		      
		      tr[tr[t].u0].d0 = t;
		      tr[tr[tn].u0].d0 = tn;
		      tr[tr[tn].u1].d0 = tn;
		    }
		  else		/* intersects in the right */
		    {
		      tr[tn].u1 = -1;
		      tr[tn].u0 = tr[t].u1;
		      tr[t].u1 = tr[t].u0;
		      tr[t].u0 = tr[t].usave;

		      tr[tr[t].u0].d0 = t;
		      tr[tr[t].u1].d0 = t;
		      tr[tr[tn].u0].d0 = tn;		      
		    }
		  
		  tr[t].usave = tr[tn].usave = 0;
		}
	      else		/* No usave.... simple case */
		{
		  tr[tn].u0 = tr[t].u1;
		  tr[t].u1 = tr[tn].u1 = -1;
		  tr[tr[tn].u0].d0 = tn;
		}
	    }
	  else 
	    {			/* fresh seg. or upward cusp */
	      int tmp_u = tr[t].u0;
	      int td0, td1;
	      if (((td0 = tr[tmp_u].d0) > 0) && 
		  ((td1 = tr[tmp_u].d1) > 0))
		{		/* upward cusp */
		  if ((tr[td0].rseg > 0) && !is_left_of(tr[td0].rseg, s.v1, seg))
		    {
		      tr[t].u0 = tr[t].u1 = tr[tn].u1 = -1;
		      tr[tr[tn].u0].d1 = tn;
		    }
		  else 
		    {
		      tr[tn].u0 = tr[tn].u1 = tr[t].u1 = -1;
		      tr[tr[t].u0].d0 = t;
		    }
		}
	      else		/* fresh segment */
		{
		  tr[tr[t].u0].d0 = t;
		  tr[tr[t].u0].d1 = tn;
		}
	    }
	  
	  if ( (tr[t].lo.y() == tr[tlast].lo.y()) && (tr[t].lo.x() == tr[tlast].lo.x()) && tribot)
	    {		/* bottom forms a triangle */
	      int tmpseg = 0;

	      if (is_swapped)	
		tmptriseg = seg.at(segnum).prev;
	      else
		tmptriseg = seg.at(segnum).next;

	  if ((tmpseg > 0) && is_left_of(tmpseg, s.v0, seg ))
		{
		  /* L-R downward cusp */
		  tr[tr[t].d1].u0 = t;
		  tr[tn].d0 = tr[tn].d1 = -1;
		}
	      else
		{
		  /* R-L downward cusp */
		  tr[tr[tn].d1].u1 = tn;
		  tr[t].d0 = tr[t].d1 = -1;
		}
	    }		
	  else
	    {
	      if ((tr[tr[t].d1].u0 > 0) && (tr[tr[t].d1].u1 > 0))
		{
		  if (tr[tr[t].d1].u0 == t) /* passes thru LHS */
		    {
		      tr[tr[t].d1].usave = tr[tr[t].d1].u1;
		      tr[tr[t].d1].uside = S_LEFT;
		    }
		  else
		    {
		      tr[tr[t].d1].usave = tr[tr[t].d1].u0;
		      tr[tr[t].d1].uside = S_RIGHT;
		    }		    
		}
	      tr[tr[t].d1].u0 = t;
	      tr[tr[t].d1].u1 = tn;
	    }
	  
	  t = tr[t].d1;
	}

      /* two trapezoids below. Find out which one is intersected by */
      /* this segment and proceed down that one */
      
      else
	{
	  int tmpseg = tr[tr[t].d0].rseg;
	  double y0, yt;
	  Point tmppt;
	  int tnext;
    bool i_d0 ( false );
    bool i_d1 ( false );

	  if ( tr[t].lo.y() == s.v0.y() )
	  {
	    if (tr[t].lo.x() > s.v0.x() )
	      i_d0 = true;
	    else
	      i_d1 = true;
	  }
	  else
	  {
	    tmppt.y() = y0 = tr[t].lo.y();
	    yt = (y0 - s.v0.y())/(s.v1.y() - s.v0.y());
	    tmppt.x() = s.v0.x() + yt * (s.v1.x() - s.v0.x());
	    
	    if ( lessThan( tmppt, tr[t].lo))
		    i_d0 = true;
	    else
		    i_d1 = true;
	  }
	  
	  /* check continuity from the top so that the lower-neighbour */
	  /* values are properly filled for the upper trapezoid */

	  if ((tr[t].u0 > 0) && (tr[t].u1 > 0))
	  {			/* continuation of a chain from abv. */
	    if (tr[t].usave > 0) /* three upper neighbours */
		  {
		    if (tr[t].uside == S_LEFT)
		    {
		      tr[tn].u0 = tr[t].u1;
		      tr[t].u1 = -1;
		      tr[tn].u1 = tr[t].usave;
		      
		      tr[tr[t].u0].d0 = t;
		      tr[tr[tn].u0].d0 = tn;
		      tr[tr[tn].u1].d0 = tn;
		    }
		    else		/* intersects in the right */
		    {
		      tr[tn].u1 = -1;
		      tr[tn].u0 = tr[t].u1;
		      tr[t].u1 = tr[t].u0;
		      tr[t].u0 = tr[t].usave;

		      tr[tr[t].u0].d0 = t;
		      tr[tr[t].u1].d0 = t;
		      tr[tr[tn].u0].d0 = tn;		      
		    }
		  
		    tr[t].usave = tr[tn].usave = 0;
		  }
	    else		/* No usave.... simple case */
		  {
		    tr[tn].u0 = tr[t].u1;
		    tr[tn].u1 = -1;
		    tr[t].u1 = -1;
		    tr[tr[tn].u0].d0 = tn;
		  }
	  }
	  else 
	  {			/* fresh seg. or upward cusp */
	    int tmp_u = tr[t].u0;
	    int td0, td1;
	    if (((td0 = tr[tmp_u].d0) > 0) &&  ((td1 = tr[tmp_u].d1) > 0))
		  {		/* upward cusp */
		    if ((tr[td0].rseg > 0) && !is_left_of(tr[td0].rseg, s.v1, seg ))
		    {
		      tr[t].u0 = tr[t].u1 = tr[tn].u1 = -1;
		      tr[tr[tn].u0].d1 = tn;
		    }
		    else 
		    {
		      tr[tn].u0 = tr[tn].u1 = tr[t].u1 = -1;
		      tr[tr[t].u0].d0 = t;
		    }
		  }
	    else		/* fresh segment */
		  {
		    tr[tr[t].u0].d0 = t;
		    tr[tr[t].u0].d1 = tn;
		  }
	  }
	  
	  if ( (tr[t].lo.y() == tr[tlast].lo.y()) && ( tr[t].lo.x() == tr[tlast].lo.x()) && tribot)
	  {
	    /* this case arises only at the lowest trapezoid.. i.e.
		tlast, if the lower endpoint of the segment is
		already inserted in the structure */
	    
	    tr[tr[t].d0].u0 = t;
	    tr[tr[t].d0].u1 = -1;
	    tr[tr[t].d1].u0 = tn;
	    tr[tr[t].d1].u1 = -1;

	    tr[tn].d0 = tr[t].d1;
	    tr[t].d1 = tr[tn].d1 = -1;
	    
	    tnext = tr[t].d1;	      
	  }
	  else if (i_d0)
				/* intersecting d0 */
	  {
	    tr[tr[t].d0].u0 = t;
	    tr[tr[t].d0].u1 = tn;
	    tr[tr[t].d1].u0 = tn;
	    tr[tr[t].d1].u1 = -1;
	    
	    /* new code to determine the bottom neighbours of the */
	    /* newly partitioned trapezoid */
	    
	    tr[t].d1 = -1;

	    tnext = tr[t].d0;
	  }
	  else			/* intersecting d1 */
	  {
	    tr[tr[t].d0].u0 = t;
	    tr[tr[t].d0].u1 = -1;
	    tr[tr[t].d1].u0 = t;
	    tr[tr[t].d1].u1 = tn;

	    /* new code to determine the bottom neighbours of the */
	    /* newly partitioned trapezoid */
	    
	    tr[tn].d0 = tr[t].d1;
	    tr[tn].d1 = -1;
	    
	    tnext = tr[t].d1;
	  }	    
	  
	  t = tnext;
	}
      
      tr.at(t_sav).rseg = tr.at(tn_sav).lseg  = segnum;
    } /* end-while */
  
  /* Now combine those trapezoids which share common segments. We can */
  /* use the pointers to the parent to connect these together. This */
  /* works only because all these new trapezoids have been formed */
  /* due to splitting by the segment, and hence have only one parent */

  tfirstl = tfirst; 
  tlastl = tlast;
  merge_trapezoids(segnum, tfirstl, tlastl, S_LEFT, qs, tr );
  merge_trapezoids(segnum, tfirstr, tlastr, S_RIGHT, qs, tr );

  seg.at(segnum).is_inserted = true;
  return 0;
}



/* Update the roots stored for each of the endpoints of the segment.
 * This is done to speed up the location-query for the endpoint when
 * the segment is inserted into the trapezoidation subsequently
 */
int find_new_roots( int segnum, std::vector< Node > &qs, std::vector< Segment > &seg, std::vector< Trapezoid > &tr  )
{
  Segment *s = &seg.at(segnum);
  
  if (s->is_inserted)
    return 0;

  s->root0 = locate_endpoint( s->v0, s->v1, s->root0, qs, seg );
  s->root0 = tr.at(s->root0).sink;

  s->root1 = locate_endpoint( s->v1, s->v0, s->root1, qs, seg );
  s->root1 = tr.at(s->root1).sink;  
  return 0;
}



  /* Main routine to perform trapezoidation */
void construct_trapezoids( int nseg, std::vector < Node > &qs, std::vector< Trapezoid > &tr, std::vector < Segment > &seg )
{
  int i ( 1 );
  int h ( 1 );

  int choose_idx ( 0 );
  std::vector < int > permute;

  //3186504610
  permute.resize( nseg + 1 );

  //3186504610
  choose_idx = 1;

  //Seed the random number generator
  srand( time(0) );

  //3186504610
  for (i = 1; i <= nseg; i++ )
  {
    permute.at( i ) = i;
  }

  //3186504610
  std::random_shuffle ( permute.begin() + 1, permute.end() );

  
  /* Add the first segment and get the query structure and trapezoid */
  /* list initialised */

  int root ( init_query_structure( choose_idx++, qs, tr, seg  ) );

  //3186504610
  for (i = 1; i <= nseg; i++)
    seg.at(i).root0 = seg.at(i).root1 = root;
  
  for (h = 1; h <= math_logstar_n(nseg); h++)
  {
    for (i = math_N(nseg, h -1) + 1; i <= math_N(nseg, h); i++)
	    add_segment(choose_idx++, qs, tr, seg );
      
    /* Find a new root for each of the segment endpoints */
    //3186504610
    for (i = 1; i <= nseg; i++)
	    find_new_roots(i, qs, seg, tr );
  }
  
  for (i = math_N(nseg, math_logstar_n(nseg)) + 1; i <= nseg; i++)
    add_segment( choose_idx++, qs, tr, seg );

}

#define CROSS_SINE(v0, v1) ((v0).x() * (v1).y() - (v1).x() * (v0).y() )


static int chain_idx = 0;
static int mon_idx = 0;



int traverse_polygon(int, int, int, int, 
                     std::vector < Trapezoid > &, 
                     std::vector < Segment > &,
                     std::vector < Monchain > &mchain, 
                     std::vector < Vertexchain > &vert,
                     std::vector < int > &mon,
                     std::vector< bool > &visited);


/* Function returns true if the trapezoid lies inside the polygon */
bool inside_polygon( Trapezoid *t, std::vector < Segment > &seg )
{
  int rseg = t->rseg;

  //Return false if not valid
  if ( !t->valid() )
    return false;

  if ((t->lseg <= 0) || (t->rseg <= 0))
    return false;
  
  if (((t->u0 <= 0) && (t->u1 <= 0)) || ((t->d0 <= 0) && (t->d1 <= 0))) /* triangle */
    return ( greaterThan( seg.at(rseg).v1, seg.at(rseg).v0));
  
  return false;
}


/* return a new mon structure from the table */
int newmon()
{
  return ++mon_idx;
}


/* return a new chain element from the table */
int new_chain_element()
{
  return ++chain_idx;
}


double get_angle( Point *vp0, Point *vpnext, Point *vp1 )
{
  Point v0, v1;
  
  v0.x() = vpnext->x() - vp0->x();
  v0.y() = vpnext->y() - vp0->y();

  v1.x() = vp1->x() - vp0->x();
  v1.y() = vp1->y() - vp0->y();

  if (CROSS_SINE(v0, v1) >= 0)	/* sine is positive */
    return DOT(v0, v1)/v0.length()/v1.length();
  else
    return (-1.0 * DOT(v0, v1)/v0.length()/v1.length() - 2);
}


/* (v0, v1) is the new diagonal to be added to the polygon. Find which */
/* chain to use and return the positions of v0 and v1 in p and q */ 
void get_vertex_positions(int v0, int v1, int &ip, int &iq, std::vector < Vertexchain > &vert )
{
  double temp ( 0.0 );
  int tp ( 0 );
  int tq ( 0 );

  Vertexchain *vp0 ( &vert.at ( v0 ) );
  Vertexchain *vp1 ( &vert.at ( v1 ) );
  
  /* p is identified as follows. Scan from (v0, v1) rightwards till */
  /* you hit the first segment starting from v0. That chain is the */
  /* chain of our interest */
  
  double angle ( -4.0 );
  for (int i = 0; i < 4; i++)
  {
    if (vp0->vnext[i] <= 0)
	    continue;
    if ((temp = get_angle(&vp0->pt, &(vert.at( vp0->vnext[i] ).pt), &vp1->pt)) > angle)
	  {
	    angle = temp;
	    tp = i;
	  }
  }

  ip = tp;

  /* Do similar actions for q */

  angle = -4.0;
  for (int i = 0; i < 4; i++)
  {
    if (vp1->vnext[i] <= 0)
	    continue;      
    if ((temp = get_angle(&vp1->pt, &(vert.at ( vp1->vnext[i] ).pt), &vp0->pt)) > angle)
	  {
	    angle = temp;
	    tq = i;
	  }
  }

  iq = tq;

}

  
/* v0 and v1 are specified in anti-clockwise order with respect to 
 * the current monotone polygon mcur. Split the current polygon into 
 * two polygons using the diagonal (v0, v1) 
 */
int make_new_monotone_poly( int mcur, int v0, int v1, std::vector < Monchain > &mchain, std::vector < Vertexchain > &vert, std::vector < int > &mon )
{
  Vertexchain *vp0 ( &vert.at( v0 ) );
  Vertexchain *vp1 ( &vert.at( v1 ) );

  int ip ( 0 );
  int iq ( 0 );
  get_vertex_positions(v0, v1, ip, iq, vert );

  int p ( vp0->vpos[ip] );
  int q ( vp1->vpos[iq] );

  /* At this stage, we have got the positions of v0 and v1 in the */
  /* desired chain. Now modify the linked lists */

  int i ( new_chain_element() );	/* for the new list */
  int j ( new_chain_element() );

  mchain.at( i ).vnum = v0;
  mchain.at( j ).vnum = v1;

  mchain.at( i ).next = mchain.at( p ).next;
  mchain.at( mchain.at( p ).next ).prev = i;
  mchain.at( i ).prev = j;
  mchain.at( j ).next = i;
  mchain.at( j ).prev = mchain.at( q ).prev;
  mchain.at( mchain.at( q ).prev ).next = j;

  mchain.at( p ).next = q;
  mchain.at( q ).prev = p;

  int nf0 ( vp0->nextfree );
  int nf1 ( vp1->nextfree );

  vp0->vnext[ip] = v1;

  vp0->vpos[nf0] = i;
  vp0->vnext[nf0] = mchain.at( mchain.at( i ).next ).vnum;
  vp1->vpos[nf1] = j;
  vp1->vnext[nf1] = v0;

  vp0->nextfree++;
  vp1->nextfree++;

  int mnew ( newmon() );

  mon.at( mcur ) = p;
  mon.at( mnew ) = i;

  return mnew;
}

/* Main routine to get monotone polygons from the trapezoidation of 
 * the polygon.
 */

int monotonate_trapezoids( int n, std::vector < Trapezoid > &tr, std::vector < Segment > &seg, 
                     std::vector < Monchain > &mchain, 
                     std::vector < Vertexchain > &vert,
                     std::vector < int > &mon,
                     std::vector< bool > &visited )
{
  int tr_start = 0;

  
  /* First locate a trapezoid which lies inside the polygon */
  /* and which is triangular */
  for ( unsigned int i = 0; i < tr.size(); i++)
  {
    if (inside_polygon(&tr.at(i), seg ))
    {
      tr_start = i;
      break;
    }
  }
  
  
  /* Initialise the mon data-structure and start spanning all the */
  /* trapezoids within the polygon */

  //3186504610
  for (int i = 1; i <= n; i++)
  {
    mchain.at(i).prev = seg.at(i).prev;
    mchain.at(i).next = seg.at(i).next;
    mchain.at(i).vnum = i;
    vert.at(i).pt = seg.at(i).v0;
    vert.at(i).vnext[0] = seg.at(i).next; /* next vertex */
    vert.at(i).vpos[0] = i;	/* locn. of next vertex */
    vert.at(i).nextfree = 1;
  }

  chain_idx = n;
  mon_idx = 0;
  mon.at(0) = 1;			// position of any vertex in the first chain  

  
  /* traverse the polygon */
  if (tr.at(tr_start).u0 > 0)
    traverse_polygon(0, tr_start, tr.at(tr_start).u0, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
  else if (tr.at(tr_start).d0 > 0)
    traverse_polygon(0, tr_start, tr.at(tr_start).d0, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
  
  /* return the number of polygons created */
  return newmon();
}


/* recursively visit all the trapezoids */
int traverse_polygon( int mcur, int trnum, int from, int dir, 
                     std::vector < Trapezoid > &tr, 
                     std::vector < Segment > &seg, 
                     std::vector < Monchain > &mchain, 
                     std::vector < Vertexchain > &vert,
                     std::vector < int > &mon,
                     std::vector< bool > &visited )
{
  if( ( trnum <= 0 ) || visited[trnum] )
    return 0;

  Trapezoid *t = &tr.at(trnum);
  int mnew ( -1 );
  int v0 ( -1 );
  int v1 ( -1 );
  int retval ( -1 );

  bool do_switch ( false );

  visited.at(trnum) = true;
  
  /* We have much more information available here. */
  /* rseg: goes upwards   */
  /* lseg: goes downwards */

  /* Initially assume that dir = TR_FROM_DN (from the left) */
  /* Switch v0 and v1 if necessary afterwards */


  /* special cases for triangles with cusps at the opposite ends. */
  /* take care of this first */
  if ((t->u0 <= 0) && (t->u1 <= 0))
  {
    if ((t->d0 > 0) && (t->d1 > 0)) /* downward opening triangle */
	  {
	    v0 = tr.at(t->d1).lseg;
	    v1 = t->lseg;
	    if (from == t->d1)
	    {
	      do_switch = true;
	      mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon );
	      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );	    
	    }
	    else
	    {
	      mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
	      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	    }
	  }
    else
	  {
	    retval = SP_NOSPLIT;	/* Just traverse all neighbours */
	    traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	    traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	    traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	    traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	  }
  }
  
  else if ((t->d0 <= 0) && (t->d1 <= 0))
  {
    if ((t->u0 > 0) && (t->u1 > 0)) /* upward opening triangle */
	  {
	    v0 = t->rseg;
	    v1 = tr.at(t->u0).rseg;
	    if (from == t->u1)
	    {
	      do_switch = true;
	      mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
	      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );	    
	    }
	    else
	    {
	      mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
	      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	    }
	  }
    else
	  {
	    retval = SP_NOSPLIT;	/* Just traverse all neighbours */
	    traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	    traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	    traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	    traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	  }
  }
  
  else if ((t->u0 > 0) && (t->u1 > 0)) 
    {
      if ((t->d0 > 0) && (t->d1 > 0)) /* downward + upward cusps */
	{
	  v0 = tr.at(t->d1).lseg;
	  v1 = tr.at(t->u0).rseg;
	  retval = SP_2UP_2DN;
	  if (((dir == TR_FROM_DN) && (t->d1 == from)) ||
	      ((dir == TR_FROM_UP) && (t->u1 == from)))
	    {
	      do_switch = true;
	      mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
	      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	    }
	  else
	    {
	      mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
	      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );	      
	    }
	}
      else			/* only downward cusp */
	{
	  if ( t->lo == seg.at(t->lseg).v1)
	    {
	      v0 = tr.at(t->u0).rseg;
	      v1 = seg.at(t->lseg).next;

	      retval = SP_2UP_LEFT;
	      if ((dir == TR_FROM_UP) && (t->u0 == from))
		{
		  do_switch = true;
		  mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon );
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		}
	    }
	  else
	    {
	      v0 = t->rseg;
	      v1 = tr.at(t->u0).rseg;	
	      retval = SP_2UP_RIGHT;
	      if ((dir == TR_FROM_UP) && (t->u1 == from))
		{
		  do_switch = true;
		  mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		}
	    }
	}
    }
  else if ((t->u0 > 0) || (t->u1 > 0)) /* no downward cusp */
    {
      if ((t->d0 > 0) && (t->d1 > 0)) /* only upward cusp */
	{
	  if ( t->hi == seg.at (t->lseg).v0 )
	    {
	      v0 = tr.at(t->d1).lseg;
	      v1 = t->lseg;
	      retval = SP_2DN_LEFT;
	      if (!((dir == TR_FROM_DN) && (t->d0 == from)))
		{
		  do_switch = true;
		  mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );	      
		}
	    }
	  else
	    {
	      v0 = tr.at(t->d1).lseg;
	      v1 = seg.at(t->rseg).next;

	      retval = SP_2DN_RIGHT;	    
	      if ((dir == TR_FROM_DN) && (t->d1 == from))
		{
		  do_switch = true;
		  mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		}
	    }
	}
      else			/* no cusp */
	{
	  if ( t->hi == seg.at(t->lseg).v0 && t->lo == seg.at(t->rseg).v0 )
	    {
	      v0 = t->rseg;
	      v1 = t->lseg;
	      retval = SP_SIMPLE_LRDN;
	      if (dir == TR_FROM_UP)
		{
		  do_switch = true;
		  mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		}
	    }
	  else if ( t->hi == seg.at(t->rseg).v1 && t->lo == seg.at(t->lseg).v1)
	    {
	      v0 = seg.at(t->rseg).next;
	      v1 = seg.at(t->lseg).next;

	      retval = SP_SIMPLE_LRUP;
	      if (dir == TR_FROM_UP)
		{
		  do_switch = true;
		  mnew = make_new_monotone_poly(mcur, v1, v0, mchain, vert, mon);
		  traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		}
	      else
		{
		  mnew = make_new_monotone_poly(mcur, v0, v1, mchain, vert, mon);
		  traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		  traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
		}
	    }
	  else			/* no split possible */
	    {
	      retval = SP_NOSPLIT;
	      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN, tr, seg, mchain, vert, mon, visited );
	      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP, tr, seg, mchain, vert, mon, visited );	      	      
	    }
	}
    }

  return retval;
}

/* A greedy corner-cutting algorithm to triangulate a y-monotone 
 * polygon in O(n) time.
 * Joseph O-Rourke, Computational Geometry in C.
 */
template < class Triangles >
bool triangulate_single_polygon( int nvert, int posmax, int side, Triangles &op,
                     std::vector < Monchain > &mchain, 
                     std::vector < Vertexchain > &vert )
{
  typedef typename Triangles::value_type Triangle;

  int v ( 0 );
  std::vector< int > rc ( vert.size() ); /* reflex chain */
  unsigned int ri ( 0 );	
  int endv ( 0 );
  int vpos ( 0 );
  
  if (side == TRI_RHS)		/* RHS segment is a single segment */
  {
    rc.at(0) = mchain.at(posmax).vnum;
    int tmp ( mchain.at(posmax).next );
    rc.at(1) = mchain.at(tmp).vnum;
    ri = 1;
    
    vpos = mchain.at(tmp).next;
    v = mchain.at(vpos).vnum;
    
    if ((endv = mchain.at(mchain.at(posmax).prev).vnum) == 0)
	    endv = nvert;
  }
  else				/* LHS is a single segment */
  {
    int tmp ( mchain.at(posmax).next );
    rc.at(0) = mchain.at(tmp).vnum;
    tmp = mchain.at(tmp).next;
    rc.at(1) = mchain.at(tmp).vnum;
    ri = 1;

    vpos = mchain.at(tmp).next;
    v = mchain.at(vpos).vnum;

    endv = mchain.at(posmax).vnum;
  }
  
  while ((v != endv) || (ri > 1))
  {
    if ( ri > ( rc.size() - 1 ) )
      return false;

    if (ri > 0)		// reflex chain is non-empty 
	  {
	    if (CROSS(vert.at( v ).pt, vert.at( rc.at(ri - 1) ).pt, vert.at( rc.at(ri) ).pt) > 0 ) // convex corner: cut if off 
	    {			
        //Add a new triangle
        Triangle t ( rc.at(ri - 1), rc.at(ri), v );
        op.push_back( t );
	      ri--;
	    }
	    else		// non-convex 
	    {		
        // add v to the chain
	      ri++;
	      rc.at(ri) = v;
	      vpos = mchain.at(vpos).next;
	      v = mchain.at(vpos).vnum;
	    }
	  }
    else			
	  {			
      // reflex-chain empty: add v to the reflex chain and advance it
	    rc.at(++ri) = v;
	    vpos = mchain.at(vpos).next;
	    v = mchain.at(vpos).vnum;
	  }
  } // end-while
  
  // reached the bottom vertex. Add in the triangle formed 
  Triangle t ( rc.at(ri - 1), rc.at(ri), v );
  op.push_back( t );
  ri--;
  
  return true;
}


/* For each monotone polygon, find the ymax and ymin (to determine the */
/* two y-monotone chains) and pass on this monotone polygon for greedy */
/* triangulation. */
/* Take care not to triangulate duplicate monotone polygons */

template < class Triangles >
void triangulate_monotone_polygons( int nvert, int nmonpoly, Triangles &op, 
                     std::vector < Monchain > &mchain, 
                     std::vector < Vertexchain > &vert,
                     std::vector < int > &mon )
{
  typedef typename Triangles::value_type Triangle;

  int v ( 0 );

  for (int i = 0; i < nmonpoly; i++)
  {
    int vcount ( 1 );
    bool processed ( false );
    
    int vfirst ( mchain.at( mon.at( i ) ).vnum );

    //ymax and ymin
    Point ymax ( vert.at( vfirst ).pt );
    Point ymin ( vert.at( vfirst ).pt );

    //Max and min pos
    int posmax ( mon.at( i ) );
    int posmin ( mon.at( i ) );

    mchain.at( mon.at ( i ) ).marked = true;

    int p ( mchain.at( mon.at( i ) ).next );

    while ((v = mchain.at( p ).vnum) != vfirst)
	  {
	    if (mchain.at( p ).marked)
	    {
	      processed = true;
	      break;		/* break from while */
	    }
	    else
	      mchain.at( p ).marked = true;

	    if ( greaterThan( vert.at(v).pt, ymax))
	    {
	      ymax = vert.at( v ).pt;
	      posmax = p;
	    }
	    if ( lessThan( vert.at(v).pt, ymin))
	    {
	      ymin = vert.at( v ).pt;
	      posmin = p;
	    }
	    p = mchain.at( p ).next;
	    vcount++;
    }

    if (processed)		/* Go to next polygon */
	    continue;
      
    if (vcount == 3)		/* already a triangle */
    {
      Triangle t ( mchain.at( p ).vnum, mchain.at( mchain.at( p ).next).vnum, mchain.at( mchain.at( p ).prev ).vnum );
      op.push_back( t );
    }
    else			/* triangulate the polygon */
	  {
	    v = mchain.at( mchain.at ( posmax ).next ).vnum;
	    if ( vert.at(v).pt == ymin )
	    {			/* LHS is a single line */
	      if( !triangulate_single_polygon(nvert, posmax, TRI_LHS, op, mchain, vert )  )
          return;
	    }
	    else
	      if( !triangulate_single_polygon(nvert, posmax, TRI_RHS, op, mchain, vert )  )
          return;
	  }
  }
}



}



/* Input specified as contours.
 * Outer contour must be anti-clockwise.
 * All inner contours must be clockwise.
 *  
 * Every contour is specified by giving all its points in order. No
 * point shoud be repeated. i.e. if the outer contour is a square,
 * only the four distinct endpoints shopudl be specified in order.
 *  
 * ncontours: #contours
 * cntr: An array describing the number of points in each
 *	 contour. Thus, cntr[i] = #points in the i'th contour.
 * vertices: Input array of vertices. Vertices for each contour
 *           immediately follow those for previous one. Array location
 *           vertices[0] must NOT be used (i.e. i/p starts from
 *           vertices[1] instead. The output triangles are
 *	     specified  w.r.t. the indices of these vertices.
 * triangles: Container of 3 indices to represent the triangle
 *  
 * Enough space must be allocated for all the arrays before calling
 * this routine
 */

template < class VertexSequence, class Triangles >
void triangulate_polygon( int ncontours, int cntr[], const VertexSequence &vertices, Triangles &triangles )
{
  /*
  See 3186504610 for places that I think will need to be changed to make vertices start at 0 instead of 1.
  I'm not sure if I've found all of the places
  */

  
  int numVerts ( vertices.size() - 1 );

  const unsigned int segSize ( numVerts * 2);
  const unsigned int trSize  ( segSize * 4 );
  const unsigned int qSize   ( segSize * 8 );

  std::vector < Detail::Node >         qs  ( qSize );		/* Query structure */
  std::vector < Detail::Trapezoid >    tr  ( trSize );		/* Trapezoid structure */
  std::vector < Detail::Segment >      seg ( segSize );		/* Segment table */

  std::vector < Detail::Monchain >     mchain ( trSize ); /* Table to hold all the monotone */
				                                        /* polygons . Each monotone polygon */
				                                         /* is a circularly linked list */

  std::vector < Detail::Vertexchain >  vert ( segSize ); /* chain init. information. This */
				                                          /* is used to decide which */
				                                          /* monotone polygon to split if */
				                                          /* there are several other */
				                                          /* polygons touching at the same */
				                                          /* vertex  */

  std::vector < int >          mon ( segSize );	/* contains position of any vertex in */
				                                        /* the monotone chain for the polygon */

  std::vector <bool>           visited ( trSize, false );


  //3186504610
  int i ( 1 );
  int ccount ( 0 );
  
  while (ccount < ncontours)
  {
    int npoints ( cntr[ccount] );
    int first ( i );
    int last  ( first + npoints - 1 ); //3186504610
    for (int j = 0; j < npoints; j++, i++)
	  {
	    seg.at (i).v0.x() = vertices[i][0];
    	seg.at (i).v0.y() = vertices[i][1];

	    if (i == last)
	    {
	      seg.at(i).next = first;
	      seg.at(i).prev = i-1;
	      seg.at(i-1).v1 = seg.at(i).v0;
	    }
	    else if (i == first)
	    {
	      seg.at(i).next = i+1;
	      seg.at(i).prev = last;
	      seg.at(last).v1 = seg.at(i).v0;
	    }
	    else
	    {
	      seg.at(i).prev = i-1;
	      seg.at(i).next = i+1;
	      seg.at(i-1).v1 = seg.at(i).v0;
	    }
	  }
      
    ccount++;
  }

  //Detail::generate_random_ordering( numVerts );

  //Make the trapezoids
  Detail::construct_trapezoids( numVerts, qs, tr, seg );

  //Make the monotone trapezoids
  int nmonpoly ( Detail::monotonate_trapezoids( numVerts, tr, seg, mchain, vert, mon, visited ) );

  //Return the number of triangles we made
  Detail::triangulate_monotone_polygons( numVerts, nmonpoly, triangles, mchain, vert, mon );

}


}
}

#endif // __USUL_ALGORITHMS_TRIANGULATE_H__

