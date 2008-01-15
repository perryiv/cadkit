
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "MeasureTool/MeasureToolComponent.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Components/Factory.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IIntersectNotify.h"
#include "Usul/Interfaces/IButtonPressSubject.h"
#include "Usul/Interfaces/IButtonID.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IArcGenReaderWriter.h"
#include "Usul/Strings/Format.h"

#include "VRV/Interfaces/INavigationScene.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/ShapeFactory.h"
#include "OsgTools/Group.h"

#include "osg/AutoTransform"
#include "osg/Geode"
#include "osg/Material"
#include "osg/LineWidth"


#include "osgUtil/IntersectVisitor"

#include <algorithm>
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Name for the group.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string MEASURE_TOOL_GROUP ( "measure_tool_group" );
}

USUL_DECLARE_COMPONENT_FACTORY ( MeasureToolComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MeasureToolComponent, MeasureToolComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MeasureToolComponent::MeasureToolComponent() : 
  BaseClass(),
  _measure ( false ),
  _appendPosition ( false ),
  _positions(),
  _view(),
  _root( new osg::Group )
{
  Usul::Documents::Manager::instance().addActiveViewListener ( this );

  _root->setName ( "measure_tool_group" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MeasureToolComponent::~MeasureToolComponent()
{
  Usul::Documents::Manager::instance().removeActiveViewListener ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MeasureToolComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast <Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IIntersectListener::IID:
    return static_cast <Usul::Interfaces::IIntersectListener*>(this);
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast <Usul::Interfaces::IMenuAdd*>(this);
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast <Usul::Interfaces::IActiveViewListener*>(this);
  case Usul::Interfaces::IButtonPressListener::IID:
    return static_cast <Usul::Interfaces::IButtonPressListener*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView )
{
  // Clear.
  this->_clear();

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Remove our selfs from the old view.
  {
    Usul::Interfaces::IIntersectNotify::QueryPtr in ( oldView );
    if ( in.valid() )
      in->removeIntersectListener ( me );

    Usul::Interfaces::IButtonPressSubject::QueryPtr bls ( oldView );
    if ( bls.valid() )
      bls->removeButtonPressListener ( me );

    VRV::Interfaces::INavigationScene::QueryPtr ns ( oldView );
    if ( ns.valid() )
      OsgTools::Group::removeAllOccurances ( _root.get(), ns->navigationScene() );
  }

  // Add our selfs to the new view.
  {
    Usul::Interfaces::IIntersectNotify::QueryPtr in ( newView );
    if ( in.valid() )
      in->addIntersectListener ( me );

    Usul::Interfaces::IButtonPressSubject::QueryPtr bls ( newView );
    if ( bls.valid() )
      bls->addButtonPressListener ( me );

    VRV::Interfaces::INavigationScene::QueryPtr ns ( newView );
    if ( ns.valid() )
      ns->navigationScene()->addChild ( _root.get() );
  }

  _view = newView;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a line segment.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Node* makeLineSegment ( const osg::Vec3& start, const osg::Vec3& end )
  {
    osg::ref_ptr <osg::Geode> geode ( new osg::Geode );

    osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
    osg::ref_ptr<osg::Vec3Array> p ( new osg::Vec3Array );

    p->resize ( 2 );
    (*p)[0] = start;
    (*p)[1] = end;

    geom->setVertexArray ( p.get() );
    geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, 2 ) );

    osg::ref_ptr<osg::Vec4Array> c ( new osg::Vec4Array );
    c->resize ( 2 );
    (*c)[0] = osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 );
    (*c)[1] = osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 );

    geom->setColorArray ( c.get() );
    geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

    osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
    lw->setWidth ( 2 );

    osg::ref_ptr<osg::StateSet> state = geom->getOrCreateStateSet();
    state->setAttribute ( lw.get() );
    state->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );

    geode->addDrawable ( geom.get() );
    
    return geode.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the observer of the intersection.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::intersectNotify ( float x, float y, const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this->mutex() );

  if ( _appendPosition )
  {
    osg::Vec3 position ( hit.getWorldIntersectPoint() );

    // Add a sphere.
    if ( _root.valid() )
    {
      Usul::Interfaces::IViewMatrix::QueryPtr vm ( caller );
      if ( vm.valid() )
        position = position * osg::Matrix::inverse ( vm->getViewMatrix() );

      // Make the auto transform.
      osg::ref_ptr< osg::AutoTransform > autoTransform ( new osg::AutoTransform );

      autoTransform->setPosition ( position );
      autoTransform->setAutoScaleToScreen ( true );

      // Set the normalize state to true, so when the sphere size changes it still looks correct.
      OsgTools::State::StateSet::setNormalize ( autoTransform.get(), true );

      osg::ref_ptr <osg::Geode> geode ( new osg::Geode );
      geode->addDrawable ( OsgTools::ShapeFactorySingleton::instance().sphere( 2.5 ) );

      autoTransform->addChild ( geode.get() );
      osg::ref_ptr <osg::Material>  mat ( new osg::Material );
      osg::Vec4 color ( 1.0, 1.0, 0.0, 1.0 );
      mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
      mat->setAmbient ( osg::Material::FRONT_AND_BACK, color );
      OsgTools::State::StateSet::setMaterial ( geode.get(), mat.get() );
      geode->getOrCreateStateSet()->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

      _root->addChild ( autoTransform.get() );

      if ( false == _positions.empty() )
        _root->addChild ( Detail::makeLineSegment ( position, _positions.back() ) );

    }

    // Add the position.
    _positions.push_back ( position );

    this->_updateMeasurement ( caller );

    _appendPosition = false;
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::menuAdd ( MenuKit::Menu& m, Usul::Interfaces::IUnknown* caller )
{
  // Typedefs.
  typedef MenuKit::Button Button;
  typedef MenuKit::ToggleButton ToggleButton;

  // Build the menu.
  MenuKit::Menu::RefPtr menu ( m.find ( "Tools", true ) );

  MenuKit::Menu::RefPtr measure ( new MenuKit::Menu ( "Measurement" ) );

  measure->append ( new ToggleButton ( Usul::Commands::genericToggleCommand ( "On", Usul::Adaptors::memberFunction<void> ( this, &MeasureToolComponent::measureOn ), Usul::Adaptors::memberFunction<bool> ( this, &MeasureToolComponent::isMeasureOn ) ) ) );
  measure->append ( new Button ( Usul::Commands::genericCommand ( "Clear", Usul::Adaptors::memberFunction<void> ( this, &MeasureToolComponent::_clear ), Usul::Commands::TrueFunctor() ) ) );
  
  // Menu Function to export the measurement points to an Arc Gen file
  measure->append ( new Button ( Usul::Commands::genericCommand ( "Export", Usul::Adaptors::memberFunction<void> ( this, &MeasureToolComponent::_exportToArcGen ), Usul::Adaptors::memberFunction<bool> ( this, &MeasureToolComponent::enableExportButton ) ) ) );
  
  menu->append ( measure );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the state to decide whether or not to enable/disable the export
//  menu.
//
///////////////////////////////////////////////////////////////////////////////

bool MeasureToolCommonent::enableExportButton() const
{
  return true;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the measure state.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::measureOn  ( bool b )
{
  Guard guard ( this->mutex() );

  _measure = b;

  this->_clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the measure state.
//
///////////////////////////////////////////////////////////////////////////////

bool MeasureToolComponent::isMeasureOn() const
{
  Guard guard ( this->mutex() );

  return _measure;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when button is released.
//
///////////////////////////////////////////////////////////////////////////////

bool MeasureToolComponent::buttonPressNotify ( Usul::Interfaces::IUnknown * caller )
{
  Guard guard ( this->mutex() );

  Usul::Interfaces::IButtonID::QueryPtr bi ( caller );
  if ( bi.valid() && 0x00000020 == bi->buttonID() && this->isMeasureOn() )
  {
    _appendPosition = true;
    return false;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the measurement.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::_updateMeasurement( Usul::Interfaces::IUnknown *caller ) const
{
  Guard guard ( this->mutex() );

  // Return if we don't have enough positions.
  if ( _positions.size() < 2 )
    return;

  double distance ( 0.0 );

  for ( Positions::size_type i = 1; i < _positions.size(); ++ i )
  {
    osg::Vec3 v ( _positions.at ( i ) - _positions.at ( i - 1 ) );
    distance += v.length();
  }

  Usul::Interfaces::ITextMatrix::QueryPtr tm ( caller );
  if ( tm.valid() )
  {
    tm->setText ( 15, 15, Usul::Strings::format (  "Distance: ", distance ), osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  }

  // set the internal measurement distance
  _measurement = distance;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::_clear()
{
  Guard guard ( this->mutex() );
  _positions.clear();

  Usul::Interfaces::ITextMatrix::QueryPtr tm ( _view );
  if ( tm.valid() )
  {
    tm->setText ( 15, 15, "", osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  }

  OsgTools::Group::removeAllChildren ( _root.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export to an Arc Gen format file.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::_exportToArcGen( Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this->mutex() );

  // This will create a new document.
  const std::string filename ( "output.gen" );
  Info info ( DocManager::instance().find ( filename, caller ) );

  if ( false == info.document.valid() )
    throw std::runtime_error ( "Error 1845732421: Failed to find a matching document for file: " + filename );

  // See if it can write the file.
  if ( false == info.document->canWrite ( filename ) )
    throw std::runtime_error ( "Error 4094644228: " + filename + " can't write to the specified extension .gen" );

  // Get the interface.
  Usul::Interfaces::IArcGenReaderWriter::QueryPtr writer ( info.document );
  if ( false == writer.valid() )
    throw std::runtime_error ( "Error 3075911574: Invalid document for file: " + filename );

  // set the measurement and positions in the ArcGen reader/writer
  writer->measurement( _measurement );
  writer->positions( _positions );

  // Tell the ArcGenRW to write the output file
  info.document->save ( filename );

}
