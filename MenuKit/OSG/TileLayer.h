#ifndef _menukit_osg_tilelayer_
#define _menukit_osg_tilelayer_

#include "../Visitor.h"      // the base class
#include "ColorThemeTile.h"  // included for the typedef

// TODO: consider this as a baseclass in the MenuKit namespace
// TODO: for other APIs to take advantage of for the skeleton
// TODO: of a scene builder
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
      TileLayer(): Visitor(), _tile() {}
      TileLayer(Visitor::Mode m): Visitor(m), _tile() {}

      //MENUKIT_DECLARE_POINTER(TileLayer<TileType>);

      void tile(const TileType& t) { _tile = t; }
      const TileType& tile() const { return _tile; }
      TileType& tile() { return _tile; }

      virtual void apply(Menu&);
      virtual void apply(Button&);
      // TODO, remove Button and Menu classes,then: virtual void apply(Item&);

    protected:
      virtual ~TileLayer() {}

    private:
      TileType _tile;
    };

  typedef TileLayer<ColorTile> ColorTileLayer;
  };

};

using namespace MenuKit;

template<class TileType> void OSG::TileLayer<TileType>::apply(Button& b)
{
  //float tw = TileType::CalculateTextWidth(m);
  //if( tw > _column_width )
  //{
    //_column_width = tw;
    //RebuildColumn( _current_parent );
  //}
  osg::ref_ptr<osg::Node> node = _tile(b);
}

template<class TileType> void OSG::TileLayer<TileType>::apply(Menu& m)
{
  osg::ref_ptr<osg::Node> node = _tile(m);
}


#endif
