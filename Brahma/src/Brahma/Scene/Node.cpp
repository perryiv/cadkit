
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "Node.h"
#include "NodeVisitor.h"

const std::string& Brahma::Scene::Node::id() const
{
  return _id;
}

void Brahma::Scene::Node::id( const std::string& id )
{
  _id = id;
}

//
//Brahma::Scene::LeafNode* Brahma::Scene::Node::database() 
//{
//  return _database.get();
//}
//
//void Brahma::Scene::Node::database( Brahma::Scene::LeafNode* db )
//{
//  _database = db;
//}
//

//void Brahma::Scene::Node::accept( Brahma::Scene::NodeVisitor &nv )
//{
//	nv.visit( *this );
//}

//const std::string& Brahma::Scene::Node::data() const 
//{
//  return _data;
//}
//
//void Brahma::Scene::Node::data( const std::string& source )
//{
//  _data = source;
//}
