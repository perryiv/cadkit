
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

#include "Helios/Plugins/MeasureTool/MeasureToolComponent.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Components/Factory.h"
#include "Usul/Documents/Document.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Interfaces/IArcGenReaderWriter.h"
#include "Usul/Interfaces/IIntersectNotify.h"
#include "Usul/Interfaces/IButtonPressSubject.h"
#include "Usul/Interfaces/IButtonID.h"
#include "Usul/Interfaces/IMouseEventSubject.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IViewMatrix.h"
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
#include "osg/MatrixTransform"
#include "osg/LineWidth"

#include "osgGA/GUIEventAdapter"

#include "osgText/Text"

#include "osgUtil/IntersectVisitor"

#include <algorithm>
#include <iostream>

//#define COMPILE_FOR_VRV 1

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
  case Usul::Interfaces::IMouseEventListener::IID:
    return static_cast <Usul::Interfaces::IMouseEventListener*> ( this );
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

#if COMPILE_FOR_VRV
    VRV::Interfaces::INavigationScene::QueryPtr ns ( oldView );
    if ( ns.valid() )
      OsgTools::Group::removeAllOccurances ( _root.get(), ns->navigationScene() );
#else
    Usul::Interfaces::IGroup::QueryPtr g ( oldView );
    if ( g.valid() )
    {
      osg::ref_ptr<osg::Group> group ( g->getGroup ( Detail::MEASURE_TOOL_GROUP ) );
      OsgTools::Group::removeAllOccurances ( _root.get(), group.get() );
    }

    Usul::Interfaces::IMouseEventSubject::QueryPtr mes ( oldView );
    if ( mes.valid() )
      mes->removeMouseEventListener ( me );
