#ifndef _menukit_osg_tilemason_h_
#define _menukit_osg_tilemason_h_

// menukit includes
#include "../Visitor.h"           // the base class
#include "../WidestItemFinder.h"  // uses the graphics to find the widest menu Item
#include "../Stack.h"

#include "ColorThemeSkinTile.h"   // included for typedef

// osg includes
#include "osg/Group"
#include "osg/MatrixTransform"

// std includes
#include <numeric>  // for accumulate(...)

#include "OsgTools/Box.h" // TODO: take this out!
namespace MenuKit
{

  namespace OSG
  {
    /**
      * TileMason: a class that uses a tile to produce a menu consisting
      * of multiple menu item tiles.
      * 
      * The main goal of the Mason is to use a tile to obtain size
      * information so that each item can be placed adjacent to other items.
      *
      * A secondary goal is to configure the tile for display attributes.
      */
    template<class TileType>
    class TileMason : public Visitor
    {
    public:
      TileMason(): Visitor(Visitor::EXPANDED), _tile(), _hori(), _vert(), _scene(0x0) {}

      MENUKIT_DECLARE_POINTER(TileMason);

      osg::Group* scene() { return _scene.release(); }

      void tile(const TileType& t) { _tile = t; }
      const TileType& tile() const { return _tile; }
      TileType& tile() { return _tile; }

      virtual void apply(Menu&);
      virtual void apply(Button&);
      // TODO, remove Button and Menu classes,then: virtual void apply(Item&);

    protected:
      virtual ~TileMason() {}

      // TODO: clean up all these overloaded functions
      // TODO: once the Item class has been rewritten
      // TODO: and the Menu and Button classes have been removed
      osg::Vec3 displacement(const Menu& item);
      osg::Vec3 displacement(const Button& item);
      void determine_push(const Menu* parent,const Menu& menu,const Detail::Box& box);
      void push(const Menu* parent, const Detail::Box& box);
      void pop(const Menu& menu);
      void determine_pop(const Menu* parent, const Menu& menu);

    private:
      TileType _tile;
      Detail::Stack<float> _hori, _vert;
      osg::ref_ptr<osg::Group> _scene;
    };

  typedef TileMason<osgTile> osgTileMason;
  };

};

// ---- implementation ---- //

using namespace MenuKit;
using namespace OSG;

template<class TileType>
void TileMason<TileType>::determine_push(const Menu* parent,
                                         const Menu& menu,
                                         const Detail::Box& box)
{
  if( !parent )
    return;

  if( parent->layout() == Menu::HORIZONTAL )
  {
    if( menu.layout() == Menu::VERTICAL )
      _vert.push( box.height() );
    else
      _hori.push( box.width() );
  }

  else
  {
    _hori.push( box.width() );
  }
}

template<class TileType>
void TileMason<TileType>::determine_pop(const Menu* parent,const Menu& menu)
{
  if( !parent )
    return;

  if( parent->layout() == Menu::HORIZONTAL )
  {
    if( menu.layout() == Menu::VERTICAL )
      _vert.pop();
    else
      _hori.pop();
  }

  else
    _hori.pop();
}

template<class TileType>
void TileMason<TileType>::push(const Menu* parent, const Detail::Box& box)
{
  if( parent->layout() == Menu::HORIZONTAL )
    _hori.push(box.width());
  else
    _vert.push(box.height());
}

template<class TileType>
void TileMason<TileType>::pop(const Menu& parent)
{
  if( parent.layout() == Menu::HORIZONTAL )
    for(Menu::const_iterator iter=parent.begin(); iter!=parent.end(); ++iter)
      _hori.pop();

  else
    for(Menu::const_iterator iter=parent.begin(); iter!=parent.end(); ++iter)
      _vert.pop();
}

template<class TileType>
osg::Vec3 TileMason<TileType>::displacement(const Menu& m)
{
  // TODO: check if item is a SEPARATOR
  osg::Vec3 dv( _tile.width(m), _tile.box().height(), 0.0 );  // TODO: y=_tile.height(m)
  return dv;
}

template<class TileType>
osg::Vec3 TileMason<TileType>::displacement(const Button& b)
{
  // TODO: check if item is a SEPARATOR
  osg::Vec3 dv( _tile.width(b), _tile.box().height(), 0.0 );  // TODO: y=_tile.height(m)
  return dv;
}

