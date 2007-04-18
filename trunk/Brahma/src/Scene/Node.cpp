
#include "Node.h"

const std::string& Brahma::Scene::Node::id() const
{
  return _id;
}

void Brahma::Scene::Node::id( const std::string& id )
{
  _id = id;
}

const std::string& Brahma::Scene::Node::data() const 
{
  return _data;
}

void Brahma::Scene::Node::data( const std::string& source )
{
  _data = source;
}
