
#include "SceneNodeVisitor.h"

#include "Brahma/Scene/Node.h"
#include "Brahma/Scene/LeafNode.h"
#include "Brahma/Scene/Group.h"
#include "Brahma/Scene/Transform.h"

#include "osg/Node"
#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgDB/ReadFile"

#include <sstream>

Parser::Scene::SceneNodeVisitor::SceneNodeVisitor() :
	_root		( new osg::Group() ), 
	_parent	( _root )
{	
}

osg::Group*	Parser::Scene::SceneNodeVisitor::root()
{
	return _root.get();
}

void Parser::Scene::SceneNodeVisitor::visit( Brahma::Scene::LeafNode& leaf, const bool& createNew  )
{		
	std::string path( leaf.data() );

	if( !path.empty() )
  {
		// Check here whether we have a dir or a file. 
	  osg::ref_ptr< osg::Node > osgNode = osgDB::readNodeFile( path );
    
	  if( osgNode.valid() )
		{
			osgNode->setName( leaf.id() );
      
			if( _parent.valid() )
			{
				_parent->addChild( osgNode.get() );
			}
    }
  }
}

void Parser::Scene::SceneNodeVisitor::visit( Brahma::Scene::Group& group, const bool& createNew )
{
	if( createNew )
	{
		osg::ref_ptr< osg::Group > osgGroup ( new osg::Group() );	
		osgGroup->setName( group.id() );
		
		if( _parent.valid() )
		{
			_parent->addChild( osgGroup.get() );

			_parent = osgGroup.get();
		}
	}

	// Visit children. 
	Brahma::Scene::Group::Children children( group.children() );
	
	Brahma::Scene::Group::Children::iterator itr( children.begin() );

	for( itr; itr != children.end(); ++itr )
	{
		( *itr )->accept( *this );		
	}

	// Reset out head to the last parent. 
	if( createNew )
	{
		_parent = _parent->getParent( 0 );
	}
}

void Parser::Scene::SceneNodeVisitor::visit( Brahma::Scene::Transform& transform, const bool& createNew )
{
	if( createNew )
	{
		// Hardcoded for matrix transform as of now. 
		osg::ref_ptr< osg::MatrixTransform > osgTransform( new osg::MatrixTransform() );
		if( osgTransform.valid() )
		{
			std::istringstream iss;
			Usul::Math::Vec3f position, pivot;
			Usul::Math::Vec4f rotation;
			
			position = transform.position();
			rotation = transform.rotation();
			pivot		 = transform.pivot();

			osgTransform->setName( transform.id() );		
			osgTransform->preMult( osg::Matrix::rotate( osg::DegreesToRadians( rotation[ 0 ] ), rotation[ 1 ], rotation[ 2 ], rotation[ 3 ] ) );
			osgTransform->preMult( osg::Matrix::translate( position[ 0 ], position[ 1 ], position[ 2 ] ) );
			
			_parent->addChild( osgTransform.get() );

			_parent = osgTransform.get();
		}	
	}

	visit( ( Brahma::Scene::Group& )( transform ) );
}
