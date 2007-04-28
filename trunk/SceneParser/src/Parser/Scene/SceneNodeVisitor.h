
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Class implements NodeVisitor interface for scene traversal. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE_SCENENODEVISITOR_H__
#define __SCENE_SCENENODEVISITOR_H__

#include "osg/Group"

#include "Brahma/Scene/NodeVisitor.h"

#include "Export.h"


///////////////////////////////////////////////////////////////////////////////
//
// Forward declarations. 
//
///////////////////////////////////////////////////////////////////////////////

namespace Brahma
{
	namespace Scene
	{
		class LeafNode;
		class Group;
		class Transform;
	}
}

namespace Parser
{
	namespace Scene
	{
		class PARSER_SCENENODEVISITOR_EXPORT SceneNodeVisitor : public Brahma::Scene::NodeVisitor
		{
			public:         
	      
				typedef osg::ref_ptr< osg::Group >				GroupPtr;	
				typedef std::vector< GroupPtr >						GroupPtrs;

				USUL_DECLARE_REF_POINTERS( SceneNodeVisitor );

				SceneNodeVisitor();				

				virtual osg::Group*												root();

				virtual void															visit( Brahma::Scene::LeafNode&, const bool& createNew = true );

				virtual void															visit( Brahma::Scene::Group&, const bool& createNew = true );

				virtual void															visit( Brahma::Scene::Transform&, const bool& createNew = true );				

			protected:

				GroupPtr																	_root;				
				GroupPtr																	_parent;

				GroupPtrs																	_groups;
		
		
				virtual	~SceneNodeVisitor();
		};
	}
}

#endif // __SCENE_SCENENODEVISITOR_H__