template<class TileType>
void OSG::TileMason<TileType>::apply(Menu& m)
{
  // manage the width of the item
  Menu* parent = m.parent();
  if( !parent )  // no parent
  {
    _scene = new osg::Group();
    OsgTools::ColorBox cb(0.2,0.2,0.2);  // TODO: take this out
    cb.color_policy().color( osg::Vec4(1.0,0.0,0.0,1.0) );  // TODO: take this out
    _scene->addChild( cb() );  // TODO: take this out

    // escape when no parent and no text
    if( m.text().empty() )  // do not generate a graphic
    {                       // do not waste time with the remainder of this method
      traverse( m );        // just move onto traversal
      if( m.expanded() )    // this pop is not really necessary, just overly cautious
        pop( m );
      return;
    }

    // no parent means assume a horizontal style for the item's graphic
    _tile.box().width( _tile.width( m ) );
  }

  Menu::Layout pl = parent->layout();
  if( pl == Menu::HORIZONTAL )
  {
    float width = _tile.width( m );
    _tile.box().width( width );
  }

  else  // parent menu is VERTICAL
  {
    // TODO: determine if this can be pulled out
    // find the widest possible graphic
    float widest(0.0);
    for(Menu::const_iterator iter=parent->begin(); iter!=parent->end(); ++iter)
    {
      float tempwidth = _tile.width( (*iter).get() );
      if( tempwidth > widest )
        widest = tempwidth;
    }
    _tile.box().width( widest );
  }

  // TODO: configure the tile if necessary for different themes
  // TODO: by telling the tile what mode to be in.
  // TODO: change this default command:
  if( m.expanded() )
    _tile.mode( TileType::HIGHLIGHT );

  else
    _tile.mode( TileType::NORMAL );

  //if( m.disabled() )   // TODO: shouldn't there be a disabled flag in the Item class??
  //  _tile.mode( TileType::DISABLED );

  // create the graphic
  osg::ref_ptr<osg::Node> node = _tile( m );

  // calculate the position of the graphic
  osg::Vec3 sum(std::accumulate(_hori.data().begin(),_hori.data().end(),0.0f),
               -std::accumulate(_vert.data().begin(),_vert.data().end(),0.0f),
                0.0f);

  // add the graphic to the scene, with the correct position
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->addChild( node.get() );
  mt->setMatrix( osg::Matrix::translate(sum) );
  _scene->addChild( mt.get() );

  // save this menu's box state
  Detail::Box menubox = _tile.box();

  // manage placement of items
  if( m.expanded() )
    determine_push( parent, m , _tile.box() );

  traverse( m );  // the _tile's box state will change

  if( m.expanded() )
  {
    pop( m );
    determine_pop( parent , m );
  }

  if( parent )
    push( parent , menubox );  // use original box size
}

template<class TileType>
void TileMason<TileType>::apply(Button& b)
{
  // manage the width of the item
  Menu* parent = b.parent();

  if( parent )
  {
    Menu::Layout pl = parent->layout();

    if( pl == Menu::HORIZONTAL )
    {
      float width = _tile.width( b );
      _tile.box().width( width );
    }

    else  // parent menu is VERTICAL
    {
      // TODO: determine if this can be pulled out
      // find the widest possible graphic
      float widest(0.0);
      for(Menu::const_iterator iter=parent->begin(); iter!=parent->end(); ++iter)
      {
        float tempwidth = _tile.width( (*iter).get() );
        if( tempwidth > widest )
          widest = tempwidth;
      }
      _tile.box().width( widest );
    }
  }

  else // no parent
  {
    _scene = new osg::Group();
    OsgTools::ColorBox cb(0.3,0.3,0.3);  // TODO: take this out
    cb.color_policy().color( osg::Vec4(0.0,1.0,0.0,1.0) );  // TODO: take this out
    _scene->addChild( cb() );  // TODO: take this out
    _tile.box().width( _tile.width( b ) );
  }

  // TODO: configure the tile if necessary for different themes
  // TODO: by telling the tile what mode to be in.
  // TODO: change this default command:
  _tile.mode( TileType::NORMAL );

  // create the graphic
  osg::ref_ptr<osg::Node> node = _tile( b );

  // calculate the position of the graphic
  float xsum = std::accumulate(_hori.data().begin(),_hori.data().end(),0.0f);
  float ysum = std::accumulate(_vert.data().begin(),_vert.data().end(),0.0f);
  osg::Vec3 sum( xsum, -ysum, 0.0f);

  // add the graphic to the scene, with the correct position
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->addChild( node.get() );
  mt->setMatrix( osg::Matrix::translate(sum) );
  _scene->addChild( mt.get() );

  if( parent )
    push( parent, _tile.box() );
}

#endif
