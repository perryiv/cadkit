
#include "Group.h"

void Brahma::Scene::Group::children( const Brahma::Scene::Group::Children& children )
{
  _children = children;
}

Brahma::Scene::Group::Children Brahma::Scene::Group::children()
{
  return _children;
}

