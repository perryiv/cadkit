#ifndef _menukit_osg_tilelayer_
#define _menukit_osg_tilelayer_

// menukit includes
#include "../Visitor.h"           // the base class
#include "ColorThemeTile.h"       // included for the typedef
#include "../WidestItemFinder.h"  // uses the graphics to find the widest menu Item
#include "../Stack.h"
#include "../Accumulate.h"

// osg includes
#include "osg/Group"
#include "osg/MatrixTransform"

// std includes
//#include <stack>
//#include <algorithm>
#include <numeric>

namespace MenuKit
{

  namespace OSG
  {
    /**
      * TileLayer: a class that uses a
      * tile to produce a menu consisting
      * of multiple menu item tiles.
      */
    template<class TileType>
    class TileLayer : public Visitor
    {
    public:
      TileLayer(): Visitor(), _tile(), _hori(), _vert(), _scene(0x0) {}
      TileLayer(Visitor::Mode m): Visitor(m), _tile() {}

      MENUKIT_DECLARE_POINTER(TileLayer);

      osg::Group* scene() { return _scene.release(); }

      void tile(const TileType& t) { _tile = t; }
      const TileType& tile() const { return _tile; }
      TileType& tile() { return _tile; }

      virtual void apply(Menu&);
      virtual void apply(Button&);
      // TODO, remove Button and Menu classes,then: virtual void apply(Item&);

    protected:
      virtual ~TileLayer() {}

      // TODO: clean up all these overloaded functions
      // TODO: once the Item class has been rewritten
      // TODO: and the Menu and Button classes have been removed
      osg::Vec3 displacement(const Menu&);
      osg::Vec3 displacement(const Button&);
      void determine_push(const Menu&);
      void determine_push(const Button&);
      void determine_pop(const Menu&);

    private:
      TileType _tile;
      Detail::Stacker<float> _hori, _vert;
      osg::ref_ptr<osg::Group> _scene;
    };

  typedef TileLayer<ColorTile> ColorTileLayer;
  };

};

// ---- implementation ---- //

using namespace MenuKit;

template<class TileType>
void OSG::TileLayer<TileType>::determine_push(const Menu& m)
{
  const Menu* parent = m.parent();

  // TODO: make sure 'displacement' is done properly for this case
  osg::Vec3 del = displacement( m );
  if( parent )
  {
    // add to the stack
    if( parent->layout() == Menu::HORIZONTAL )
      _hori.push( del[0] );
    else
      _vert.push( del[1] );
  }

  else
  {
    _hori.push( del[0] );
    _vert.push( del[1] );
  }

}

template<class TileType>
void OSG::TileLayer<TileType>::determine_push(const Button& b)
{
  // TODO: make sure 'displacement' watches for correct column width
  osg::Vec3 del = displacement( b );
  const Menu* parent = b.parent();
  if( parent )
  {
    // add to the stack
    if( parent->layout() == Menu::HORIZONTAL )
      _hori.push( del[0] );
    else
      _vert.push( del[1] );
  }

  else
  {
    // TODO: does this make sense?
    _hori.push( del[0] );
    _vert.push( del[1] );
  }
}

template<class TileType>
void OSG::TileLayer<TileType>::determine_pop(const Menu& m)
{
  // TODO: clear more than just one pop?
  // TODO: clear all the children?
  // clear the current stack spot
  for(Menu::const_iterator iter=m.begin(); iter!=m.end(); ++iter)
  {
    if( m.layout() == Menu::HORIZONTAL )
      _hori.pop();
    else
      _vert.pop();
  }
}

template<class TileType>
osg::Vec3 OSG::TileLayer<TileType>::displacement(const Menu& m)
{
  Menu::Layout ml = m.layout();
  osg::Vec3 dv;

  if( m.parent() )
  {
    const Menu* parent = m.parent();
    Menu::Layout pl = parent->layout();

    if( pl==Menu::VERTICAL && ml==Menu::VERTICAL )
    {
      dv[0] = 0.5*_tile.width( m );
      dv[1] = 0.0;
    }

    if( pl==Menu::VERTICAL && ml==Menu::HORIZONTAL )
    {
      dv[0] = 0.5*_tile.width( m );
      dv[1] = 0.0;
    }

    if( pl==Menu::HORIZONTAL && ml==Menu::VERTICAL )
    {
      dv[0] = 0.5*_tile.width( m ); //TODO: - 0.5*_tile(parent);
      dv[1] = -0.5*_tile.box().height();
    }

    if( pl==Menu::HORIZONTAL && ml==Menu::HORIZONTAL )
    {
      dv[0] = 0.5*_tile.width( m );
      dv[1] = 0.0;
    }
  }

  else  // parent not valid
  {
    dv[0] = 0.5*_tile.width( m );
    dv[1] = -0.5*_tile.box().height();
  }

  dv[2] = 0.0;
  return dv;
}

