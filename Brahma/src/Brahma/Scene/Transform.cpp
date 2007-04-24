
#include "Transform.h"

const std::string& Brahma::Scene::Transform::type() const 
{
  return _type;
}

void Brahma::Scene::Transform::type( const std::string& type )
{
  _type = type;
}

const Usul::Math::Vec3f& Brahma::Scene::Transform::position() const
{
  return _position;
}

void Brahma::Scene::Transform::position( const Usul::Math::Vec3f& position )
{
  _position = position;
}

const Usul::Math::Vec4f& Brahma::Scene::Transform::rotation() const
{
  return _rotation;
}

void Brahma::Scene::Transform::rotation( const Usul::Math::Vec4f& rotation )
{
  _rotation = rotation;
}

const Usul::Math::Vec3f& Brahma::Scene::Transform::pivot() const
{
  return _pivot;
}

void Brahma::Scene::Transform::pivot( const Usul::Math::Vec3f& pivot )
{
  _pivot = pivot;
}







