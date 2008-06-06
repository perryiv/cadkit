
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The SelectionObject class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Fox/Plugins/SelectionSTL/SelectionObject.h"

#include "Helios/Fox/Views/Canvas.h"

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/ToolBar/ToggleButton.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/Cursor.h"

#include "OsgTools/Visitor.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/ISelectionBox.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ISetTool.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Matrix"
#include "osg/Plane"

using namespace Selection;
using namespace Selection::STL;

FXDEFMAP ( SelectionObject ) MessageMap[] = 
{
  //          Message Type     ID                              Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, SelectionObject::ID_SELECTION,  SelectionObject::onCommandSelection  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  SelectionObject::ID_SELECTION,  SelectionObject::onUpdateSelection   ),
};

FOX_TOOLS_IMPLEMENT ( SelectionObject, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SelectionObject , SelectionObject::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SelectionObject::SelectionObject() : BaseClass(),
_caller(),
_cornerOne ( 0.0, 0.0, 0.0 ),
_cornerTwo ( 0.0, 0.0, 0.0 ),
_selectionCursor ( FoxTools::Cursors::Factory::create ( FoxTools::Cursors::Factory::CURSOR_SELECTION ) ),
_frustum( new osg::Polytope )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SelectionObject::~SelectionObject()
{
  delete _frustum;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SelectionObject::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case ICommand::IID:
    return static_cast<ICommand*>(this);
 // case Usul::Interfaces::IFoxEvent::IID:
 //   return static_cast<Usul::Interfaces::IFoxEvent*>(this);
 // case Usul::Interfaces::ISetCursor::IID:
 //   return static_cast<Usul::Interfaces::ISetCursor*>(this);
  case Usul::Interfaces::IFoxToolbar::IID:
    return static_cast<Usul::Interfaces::IFoxToolbar*>(this);
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast<Usul::Interfaces::IMenuEntry*>(this);
  case Usul::Interfaces::ICleanUp::IID:
    return static_cast<Usul::Interfaces::ICleanUp*>(this);
#ifdef _DEBUG
  case Usul::Interfaces::IPlugin::IID:
    return static_cast<Usul::Interfaces::IPlugin*>(this);
#endif
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select vertices inside selection box
//
///////////////////////////////////////////////////////////////////////////////

void SelectionObject::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView( caller );
  
  if( activeView.valid() )
  {
    Usul::Interfaces::ISetTool::ValidQueryPtr setTool ( activeView->getActiveView() );

    setTool->setTool ( this );
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Function for handling selection
//  If left mouse button down - define first point of square
//  if drag, define second point, draw new square
//  if left mouse button up - define second point and do something
//
///////////////////////////////////////////////////////////////////////////////

bool SelectionObject::execute( Usul::Interfaces::IUnknown* caller, const FX::FXEvent &event)
{
  //Get the needed interfaces
  Usul::Interfaces::ISelectionBox::ValidQueryPtr selectionBox ( caller );
  Usul::Interfaces::IGetHeight::ValidQueryPtr getHeight ( caller );
  Usul::Interfaces::IRedraw::ValidQueryPtr redraw ( caller );

  if(event.type == FX::SEL_LEFTBUTTONPRESS )
  {
    _cornerOne[0] = event.click_x;
    _cornerOne[1] = getHeight->getHeightInt() - event.click_y;

    _cornerTwo[0] = event.click_x;
    _cornerTwo[1] = getHeight->getHeightInt() - event.click_y;

    //draw the selection box
    selectionBox->drawSelectionBox (_cornerOne, _cornerTwo );

    redraw->redraw();
  }
  else if ( event.type == FX::SEL_MOTION && Usul::Bits::has<int, int> ( event.state, FX::LEFTBUTTONMASK ) )
  {
    _cornerTwo[0] = event.last_x;
    _cornerTwo[1] = getHeight->getHeightInt() - event.last_y;

    //draw the selection box
    selectionBox->drawSelectionBox( _cornerOne, _cornerTwo );

    redraw->redraw();
  }
  else if ( event.type == FX::SEL_LEFTBUTTONRELEASE )
  {
    Usul::Interfaces::IOpenSceneGraph::ValidQueryPtr osg ( caller );
    // Get the scene.
    osg::ref_ptr<osg::Node> model ( dynamic_cast < osg::Node * > ( osg->osgReferenced() ) );

    _cornerTwo[0] = event.last_x;
    _cornerTwo[1] = getHeight->getHeightInt() - event.last_y;

    if ( Helios::Views::View *view = dynamic_cast< Helios::Views::View* > ( caller ) )
    {
      osg::Matrix M ( view->viewer()->getViewMatrix() );
      osg::Matrix P ( view->viewer()->getProjectionMatrix() );
      osg::Matrix W ( view->viewer()->getViewport()->computeWindowMatrix() );
      osg::Matrix MVPW ( M * P * W );
      osg::Matrix IMVPW ( osg::Matrix::inverse ( MVPW ) );

      //_cornerOne[2] = 0.0;
      //_cornerTwo[2] = 1.0;

      osg::Vec3 nearOne   ( _cornerOne[0], _cornerOne[1], 0.0 );
      osg::Vec3 farOne    ( _cornerOne[0], _cornerOne[1], 1.0 );
      osg::Vec3 nearThree ( _cornerTwo[0], _cornerTwo[1], 0.0 );
      osg::Vec3 farThree  ( _cornerTwo[0], _cornerTwo[1], 1.0 );

      nearOne = nearOne * IMVPW;
      farOne = farOne * IMVPW;
      nearThree = nearThree * IMVPW;
      farThree = farThree * IMVPW;

      osg::Vec3 nearTwo ( nearOne[0], nearThree[1], nearOne[2] );
      osg::Vec3 nearFour ( nearThree[0], nearOne[1], nearOne[2] );
      osg::Vec3 farTwo ( farOne[0], farThree[1], farOne[2] );
      osg::Vec3 farFour ( farThree[0], farOne[1], farOne[2] );

      //_cornerOne = _cornerOne * IMVPW;
      //_cornerTwo = _cornerTwo * IMVPW;

      _frustum->clear();

      osg::Plane planeOne ( nearOne, nearTwo, nearThree );
      osg::Plane planeTwo ( nearOne, farOne, farTwo );
      osg::Plane planeThree ( nearTwo, farTwo, farThree );
      osg::Plane planeFour ( nearThree, farThree, farFour );
      osg::Plane planeFive ( nearFour, farFour, farOne );
      osg::Plane planeSix ( farOne, farTwo, farThree );

      _frustum->add( planeOne );
      _frustum->add( planeTwo );
      _frustum->add( planeThree );
      _frustum->add( planeFour );
      _frustum->add( planeFive );
      _frustum->add( planeSix );

      osg::ref_ptr <osg::Geode> geode ( new osg::Geode );
      osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

      osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
      //near square
      vertices->push_back ( nearOne );
      vertices->push_back ( nearTwo );
      vertices->push_back ( nearTwo );
      vertices->push_back ( nearThree );
      vertices->push_back ( nearThree );
      vertices->push_back ( nearFour );
      vertices->push_back ( nearFour );
      vertices->push_back ( nearOne );
      //far square
      vertices->push_back ( farFour );
      vertices->push_back ( farThree );
      vertices->push_back ( farThree );
      vertices->push_back ( farTwo );
      vertices->push_back ( farTwo );
      vertices->push_back ( farOne );
      vertices->push_back ( farOne );
      vertices->push_back ( farFour );
      //connect squares
      vertices->push_back ( nearOne );
      vertices->push_back ( farOne );
      vertices->push_back ( nearTwo );
      vertices->push_back ( farTwo );
      vertices->push_back ( nearThree );
      vertices->push_back ( farThree );
      vertices->push_back ( nearFour );
      vertices->push_back ( farFour );


      geometry->setVertexArray ( vertices.get() );
      geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );

      geode->addDrawable( geometry.get() );
      geode->setName( "test" );

      osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
      modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      modelview_abs->setMatrix(osg::Matrix::identity());
      modelview_abs->addChild( geode.get() );
      
      view->scene()->addChild( modelview_abs );
    }

    // Declare the visitor.
    osg::ref_ptr<osg::NodeVisitor> geodeVisitor ( 
      OsgTools::MakeVisitor<osg::Geode>::make ( 
        Usul::Adaptors::memberFunction ( this, &SelectionObject::_selectionVerticesGeode ) ) );

    //model->accept( *geodeVisitor );

    //remove the selection box
    selectionBox->removeSelectionBox();

    redraw->redraw();

    //We are done
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SelectionObject::menuText() const
{
  return "Selection STL";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SelectionObject::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SelectionObject::statusBarText() const
{
  return "Select portion of STL file";
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the group for this menu entry.
//
///////////////////////////////////////////////////////////////////////////////

std::string   SelectionObject::menuGroup() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertices of this geode that are in the selection box
//
///////////////////////////////////////////////////////////////////////////////

void SelectionObject::_selectionVerticesGeode( osg::Geode *geode )
{
  if(geode->getName() == "Selection Box" || geode->getName() == "test")
    return;
  //loop through the drawables
  unsigned int numDrawables ( geode->getNumDrawables() );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    osg::Drawable *drawable = geode->getDrawable ( i );

    // See if the drawable is a geometry.
    osg::Geometry *geometry = drawable->asGeometry();
    if(geometry)
    {
      const osg::Vec3Array *vertices = dynamic_cast< const osg::Vec3Array*> ( geometry->getVertexArray() );
      const osg::Vec3Array *normals = geometry->getNormalArray();

      osg::ref_ptr < osg::Vec3Array > newVertices ( new osg::Vec3Array ( ));
      osg::ref_ptr < osg::Vec3Array > newNormals  ( new osg::Vec3Array ( ) );

      newVertices->reserve( vertices->size() );
      newNormals->reserve( normals->size() );

      const unsigned int numTriangles ( normals->size() );

      for(unsigned int j = 0; j < numTriangles; ++j)
      {
        const osg::Vec3 v1 ( vertices->at ( ( j * 3 )     ) );
        const osg::Vec3 v2 ( vertices->at ( ( j * 3 ) + 1 ) );
        const osg::Vec3 v3 ( vertices->at ( ( j * 3 ) + 2 ) );

        if(!_containsTriangle ( v1, v2, v3 ) )
        {
          newVertices->push_back( v1 );
          newVertices->push_back( v2 );
          newVertices->push_back( v3 );
          newNormals->push_back ( normals->at ( j ) );
        }
      }

      // Set vertices and normals.
      geometry->setVertexArray ( newVertices.get() );
      geometry->setNormalArray ( newNormals.get() );

      osg::ref_ptr<osg::DrawArrays> draw ( dynamic_cast < osg::DrawArrays * > ( geometry->getPrimitiveSet ( 0 ) ) );
      draw->setCount ( newVertices->size() );

    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the point lay within the selection box?
//
///////////////////////////////////////////////////////////////////////////////

bool SelectionObject::_containsPoint( const osg::Vec3& v )
{
  //return ( (_cornerOne < v) && (v < _cornerTwo) );//(_cornerOne[0] < v[0]) && (v[0] < _cornerTwo[0]) && (_cornerOne[1] < v[1]) && (v[1] < _cornerTwo[1]));
  return _frustum->contains ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the selection box contain this triangle.
//
///////////////////////////////////////////////////////////////////////////////

bool SelectionObject::_containsTriangle ( const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3 )
{
  return (_containsPoint(v1) || _containsPoint(v2) || _containsPoint(v3));
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't remember what this is suppose to do.
//
///////////////////////////////////////////////////////////////////////////////

void SelectionObject::_setCorners()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Selection button pressed.
//
///////////////////////////////////////////////////////////////////////////////

long SelectionObject::onCommandSelection  ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  if ( activeView.valid() &&  activeView->getActiveView() )
  {
    Usul::Interfaces::ISetTool::ValidQueryPtr setTool ( activeView->getActiveView() );
    bool mode ( setTool->getTool() == this );
    mode ? setTool->doneTool() : setTool->setTool( this );
  }
  
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the selection toolbar
//
///////////////////////////////////////////////////////////////////////////////

long SelectionObject::onUpdateSelection   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXToggleButton *toggle = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if( 0x0 == toggle )
    return 0;

  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  Usul::Interfaces::ISetTool::QueryPtr setTool ( activeView->getActiveView() );

  if( activeView.valid() && activeView->getActiveView() && setTool.valid() )
  {
    toggle->enable();
    bool result ( setTool->getTool() == this );
    toggle->setState ( ( result ) ? FX::STATE_CHECKED : FX::STATE_UNCHECKED );
  }
  else
    toggle->disable();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add selection button to the toolbar
//
///////////////////////////////////////////////////////////////////////////////

void SelectionObject::addButtons( Usul::Interfaces::IUnknown* caller, FoxTools::ToolBar::Bar* toolbar)
{
  _caller = caller;

  typedef FoxTools::Icons::Factory Icons;
  typedef FoxTools::ToolBar::ToggleButton ToggleButton;
  toolbar->append ( new ToggleButton ( "Selection", "Selection Triangles.", Icons::ICON_TOOL_SELECTION,  this, ID_SELECTION ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the selection cursor
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor* SelectionObject::getCursor()
{
  return _selectionCursor.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the selection box
//
///////////////////////////////////////////////////////////////////////////////

void SelectionObject::cleanUp( Usul::Interfaces::IUnknown* caller )
{
  Usul::Interfaces::ISelectionBox::ValidQueryPtr selectionBox ( caller );
  Usul::Interfaces::IRedraw::ValidQueryPtr redraw ( caller );

  selectionBox->removeSelectionBox();
  redraw->redraw();
}