template<class TileType>
osg::Vec3 OSG::TileLayer<TileType>::displacement(const Button& b)
{
  osg::Vec3 dv;

  const Menu* parent = b.parent();
  if( parent )
  {
    Menu::Layout pl = parent->layout();

    if( pl==Menu::VERTICAL )
    {
      dv[0] = 0.5*_tile.width( b );
      dv[1] = 0.0;
    }

    if( pl==Menu::HORIZONTAL )
    {
      dv[0] = 0.5*_tile.width( b ); //TODO: - 0.5*_tile(parent);
      dv[1] = -0.5*_tile.box().height();
    }
  }

  else  // parent not valid
  {
    dv[0] = 0.5*_tile.width( b );
    dv[1] = -0.5*_tile.box().height();
  }

  dv[2] = 0.0;
  return dv;
}

template<class TileType>
void OSG::TileLayer<TileType>::apply(Menu& m)
{
  if( !m.parent() )
  {
    _scene = new osg::Group();
  }

  if( m.text().empty() )
  {
    traverse( m );
    return;
  }

  // create the graphic
  osg::ref_ptr<osg::Node> node = _tile( m );

  // calculate the position of the graphic
  osg::Vec3 sum( std::accumulate(_hori.data().begin(),_hori.data().end(),0.0f),
                 std::accumulate(_vert.data().begin(),_vert.data().end(),0.0f),
                 0.0f);
  osg::Vec3 del = displacement( m );
  osg::Vec3 all = sum + del;

  // add the graphic to the scene, with the correct position
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->addChild( node.get() );
  mt->setMatrix( osg::Matrix::translate(all) );
  _scene->addChild( mt.get() );

  // make the child graphics
  determine_push( m );
  traverse( m );            // traverse the children of menu
  determine_pop( m );
}

template<class TileType>
void OSG::TileLayer<TileType>::apply(Button& b)
{
  // TODO: is the tile a good place to put this code?
  // TODO: thus simplifying the code here??
  // determine the width of the graphic
  Menu* parent = b.parent();
  Menu::Layout pl = parent->layout();

  if( pl == Menu::HORIZONTAL )
  {
    float width = _tile.width( b );
    _tile.box().width( width );
  }

  else  // parent menu is VERTICAL
  {
    // TODO: determine if this can be pulled out
    // TODO: so that the WIF test is only needed once
    // find the widest possible graphic
    Detail::WidestItemFinder<ColorTile> wif;
    std::for_each( parent->begin(), parent->end(), wif);
    float width = wif.widest();
    _tile.box().width( width );
  }

  if( !b.parent() )
  {
    float width = _tile.width( b );
    _tile.box().width( width );
  }

  // create the graphic
  osg::ref_ptr<osg::Node> node = _tile( b );

  // calculate the position of the graphic
  //Detail::Accumulator<float> xsum(0.0f), ysum(0.0f);
  //std::for_each<(_hori.begin(),_hori.end(),xsum);
  //std::for_each(_vert.begin(),_vert.end(),ysum);
  float xsum = std::accumulate(_hori.data().begin(),_hori.data().end(),0.0f);
  float ysum = std::accumulate(_vert.data().begin(),_vert.data().end(),0.0f);
  osg::Vec3 sum( xsum, ysum, 0.0f);
  osg::Vec3 del = displacement( b );
  osg::Vec3 all = sum + del;

  // add the graphic to the scene, with the correct position
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->addChild( node.get() );
  mt->setMatrix( osg::Matrix::translate(all) );
  _scene->addChild( mt.get() );

  determine_push( b );
}

#endif
