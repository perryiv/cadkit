
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "Group.h"
#include "NodeVisitor.h"

const Brahma::Scene::Group::Children& Brahma::Scene::Group::children() const
{
  return _children;
}

void Brahma::Scene::Group::children( const Brahma::Scene::Group::Children& children )
{
  _children = children;
}

void Brahma::Scene::Group::accept( NodeVisitor& nv )
{
	nv.visit( *this );
}
