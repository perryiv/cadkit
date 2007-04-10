
#include "Collision.h"

#include "osg/LineSegment"

#include "VrjUtil/OsgVjUtil.h"
#include "VrjUtil/OsgTreeTraversal.h" 

using namespace SimFlow::SimFlowUtil;

Collision::Collision::Collision()
{
}

Collision::Collision::~Collision()
{
}

Collision::Ray& Collision::Collision::ray( unsigned int index )
{
  if( index < mRays.size() )
  {
    return mRays[index];
  }
  else
  {
    throw std::runtime_error( "Index out of bound error: " );
  }
}

Collision::Ray& Collision::Collision::ray( std::string id )
{
  try
  {
    Rays::iterator itr;
    for( itr = mRays.begin(); itr != mRays.end(); ++itr )
    {
      if( itr->ID() == id )
      {
        return (*itr);
      }
    }
  }
  catch( std::exception& e )
  {
    std::cerr << "Ray with id " << id << " not found: " << std::endl;
  }
}

Collision::Collision::Rays Collision::Collision::rays()
{
	return mRays;
}

void Collision::Collision::ray( Ray ray )
{	
	mRays.push_back( ray );
}

void Collision::Collision::testCollision( osg::Node* rootNode, Ray& ray, Scope scope, std::string tag, unsigned int limit )
{
  std::vector< osg::Node* > nodes; 
  testCollision( rootNode, ray, nodes, scope, tag, limit );
}

void Collision::Collision::testCollision( osg::Node* rootNode, std::vector< Ray >& rays, Scope scope, std::string tag, unsigned int limit )
{
  for( size_t i=0; i < rays.size(); ++i )
  {
    testCollision( rootNode, rays[i], scope, tag, limit );
  }
}

void Collision::Collision::testCollision( osg::Node* rootNode, Ray& ray, std::vector< osg::Node* > nodes, Scope scope, std::string tag, unsigned int limit )
{	
	if( ray.active() )
	{		
		osg::NodePath nodePath = rootNode->getParentalNodePaths().front();
		osg::Matrix mat = osg::computeWorldToLocal( nodePath );

    if( scope == LOCAL )
    {
		  ray.startPoint( ray.startPoint() * mat );
		  ray.endPoint( ray.endPoint() * mat );
    }

		osg::ref_ptr< osg::LineSegment > seg ( new osg::LineSegment( osg::Vec3( ray.startPoint() ), osg::Vec3( ray.endPoint() ) ) ); 	
		osgUtil::IntersectVisitor::HitList hli;

		getHits( rootNode, seg.get(), hli );
		
		if( !hli.empty() )
		{
			osgUtil::IntersectVisitor::HitList::iterator hitr;			

      // As of now store values related to first collision only. 
			hitr = hli.begin();
			
      ray.worldIntersectPoint( hitr->getWorldIntersectPoint() );
			ray.worldIntersectNormal( hitr->getWorldIntersectNormal() );
			ray.localIntersectPoint( hitr->getLocalIntersectPoint() );
			ray.localIntersectNormal( hitr->getLocalIntersectNormal() );			

      osg::NodePath& nodePath = hitr->_nodePath;	

      unsigned int max = ( nodePath.size() >= limit ) ?  limit : nodePath.size(); 
			for( size_t i = 0; i < max; ++i )
			{
				osg::ref_ptr<osg::Node>  node	= ( nodePath.size()>=1 )?nodePath[nodePath.size()-1]:0;
				
				if( node.valid() ) 
        {  				
          if( !(  tag.compare("") == 0 ) )
          {
            osg::ref_ptr< osg::Node > target = VrjUtil::OsgTreeTraversal::findNodeByName( node.get(), tag, VrjUtil::OsgTreeTraversal::UP );
            nodes.push_back( target.get() );
		  	  }
          else
          {
            nodes.push_back( node.get() );
          }
        }
      }
    }
		else
		{
			// Save the values from the last intersection. 
		}
	}
}

void Collision::Collision::getHits( osg::Node* rootNode, osg::LineSegment* seg, osgUtil::IntersectVisitor::HitList& hli )
{   
	// Create an intersect visitor and add our ray to its segment list.
	osgUtil::IntersectVisitor   intersector ;
	intersector.addLineSegment( seg );

	// Pass the visitor to the node where we will start checking for intersections (here, root)
	rootNode->accept( intersector );

	// Get the hits	
	hli =  intersector.getHitList( seg );
} 

/*
void Collision::updateScene()
{
	std::vector< Ray >::iterator itr;
	for( itr=mRays.begin(); itr != mRays.end(); ++itr )
	{
		if( itr->active() )
		{
			osg::Matrixf mat = ( osg::Matrixf ) mController->getSceneTransform()->getMatrix();
			
			osg::Vec3f curPos;
		    curPos.set( mat.getTrans() );			

			// Subtract what we added last, get the intersection point 
			// and add the seperation on top of that. 
			curPos[1] = -( ( itr->worldIntersectPoint()[1] +  mCurrDistance[1] ) - curPos[1] );			
			curPos[2] = -( ( itr->worldIntersectPoint()[2] +  mCurrDistance[2] ) - curPos[2] );
			
			mat.setTrans( curPos );
			mController->getSceneTransform()->setMatrix( mat );
		}
	}
}

void Collision::update()
{
	mPitchMatrix = mController->getGlobalNavigator().getPitchMatrix(); 		
	//std::cout << " PitchMatrix is: " << mPitchMatrix << std::endl;	
		
	std::vector< Ray >::iterator itr;;
	for( itr=mRays.begin(); itr != mRays.end(); ++itr )
	{
		//itr->mStartPoint = itr->mBaseEndPoint * osg::Matrixf( mPitchMatrix.mData );
		//itr->mEndPoint	= itr->mBaseEndPoint   * osg::Matrixf( mPitchMatrix.mData );			
	}
	
    mCurrDistance = mOrigDistance * osg::Matrixf( mPitchMatrix.mData );

	checkCollision();

	updateScene();
}
*/
