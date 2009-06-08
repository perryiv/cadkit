
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/QtCustomGeometry/AddGeometryWidget.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Model.h"
#include "Minerva/Interfaces/IAddLayer.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/ShapeFactory.h"
#include "OsgTools/Torus.h"
#include "OsgTools/Box.h"
#include "OsgTools/Callbacks/SortBackToFront.h"

#include "QtTools/Color.h"

#include "QtGui/QButtonGroup"

#include "osg/Geode"

#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Constant primitive id's.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const unsigned int SPHERE = 0;
  const unsigned int TORUS = 1;
  const unsigned int BOX = 2;
  const unsigned int SQUARE = 3;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddGeometryWidget::AddGeometryWidget ( Usul::Interfaces::IUnknown* caller, QWidget *parent ) : BaseClass ( parent ),
  _group ( new QButtonGroup )
{
  this->setupUi ( this );

  _group->addButton ( _sphereRadioButton, Detail::SPHERE );
  _group->addButton ( _torusRadioButton, Detail::TORUS );
  _group->addButton ( _boxRadioButton, Detail::BOX );
  _group->addButton ( _squareRadioButton, Detail::SQUARE );

  // Not implemented yet.
  _squareRadioButton->setEnabled ( false );

  // Sphere is the default.
  _sphereRadioButton->setChecked ( true );

  // Set ranges for spin boxes.
  _radiusDoubleSpinBox->setRange ( 0.0, std::numeric_limits<float>::max() );
  _innerRadiusDoubleSpinBox->setRange ( 0.0, std::numeric_limits<float>::max() );
  _outerRadiusDoubleSpinBox->setRange ( 0.0, std::numeric_limits<float>::max() );
  _boxWidth->setRange ( 0.0, std::numeric_limits<float>::max() );
  _boxHeight->setRange ( 0.0, std::numeric_limits<float>::max() );
  _boxDepth->setRange ( 0.0, std::numeric_limits<float>::max() );
  _squareWidth->setRange ( 0.0, std::numeric_limits<float>::max() );
  _squareHeight->setRange ( 0.0, std::numeric_limits<float>::max() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddGeometryWidget::~AddGeometryWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply.
//
///////////////////////////////////////////////////////////////////////////////

void AddGeometryWidget::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );
  if ( false == al.valid() )
    return;

  // Make the geometry.
  Geometry::RefPtr geometry ( this->_makeGeometry() );

  // Return now if there isn't a valid geometry.
  if ( false == geometry.valid() )
    return;

  // Make a new data object.
  Minerva::Core::Data::DataObject::RefPtr object ( new Minerva::Core::Data::DataObject );

  // Set the name.
  object->name ( _name->text().toStdString() );

  // Add the geometry.
  object->addGeometry ( geometry );

  // Add the data object to the parent.
  al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( object ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a shpere.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Node* makeSphere ( double radius )
  {
    osg::ref_ptr<osg::Geometry> geometry ( OsgTools::ShapeFactory::instance().sphere ( radius ) );
    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
    geode->addDrawable ( geometry.get() );

    return geode.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a torus.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Node* makeTorus ( double innerRadius, double outerRadius )
  {
    OsgTools::Torus torus ( innerRadius, outerRadius );
    return torus();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a box.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct DoNothing
  {
    void operator() ( osg::Geometry* ) const
    {
    }
  };

  osg::Node* makeBox ( double w, double h, double d )
  {
    OsgTools::Box<DoNothing> box ( w, h, d );
    return box();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the geometry.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Geometry* AddGeometryWidget::_makeGeometry() const
{
  switch ( _group->checkedId() )
  {
  case Detail::SPHERE: return this->_makeSphere();
  case Detail::TORUS:  return this->_makeTorus();
  case Detail::BOX:    return this->_makeBox();
  case Detail::SQUARE: return this->_makeSquare();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a shpere.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Geometry* AddGeometryWidget::_makeSphere() const
{
  Minerva::Core::Data::Model::RefPtr model ( this->_makeModel() );

  const double radius ( _radiusDoubleSpinBox->value() );

  // Handle bad input.
  if ( 0.0 == radius )
    return 0x0;

  // Make the node.
  osg::ref_ptr<osg::Node> node ( Detail::makeSphere ( radius ) );

  // Set the material.
  this->_setMaterial ( node.get() );

  // Set the model.
  model->model ( node.get() );

  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a torus.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Geometry* AddGeometryWidget::_makeTorus() const
{
  Minerva::Core::Data::Model::RefPtr model ( this->_makeModel() );

  // Make the node.
  osg::ref_ptr<osg::Node> node ( Detail::makeTorus ( _innerRadiusDoubleSpinBox->value(), _outerRadiusDoubleSpinBox->value() ) );

  // Set the material.
  this->_setMaterial ( node.get() );

  // Set the model.
  model->model ( node.get() );

  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a box.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Geometry* AddGeometryWidget::_makeBox() const
{
  Minerva::Core::Data::Model::RefPtr model ( this->_makeModel() );

  // Make the node.
  osg::ref_ptr<osg::Node> node ( Detail::makeBox ( _boxWidth->value(), _boxHeight->value(), _boxDepth->value() ) );

  // Set the material.
  this->_setMaterial ( node.get() );

  // Set the model.
  model->model ( node.get() );

  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a square.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Geometry* AddGeometryWidget::_makeSquare() const
{
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material for the node.
//
///////////////////////////////////////////////////////////////////////////////

void AddGeometryWidget::_setMaterial ( osg::Node* node ) const
{
  const osg::Vec4f color ( QtTools::Color<osg::Vec4f>::convert ( _colorButton->color() ) );
  const float transparency ( static_cast<float> ( _transparencySlider->value() ) / 255 );
  OsgTools::State::StateSet::setMaterial ( node, color, color, transparency );

  if ( transparency < 1.0 && 0x0 != node )
    node->setCullCallback ( new OsgTools::Callbacks::SortBackToFront );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a model object.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Model* AddGeometryWidget::_makeModel() const
{
  Minerva::Core::Data::Model::RefPtr model ( new Minerva::Core::Data::Model );
  model->toMeters ( 1.0f );

  // Set the location.
  osg::Vec3 location ( _longDoubleSpinBox->value(), _latDoubleSpinBox->value(), 0.0 );
  model->location ( location );

  return model.release();
}