#endif
  }

  // Add our selfs to the new view.
  {
    Usul::Interfaces::IIntersectNotify::QueryPtr in ( newView );
    if ( in.valid() )
      in->addIntersectListener ( me );

    Usul::Interfaces::IButtonPressSubject::QueryPtr bls ( newView );
    if ( bls.valid() )
      bls->addButtonPressListener ( me );

#if COMPILE_FOR_VRV
    VRV::Interfaces::INavigationScene::QueryPtr ns ( newView );
    if ( ns.valid() )
      ns->navigationScene()->addChild ( _root.get() );
#else
    Usul::Interfaces::IGroup::QueryPtr g ( newView );
    if ( g.valid() )
    {
      osg::ref_ptr<osg::Group> group ( g->getGroup ( Detail::MEASURE_TOOL_GROUP ) );
      group->addChild ( _root.get() );
    }

    Usul::Interfaces::IMouseEventSubject::QueryPtr mes ( newView );
    if ( mes.valid() )
      mes->addMouseEventListener ( me );
#endif
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
    (*p)[0] = osg::Vec3 ( 0.0, 0.0, 0.0 );
    (*p)[1] = end - start;

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

    const double distance ( ( start - end ).length() );
    osg::ref_ptr<osgText::Text> text ( new osgText::Text );
    text->setText ( Usul::Strings::format ( distance ) );
    text->setColor( osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) );
    text->setPosition ( (*p)[1] / 2.0 );
    text->setAutoRotateToScreen( true );
    text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    text->setCharacterSize( 60.0 );
    geode->addDrawable ( text.get() );
    
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( start ) );
    mt->addChild ( geode.get() );

    return mt.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the observer of the intersection.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this->mutex() );

  if ( _appendPosition )
  {
    osg::Vec3 position ( hit.getWorldIntersectPoint() );

    // Add a sphere.
    if ( _root.valid() )
    {
#if COMPILE_FOR_VRV
      Usul::Interfaces::IViewMatrix::QueryPtr vm ( caller );
      if ( vm.valid() )
        position = position * osg::Matrix::inverse ( vm->getViewMatrix() );
#endif

      // Make the auto transform.
      osg::ref_ptr< osg::AutoTransform > autoTransform ( new osg::AutoTransform );

      autoTransform->setPosition ( position );
      autoTransform->setAutoScaleToScreen ( true );

      // Set the normalize state to true, so when the sphere size changes it still looks correct.
      OsgTools::State::StateSet::setNormalize ( autoTransform.get(), true );

      osg::ref_ptr <osg::Geode> geode ( new osg::Geode );
      geode->addDrawable ( OsgTools::ShapeFactory::instance().sphere( 2.5 ) );

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
      
      // Request a redraw.
      Usul::Interfaces::IDocument::QueryPtr doc ( Usul::Documents::Manager::instance().activeDocument() );
      if ( doc.valid() )
        doc->requestRedraw();
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
  
  measure->append ( new Button ( Usul::Commands::genericCommand ( "Export Line", 
      Usul::Adaptors::bind1<void> ( caller, Usul::Adaptors::memberFunction<void> ( this, &MeasureToolComponent::_exportLine ) ),
      Usul::Adaptors::memberFunction<bool> ( this, &MeasureToolComponent::enableExportButton ) ) ) );

	measure->append ( new Button ( Usul::Commands::genericCommand ( "Export Line Segments", 
      Usul::Adaptors::bind1<void> ( caller, Usul::Adaptors::memberFunction<void> ( this, &MeasureToolComponent::_exportLineSegments ) ),
      Usul::Adaptors::memberFunction<bool> ( this, &MeasureToolComponent::enableExportButton ) ) ) );
  
  menu->append ( measure );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the state to decide whether or not to enable/disable the export menu.
//
///////////////////////////////////////////////////////////////////////////////

bool MeasureToolComponent::enableExportButton() const
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the measure state.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::measureOn ( bool b )
{
  Guard guard ( this->mutex() );

  _measure = b;
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

void MeasureToolComponent::_updateMeasurement ( Usul::Interfaces::IUnknown *caller )
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

  // Set the internal measurement distance
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
//  Export to file.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::_export ( const std::string& key, const std::string& value, Usul::Interfaces::IUnknown* caller )
{
	Guard guard ( this->mutex() );

  // Helpful typedefs.
  typedef Usul::Documents::Manager                     DocManager;
  typedef DocManager::DocumentInfo                     Info;
	typedef Usul::Interfaces::ISaveFileDialog            ISaveFileDialog;
	typedef ISaveFileDialog::Filters                     Filters;
	typedef ISaveFileDialog::Filter                      Filter;
	typedef ISaveFileDialog::FileResult                  FileResult;
  typedef Usul::Interfaces::IArcGenReaderWriter        IArcGenReaderWriter;

	// Default filename.
	const std::string filename ( "output" + Usul::Functions::GUID::generate() + ".shp" );

	// The filters.
	Filters filters;
	filters.push_back ( Filter ( "ESRI ShapeFile (*.shp)", "*.shp" ) );

	// Query for the dialog.
	ISaveFileDialog::QueryPtr dialog ( caller );

	// Get the filename.
	FileResult result ( dialog.valid() ? dialog->getSaveFileName ( "Save Measurement", filters ) : FileResult ( filename, Filter() ) );

  // Find a document that can save the file..
	Info info ( DocManager::instance().find ( result.first, caller, false, true ) );

	// Get the document.
	Usul::Documents::Document::RefPtr document ( info.document );

	// Make sure it's valid.
  if ( false == document.valid() )
    throw std::runtime_error ( "Error 1845732421: Failed to find a matching document for file: " + result.first );

  // See if it can write the file.
  if ( false == document->canSave ( result.first ) )
    throw std::runtime_error ( "Error 4094644228: " + filename + " can't write to the specified extension." );

  // Get the interface.
  Usul::Interfaces::IArcGenReaderWriter::QueryPtr writer ( document );
  if ( false == writer.valid() )
    throw std::runtime_error ( "Error 3075911574: Invalid document for file: " + result.first );

  // Set the measurement and positions.
  writer->measurement ( _measurement );
  writer->setPolyLineVertices ( _positions );

	// Add the option.
	document->setOption ( key, value );

  // Write the file.
  document->write ( result.first, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export to entire line to file.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::_exportLine ( Usul::Interfaces::IUnknown *caller )
{
  this->_export ( "expand_geometry", "false", caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export line segments to file.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::_exportLineSegments ( Usul::Interfaces::IUnknown *caller )
{
	this->_export ( "expand_geometry", "true", caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when mouse event occurs.
//
///////////////////////////////////////////////////////////////////////////////

void MeasureToolComponent::mouseEventNotify ( osgGA::GUIEventAdapter& ea, Usul::Interfaces::IUnknown * caller )
{
  // See if it's the left button.
  const bool left ( Usul::Bits::has ( ea.getButton(), osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) );

  if ( left && osgGA::GUIEventAdapter::PUSH == ea.getEventType() && this->isMeasureOn() )
  {
    Guard guard ( this->mutex() );
    _appendPosition = true;
  }
}
