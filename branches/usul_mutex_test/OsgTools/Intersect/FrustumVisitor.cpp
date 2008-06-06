  
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation adapted from osgUtil::IntersectVisitor.
//
///////////////////////////////////////////////////////////////////////////////

#include "FrustumVisitor.h"
#include "FrustumFunctor.h"

#include "osg/Group"
#include "osg/Transform"

#include <algorithm>

using namespace OsgTools::Intersect;

  
///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumVisitor::FrustumVisitor(  ) : BaseClass ( )
{
  // Always hit every node.
  const unsigned int all ( 0xffffffff );
  this->setTraversalMask    ( all );
  this->setNodeMaskOverride ( all );

  // overide the default node visitor mode.
  setTraversalMode(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);

  this->reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumVisitor::~FrustumVisitor()
{
}

Hit::Hit()
{
}


Hit::Hit(const Hit& hit)
{
    // copy data across.
    _ratio = hit._ratio;
    _originalFrustum = hit._originalFrustum;
    _localFrustum = hit._localFrustum;
    _nodePath = hit._nodePath;
    _geode = hit._geode;
    _drawable = hit._drawable;
    _matrix = hit._matrix;
    _inverse = hit._inverse;

    _vecIndexList = hit._vecIndexList;
    _primitiveIndex = hit._primitiveIndex;
    _intersectPoint = hit._intersectPoint;
    _intersectNormal = hit._intersectNormal;
}


Hit::~Hit()
{
}


Hit& Hit::operator = (const Hit& hit)
{
    if (&hit==this) return *this;

    _matrix = hit._matrix;
    _inverse = hit._inverse;
    _originalFrustum = hit._originalFrustum;
    _localFrustum = hit._localFrustum;

    // copy data across.
    _ratio = hit._ratio;
    _nodePath = hit._nodePath;
    _geode = hit._geode;
    _drawable = hit._drawable;

    _vecIndexList = hit._vecIndexList;
    _primitiveIndex = hit._primitiveIndex;
    _intersectPoint = hit._intersectPoint;
    _intersectNormal = hit._intersectNormal;

    return *this;
}

const osg::Vec3 Hit::getWorldIntersectNormal() const
{
    if (_inverse.valid()) 
    {
        osg::Vec3 norm = osg::Matrix::transform3x3(*_inverse,_intersectNormal); 
        norm.normalize();
        return norm;
    }
    else return _intersectNormal;
}

FrustumVisitor::IntersectState::IntersectState()
{
    _frustumMaskStack.push_back(0xffffffff);
}


FrustumVisitor::IntersectState::~IntersectState()
{
}


bool FrustumVisitor::IntersectState::isCulled(const osg::BoundingSphere& bs, FrustumMask& segMaskOut)
{
    bool hit = false;
    FrustumMask mask = 0x00000001;
    segMaskOut = 0x00000000;
    FrustumMask segMaskIn = _frustumMaskStack.back();
    //    notify(INFO) << << "IntersectState::isCulled() mask in "<<segMaskIn<<"  ";
    for(IntersectState::FrustumList::iterator sitr=_frustumList.begin();
        sitr!=_frustumList.end();
        ++sitr)
    {
        if ((segMaskIn & mask) && (sitr->second)->contains(bs))
        {
            //            notify(INFO) << << "Hit ";
            segMaskOut = segMaskOut| mask;
            hit = true;
        }
        mask = mask << 1;
    }
    //    notify(INFO) << << "mask = "<<segMaskOut<< std::endl;
    return !hit;
}


bool FrustumVisitor::IntersectState::isCulled(const osg::BoundingBox& bb, FrustumMask& segMaskOut)
{
    bool hit = false;
    FrustumMask mask = 0x00000001;
    segMaskOut = 0x00000000;
    FrustumMask segMaskIn = _frustumMaskStack.back();
    for(IntersectState::FrustumList::iterator sitr=_frustumList.begin();
        sitr!=_frustumList.end();
        ++sitr)
    {
        if ((segMaskIn & mask) && (sitr->second)->contains(bb))
        {
            segMaskOut = segMaskOut| mask;
            hit = true;
        }
        mask = mask << 1;
    }
    return !hit;
}

void FrustumVisitor::reset()
{
  _intersectStateStack.clear();

  // create a empty IntersectState on the the intersectStateStack.
  _intersectStateStack.push_back(new IntersectState);

  _hitList.clear();
}


void FrustumVisitor::addFrustum ( Frustum* frustum )
{
  if ( 0x0 == frustum ) 
    return;
  
  IntersectState* cis = _intersectStateStack.back().get();
  
  // first check to see if segment has already been added.
  for(IntersectState::FrustumList::iterator itr = cis->_frustumList.begin();
      itr != cis->_frustumList.end();
      ++itr)
  {
      if ( itr->first == frustum )
        return;
  }

  // create a new segment transformed to local coordintes.
  Frustum* nf = new Frustum;

  if (cis->_inverse.valid())
  {
    *nf = *frustum;
    nf->transform ( *cis->_inverse.get() );
  }
  else *nf = *frustum;
  
  cis->addFrustumPair( frustum ,nf );

}


void FrustumVisitor::_pushMatrix(const osg::Matrix& matrix)
{
  IntersectState* nis = new IntersectState;

  IntersectState* cis = _intersectStateStack.back().get();

  if (cis->_matrix.valid())
  {
    nis->_matrix = new osg::RefMatrix;
    nis->_matrix->mult(matrix,*(cis->_matrix));
  }
  else
  {
    nis->_matrix = new osg::RefMatrix(matrix);
  }

  osg::RefMatrix* inverse_world = new osg::RefMatrix;
  inverse_world->invert(*(nis->_matrix));
  nis->_inverse = inverse_world;

  IntersectState::FrustumMask segMaskIn = cis->_frustumMaskStack.back();
  IntersectState::FrustumMask mask = 0x00000001;
  for(IntersectState::FrustumList::iterator sitr=cis->_frustumList.begin();
      sitr!=cis->_frustumList.end();
      ++sitr)
  {
      if ((segMaskIn & mask))
      {
          Frustum* seg = new Frustum;
          *seg = *(sitr->first);
          seg->transform (*inverse_world);
          nis->addFrustumPair(sitr->first.get(),seg);
      }
      mask = mask << 1;
  }

  _intersectStateStack.push_back(nis);
}


void FrustumVisitor::_popMatrix()
{
  if (!_intersectStateStack.empty())
  {
      _intersectStateStack.pop_back();
  }
}

bool FrustumVisitor::_enterNode( osg::Node& node)
{
  const osg::BoundingSphere& bs = node.getBound();
    if (bs.valid())
    {
        IntersectState* cis = _intersectStateStack.back().get();
        IntersectState::FrustumMask sm=0xffffffff;
        if (cis->isCulled(bs,sm)) return false;
        cis->_frustumMaskStack.push_back(sm);
        return true;
    }
    else
    {
        return false;
    }
}


void FrustumVisitor::_leaveNode()
{
    IntersectState* cis = _intersectStateStack.back().get();
    cis->_frustumMaskStack.pop_back();
}


void FrustumVisitor::apply( osg::Node& node)
{
  if (!_enterNode(node)) return;

  traverse(node);

  _leaveNode();
}


void FrustumVisitor::apply( osg::Geode& geode)
{
  if (!_enterNode(geode)) return;

  for(unsigned int i = 0; i < geode.getNumDrawables(); i++ )
  {
      _intersect(*geode.getDrawable(i));
  }
  
  _leaveNode();
}


void FrustumVisitor::apply( osg::Group& node )
{
  if (! _enterNode(node)) return;

  traverse(node);

  _leaveNode();
}


void FrustumVisitor::apply( osg::Transform& node)
{
  if (!_enterNode(node)) return;

  osg::ref_ptr<osg::RefMatrix> matrix = new osg::RefMatrix;
  node.computeLocalToWorldMatrix(*matrix,this);

  _pushMatrix(*matrix);

  traverse(node);

  _popMatrix();

  _leaveNode();
}


void FrustumVisitor::apply( osg::Switch& node)
{
  // TODO Only go down active nodes.
  apply((osg::Group&)node);
}


void FrustumVisitor::apply( osg::LOD& node)
{
  // TODO Only go down the highest lod.
  apply((osg::Group&)node);
}


struct GeomHit
{
  GeomHit(unsigned int index, const osg::Vec3& normal, const osg::Vec3& ip, float r1, const osg::Vec3* v1, float r2, const osg::Vec3* v2, float r3, const osg::Vec3* v3):
        _index(index),
        _normal(normal),
        _ipt ( ip ),
        _r1(r1),
        _v1(v1),        
        _r2(r2),
        _v2(v2),        
        _r3(r3),
        _v3(v3) {}

    GeomHit(unsigned int index, const osg::Vec3& normal, const osg::Vec3& ip, float r1, const osg::Vec3* v1, float r2, const osg::Vec3* v2 ):
        _index(index),
        _normal(normal),
        _ipt ( ip ),
        _r1(r1),
        _v1(v1),        
        _r2(r2),
        _v2(v2),        
        _r3(0.0),
        _v3(0x0) {}

    GeomHit(unsigned int index, const osg::Vec3& normal, const osg::Vec3& ip, float r1, const osg::Vec3* v1 ):
        _index(index),
        _normal(normal),
        _ipt ( ip ),
        _r1(r1),
        _v1(v1),        
        _r2(0.0),
        _v2(0x0),        
        _r3(0),
        _v3(0x0) {}

    unsigned int        _index;
    osg::Vec3	          _ipt;  // Intersection point.
    const osg::Vec3     _normal;
    float               _r1;
    const osg::Vec3*    _v1;        
    float               _r2;
    const osg::Vec3*    _v2;        
    float               _r3;
    const osg::Vec3*    _v3;        
};


struct Intersector
{
    osg::ref_ptr<Frustum> frustum;

    int _index;
    float _ratio;
    bool _hit;
    
    

    typedef std::map<float,GeomHit> HitList;
    
    HitList _hl;

    Intersector()
    {
    }

    Intersector(const OsgTools::Intersect::Frustum& seg,float ratio=FLT_MAX)
    {
        set(seg,ratio);
    }
    
    void set(const OsgTools::Intersect::Frustum& seg,float ratio=FLT_MAX)
    {
      frustum=new OsgTools::Intersect::Frustum(seg);
        _hit=false;
        _index = 0;
        _ratio = ratio;
    }

    inline void operator () (const osg::Vec3& v1,const osg::Vec3& v2,const osg::Vec3& v3, bool treatVertexDataAsTemporary)
    {
      ++_index;
#if 0
      if( frustum->contains ( v1 ) || frustum->contains( v2 ) || frustum->contains ( v3 ) )
      {
        osg::Vec3 ip ( ( v1 + v2 + v3 ) / 3 );
        float ratio ( frustum->getPlaneList().at( 0 ).distance( ip ) );

        if ( treatVertexDataAsTemporary )
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), ip, 0.0, 0x0, 0.0, 0x0, 0.0, 0x0 ) ) );
        else
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), ip, 0.0, &v1, 0.0, &v2, 0.0, &v3 ) ) );

        _hit = true;
      }
