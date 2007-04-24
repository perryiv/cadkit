
#include "Group.h"

const Brahma::Scene::Group::Children& Brahma::Scene::Group::children() const
{
  return _children;
}

void Brahma::Scene::Group::children( const Brahma::Scene::Group::Children& children )
{
  _children = children;
}

