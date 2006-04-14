
#include "SceneTree.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/TreeList.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/MenuPane.h"
#include "FoxTools/Headers/MenuCommand.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/Vec4f.h"

#include "Usul/Interfaces/IGroup.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Bits/Bits.h"
#include "Usul/App/Controller.h"

#include "OsgTools/Box.h"
#include "OsgTools/Sphere.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Node"
#include "osg/Geode"
#include "osg/Group"
#include "osg/Drawable"
#include "osg/StateSet"
#include "osg/StateAttribute"
#include "osg/Material"
#include "osg/MatrixTransform"


namespace Detail
{
  void getState ( std::ostream& os, osg::StateSet *ss, osg::StateAttribute::GLMode mode )
  {
    if ( Usul::Bits::has ( ss->getMode ( mode ), osg::StateAttribute::ON ) )
    {
      os << " ON";
    }
    else if ( Usul::Bits::has ( ss->getMode ( mode ), osg::StateAttribute::OFF ) )
    {
      os << " OFF";
    }
    if ( Usul::Bits::has ( ss->getMode ( mode ), osg::StateAttribute::OVERRIDE ) )
    {
      os << " OVERRIDE";
    }
    if ( Usul::Bits::has ( ss->getMode ( mode ), osg::StateAttribute::PROTECTED ) )
    {
      os << " PROTECTED";
    }
    if ( Usul::Bits::has ( ss->getMode ( mode ), osg::StateAttribute::INHERIT ) )
    {
      os << " INHERIT";
    }
    
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( SceneTree ) MessageMap[] = 
{
  //          Message Type     ID                                   Message Handler.
  FXMAPFUNC ( FX::SEL_RIGHTBUTTONPRESS, 0,            SceneTree::onCommandRightClick     ),
  FXMAPFUNC ( FX::SEL_LEFTBUTTONPRESS,  0,            SceneTree::onCommandLeftClick     ),
  FXMAPFUNC ( FX::SEL_COMMAND,          SceneTree::ID_PROPERTIES,           SceneTree::onCommandProperties     ),
};

FOX_TOOLS_IMPLEMENT ( SceneTree, FX::FXTreeList, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneTree::SceneTree (FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
BaseClass( p, tgt, sel, opts, x,y,w,h ),
_nodes(),
_drawables(),
_selectedItem( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update this scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void SceneTree::update ( osg::Node *scene  )
{
  // Clear any nodes we may be saving.
  _nodes.clear();

  // Clear any drawables we may be saving
  _drawables.clear();

  // Clear the trees.
  this->clearItems();

  // Build them again.
  this->_buildSceneTree ( scene, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene-tree control.
//
///////////////////////////////////////////////////////////////////////////////

void SceneTree::_buildSceneTree ( osg::Node *node, FX::FXTreeItem *parent )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  // See if the node is a group.
  osg::Group *group = dynamic_cast < osg::Group * > ( node );
  if ( group )
  {
    parent = this->_addTreeItem ( group, parent );
    for ( unsigned int i = 0; i < group->getNumChildren(); ++i )
      this->_buildSceneTree ( group->getChild ( i ), parent );
    return;
  }

  // See if the node is a geode.
  osg::Geode *geode = dynamic_cast < osg::Geode * > ( node );
  if ( geode )
  {
    parent = this->_addTreeItem ( geode, parent );
    for ( unsigned int i = 0; i < geode->getNumDrawables(); ++i )
      this->_addTreeItem ( geode->getDrawable ( i ), parent );
    return;
  }

  // Otherwise...
  else
  {
    // If we get this far then it's a node.
    this->_addTreeItem ( node, parent );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tree item.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXTreeItem *SceneTree::_addTreeItem ( osg::Node *node, FX::FXTreeItem *parent )
{
  // Handle bad input.
  if ( 0x0 == node )
    return 0x0;

  // Get the item text.
  FX::FXString name ( ( node->getName().empty() ) ? "" : node->getName().c_str() );
  FX::FXString cn   ( node->className() );
  FX::FXString text ( ( name.empty() ) ? cn : cn + " (" + name + ")" );

  // Add the item.
  FX::FXTreeItem *item ( this->appendItem ( parent, text, 0x0, 0x0 ) );
  _nodes.insert ( Nodes::value_type ( item, node ) );
  return item;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tree item.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXTreeItem *SceneTree::_addTreeItem ( osg::Drawable *drawable, FX::FXTreeItem *parent )
{
  // Handle bad input.
  if ( 0x0 == drawable )
    return 0x0;

  // Add the item.
  FX::FXTreeItem *item ( this->appendItem ( parent, drawable->className(), 0x0, 0x0 ) );
  _drawables.insert( Drawables::value_type ( item, drawable ) );
  return item;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node-tree control.
//
///////////////////////////////////////////////////////////////////////////////

void SceneTree::_buildNodeTree ( osg::Node *node, FX::FXTreeItem *parent )
{
  // Handle bad input.
  if ( 0x0 == node || 0x0 == parent )
    return;

  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Right mouse button click.
//
///////////////////////////////////////////////////////////////////////////////

long SceneTree::onCommandRightClick ( FX::FXObject* object, FX::FXSelector sel, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  FX::FXTreeItem* item ( this->getCursorItem() );

  if ( item )
  {
    _selectedItem = item;

    FX::FXMenuPane *menu = new FX::FXMenuPane ( FoxTools::Functions::mainWindow() );
    //new FX::FXMenuCommand ( menu, "Show Bounds\t\t", 0x0, this, ID_SHOW_BOUNDING_BOX );
    new FX::FXMenuCommand ( menu, "Properties\t\t", 0x0, this, ID_PROPERTIES );
    menu->create();

    // Show the menu-pane.
    menu->popup ( 0x0, event->root_x, event->root_y );

    // Run a model loop until the menu is dismissed.
    FoxTools::Functions::application()->runModalWhileShown ( menu );

    _selectedItem = 0x0;

    delete menu;

  }

  return BaseClass::onRightBtnPress( object, sel, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show properties of a node or drawable.
//
///////////////////////////////////////////////////////////////////////////////

long SceneTree::onCommandProperties ( FX::FXObject* object, FX::FXSelector sel, void *data )
{
  osg::StateSet *ss ( 0x0 );

  FX::FXTreeItem* item ( _selectedItem );

  // Check to see if it's a node.
  Nodes::const_iterator nodeIter ( _nodes.find( item ) );
  if( nodeIter != _nodes.end() )
  {
    osg::Node* node ( nodeIter->second );
    ss = node->getOrCreateStateSet();
  }

  // Check to see if it's a drawable.
  Drawables::const_iterator drawableIter ( _drawables.find( item ) );
  if( drawableIter != _drawables.end() )
  {
    osg::Drawable* drawable ( drawableIter->second );
    ss = drawable->getOrCreateStateSet();
  }

  if( !ss )
    return 0;

  // Build dialog to show state set.
  FX::FXDialogBox  dialog ( FoxTools::Functions::mainWindow(), item->getText(), FX::DECOR_CLOSE|DECOR_TITLE|DECOR_BORDER );

  {
    std::ostringstream depth;
    depth << "GL_DEPTH_TEST: ";

    Detail::getState ( depth, ss, GL_DEPTH_TEST );

    new FX::FXLabel ( &dialog, depth.str().c_str() );
  }

  {
    std::ostringstream lighting;
    lighting << "GL_LIGHTING: ";

    Detail::getState ( lighting, ss, GL_LIGHTING );

    new FX::FXLabel ( &dialog, lighting.str().c_str() );
  }

  {
    std::ostringstream os;
    os << "GL_CULL_FACE: ";

    Detail::getState ( os, ss, GL_CULL_FACE );

    new FX::FXLabel ( &dialog, os.str().c_str() );
  }

  {
    std::ostringstream os;
    os << "GL_BLEND: ";

    Detail::getState ( os, ss, GL_BLEND );

    new FX::FXLabel ( &dialog, os.str().c_str() );
  }

  // Is there any material?
  osg::StateAttribute *attribute ( ss->getAttribute ( osg::StateAttribute::MATERIAL ) );
  if( attribute )
  {
    osg::Material *material = static_cast < osg::Material* > ( attribute );

    // Back colors.
    {
      new FX::FXLabel ( &dialog, "Back:" );
      osg::Vec4 ambient ( material->getAmbient( osg::Material::BACK ) );
      osg::Vec4 diffuse ( material->getDiffuse( osg::Material::BACK ) );
      osg::Vec4 specular ( material->getSpecular ( osg::Material::BACK ) );
      osg::Vec4 emission ( material->getEmission ( osg::Material::BACK ) );

      {
        std::ostringstream os;
        os << "Ambient: ";
        os << ambient[0] << " " << ambient[1] << " " << ambient[2] << " " << ambient[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( ambient[0], ambient[1], ambient[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Diffuse: ";
        os << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << " " << diffuse[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( diffuse[0], diffuse[1], diffuse[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Specular: ";
        os << specular[0] << " " << specular[1] << " " << specular[2] << " " << specular[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( specular[0], specular[1], specular[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Emission: ";
        os << emission[0] << " " << emission[1] << " " << emission[2] << " " << emission[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( emission[0], emission[1], emission[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Shininess: " << material->getShininess( osg::Material::BACK );
        new FX::FXLabel ( &dialog, os.str().c_str() );
      }
      
    }
    
    // Front colors.
    {
      new FX::FXLabel ( &dialog, "Front:" );

      osg::Vec4 ambient ( material->getAmbient( osg::Material::FRONT ) );
      osg::Vec4 diffuse ( material->getDiffuse( osg::Material::FRONT ) );
      osg::Vec4 specular ( material->getSpecular ( osg::Material::FRONT ) );
      osg::Vec4 emission ( material->getEmission ( osg::Material::FRONT ) );

      {
        std::ostringstream os;
        os << "Ambient: ";
        os << ambient[0] << " " << ambient[1] << " " << ambient[2] << " " << ambient[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( ambient[0], ambient[1], ambient[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Diffuse: ";
        os << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << " " << diffuse[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( diffuse[0], diffuse[1], diffuse[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Specular: ";
        os << specular[0] << " " << specular[1] << " " << specular[2] << " " << specular[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( specular[0], specular[1], specular[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Emission: ";
        os << emission[0] << " " << emission[1] << " " << emission[2] << " " << emission[3];
        FX::FXLabel *label ( new FX::FXLabel ( &dialog, os.str().c_str() ) );
        FX::FXVec4f color ( emission[0], emission[1], emission[2], 1.0 );
        label->setTextColor ( color );
      }

      {
        std::ostringstream os;
        os << "Shininess: " << material->getShininess( osg::Material::FRONT );
        new FX::FXLabel ( &dialog, os.str().c_str() );
      }
    }
  }
  else
  {
    new FX::FXLabel ( &dialog, "No Material" );
  }

  dialog.create();

  dialog.execute( FX::PLACEMENT_OWNER );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Left mouse button click.
//
///////////////////////////////////////////////////////////////////////////////

long SceneTree::onCommandLeftClick ( FX::FXObject* object, FX::FXSelector sel, void *data )
{
  FX::FXTreeItem* item ( this->getCurrentItem() );

  Usul::Interfaces::IViewer::QueryPtr viewer ( Usul::App::Controller::instance().activeView() );

  if( false == viewer.valid() )
    return 0;

  Usul::Interfaces::IGroup::QueryPtr group ( viewer );

  FX::FXEvent *event = (FX::FXEvent *) data;

  if( group.valid() )
  {

    group->removeGroup ( "Scene Tree Bounding Box" );

    if( 0 != this->hitItem( item, event->click_x, event->click_y ) )
    {
  
      osg::ref_ptr < osg::Group > scene ( group->getGroup ( "Scene Tree Bounding Box" ) );

      osg::BoundingBox bb;

      // Check to see if it's a node.
      Nodes::const_iterator nodeIter ( _nodes.find( item ) );
      if( nodeIter != _nodes.end() )
      {
        osg::Node* node ( nodeIter->second );
        
        if( osg::Geode *geode = dynamic_cast < osg::Geode* > ( node ) )
          bb.expandBy ( geode->getBound() );
        
          // Get the node's bounding box.
        else
          bb.expandBy ( node->getBound() );
      }

      // Check to see if it's a drawable.
      Drawables::const_iterator drawableIter ( _drawables.find( item ) );
      if( drawableIter != _drawables.end() )
      {
        osg::Drawable* drawable ( drawableIter->second );

        // Get the drawables's bounding box.
        bb.expandBy ( drawable->getBound() );
      }

      // Make a new blue box.
      OsgTools::ColorBox box ( bb );
      box.color_policy().color ( osg::Vec4 ( 0, 0, 1, 1 ) );

      // Position it.
      osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
      mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
      mt->addChild ( box() );

      // Wire-frame.
      OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
      OsgTools::State::StateSet::setLighting ( mt.get(), false );

      scene->addChild ( mt.get() );
  
    }

    viewer->render();

  }


  return BaseClass::onLeftBtnPress( object, sel ,data );
}