#endif
    }

    inline void operator () (const osg::Vec3& v1,const osg::Vec3& v2, bool treatVertexDataAsTemporary)
    {
      ++_index;

      std::vector< osg::Vec3 > vertices;
      frustum->intersect( v1, v2, vertices );

      // Handle trival case
      if ( vertices.size() == 2 )
      {
        osg::Vec3 ip ( ( vertices.at( 0 ) + vertices.at( 1 ) ) / 2 );
        float ratio ( frustum->getPlaneList().at( 0 ).distance( ip ) );        

        if ( treatVertexDataAsTemporary )
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), ip, 0.0, 0x0, 0.0, 0x0 ) ) );
        else
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), ip, 0.0, &v1, 0.0, &v2 ) ) );
          

        _hit = true;
      }
      else if ( vertices.size() == 1 )
      {
        osg::Vec3 ip;
        if( frustum->contains ( v1 ) )
        {
          ip = ( ( v1 + vertices.at( 0 ) ) / 2 );
        }
        
        else if ( frustum->contains( v2 ) )
        {
          ip = ( ( v2 + vertices.at( 0 ) ) / 2 );
        }

        float ratio ( frustum->getPlaneList().at( 0 ).distance( ip ) );        

        if ( treatVertexDataAsTemporary )
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), ip, 0.0, 0x0, 0.0, 0x0 ) ) );
        else
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), ip, 0.0, &v1, 0.0, &v2 ) ) );
          
        _hit = true;
      }
    }

    inline void operator () (const osg::Vec3& v1, bool treatVertexDataAsTemporary)
    {
      ++_index;

      if( frustum->contains ( v1 ) )
      {
        float ratio ( frustum->getPlaneList().at( 0 ).distance( v1 ) );

        if ( treatVertexDataAsTemporary )
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), v1, 0.0, 0x0 ) ) );
        else
          _hl.insert ( std::pair <float,GeomHit> ( ratio, GeomHit ( _index, osg::Vec3(), v1, 0.0, &v1 ) ) );
          
        _hit = true;
      }
    }

};

bool FrustumVisitor::_intersect( osg::Drawable& drawable)
{
    bool hitFlag = false;

    IntersectState* cis = _intersectStateStack.back().get();

    const osg::BoundingBox& bb = drawable.getBound();

    for(IntersectState::FrustumList::iterator sitr=cis->_frustumList.begin();
        sitr!=cis->_frustumList.end();
        ++sitr)
    {
        if (sitr->second->contains(bb))
        {

            FrustumFunctor< Intersector > ti;
            ti.set(*sitr->second);
            drawable.accept(ti);
            if (ti._hit)
            {
            
                osg::Geometry* geometry = drawable.asGeometry();
                

                for( Intersector::HitList::iterator thitr=ti._hl.begin();
                    thitr!=ti._hl.end();
                    ++thitr)
                {
                
                    Hit hit;
                    hit._nodePath = _nodePath;
                    hit._matrix = cis->_matrix;
                    hit._inverse = cis->_inverse;
                    hit._drawable = &drawable;
                    if (_nodePath.empty()) hit._geode = NULL;
                    else hit._geode = dynamic_cast<osg::Geode*>(_nodePath.back());

                    GeomHit& triHit = thitr->second;
                    
                    hit._ratio = thitr->first;
                    hit._primitiveIndex = triHit._index;
                    hit._originalFrustum = sitr->first;
                    hit._localFrustum = sitr->second;

                    hit._intersectPoint = triHit._ipt;

                    hit._intersectNormal = triHit._normal;
                    
                    if (geometry)
                    {
                        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                        if (vertices)
                        {
                            osg::Vec3* first = &(vertices->front());
                            if (triHit._v1) hit._vecIndexList.push_back(triHit._v1-first);
                            if (triHit._v2) hit._vecIndexList.push_back(triHit._v2-first);
                            if (triHit._v3) hit._vecIndexList.push_back(triHit._v3-first);
                        }
                    }
                    

                    _hitList[sitr->first.get()].push_back(hit);

                    std::sort(_hitList[sitr->first.get()].begin(),_hitList[sitr->first.get()].end());

                    hitFlag = true;

                }
            }
        }
    }

    return hitFlag;

}


