
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_mason_h_
#define _menukit_osg_mason_h_

#include "MenuKit/Visitor.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "MenuKit/Stack.h"
#include "MenuKit/Box.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include <string>

#include <numeric>  // for std::accumulate

namespace MenuKit
{
  namespace OSG
  {
    /** TileMason
      * The TileMason class is a Visitor that builds a scene
      * from the MenuKit::Item that accepted it.
      *
      * It uses a TileFunctor class that only needs to support
      *  a few interfaces.
      *
      * The main goal of the Mason is to use a tile to obtain size
      * information so that each item can be placed adjacent to other items.
      * TileMason maintains a stack for offsets in the vertical and horizontal directions.
      *
      * A secondary goal is to configure the tile for display attributes.
      */
    template<typename TileType>
    class Mason : public Visitor
    {
    public:
      typedef Visitor base_class;
      typedef TileType tile_type;
      typedef typename tile_type::RefPtr tile_ptr;
      typedef Mason<tile_type> thisclass;
      MENUKIT_DECLARE_POINTER ( thisclass );
      typedef Detail::Stack<float> FloatStack;

      Mason(): base_class(base_class::EXPANDED), _tile(0x0), _hori(), _vert(), _scene(0x0), _scene_name("MK_OSG_TOP"), _xform_name("MK_OSG_XFORM") {}

      virtual void apply(Menu& m);
      virtual void apply(Button& m);

      const FloatStack& horizontal_stack() const { return _hori; }
      const FloatStack& vertical_stack() const { return _vert; }

      void tile(tile_type* t)       { _tile = t; }
      tile_type* tile()             { return _tile.get(); }
      const tile_type* tile() const { return _tile.get(); }

      //const osg::Node* scene() const { return _scene.get(); }
      osg::Node* scene() { return _scene.get(); }

      const std::string& scene_name() const { return _scene_name; }
      const std::string& transform_name() const { return _xform_name; }

    protected:
      virtual ~Mason() {}

      void _make_graphic(const Menu& m, float dx, float dy);
      void _make_graphic(const Button& b, float dx, float dy);
      float _determine_greatest(const Menu::Items& items, Menu::Layout direction);

      enum Direction { HORIZONTAL, VERTICAL };
      Direction _determine_pushpop_direction(Menu::Layout menus,Menu::Layout parents);

      Detail::Box _determine_tile_size(const Menu& m,Menu::Layout parent_layout);
      Detail::Box _determine_tile_size(const Button& b,Menu::Layout parent_layout);

      typename tile_type::DisplayMode _determine_tile_display_mode(const Button& m);
      typename tile_type::DisplayMode _determine_tile_display_mode(const Menu& m);

    private:
      ///\todo TODO: evaluate if this class is copyable
      // not implemented by design
      Mason(const Mason& m);
      Mason& operator =(const Mason& m);

      tile_ptr _tile;
      FloatStack _hori, _vert;
      osg::ref_ptr<osg::Group> _scene;
      std::string _scene_name, _xform_name;
    };

    template<typename T>
    void Mason<T>::apply(Menu& m)
    {
      /**\todo TODO: find if there is a better way to reset the _scene.
        * This will only work for the case where a top level menu has been traversed.
        * If it is desired to begin traversal at a submenu, the effect is unknown.
        * A switch should be invented.
        */
      // grab some info
      Menu* parent = m.parent();
      Menu::Layout pl;
      if( parent )
        pl = parent->layout();
      else
      {
        /**\todo TODO: implement a 'switch' to be the initialization event.
          * FIX: This will cause problems if any Item other than the top Item
          * is initially traversed.
          */

        _scene = new osg::Group();  // NULL parent is the initialization event
        _scene->setName( _scene_name );

        pl = Menu::HORIZONTAL;  // designates as horizontal layout
 
        if( m.text().empty() )
        {
          traverse(m);

          Menu::Layout ml = m.layout();
          // pop for all children
          if( ml == Menu::VERTICAL )
          {
            for(Menu::Items::const_iterator iter=m.items().begin(); iter!=m.items().end(); iter++)
              _vert.pop();
          }

          else
          {
            for(Menu::Items::const_iterator iter=m.items().begin(); iter!=m.items().end(); iter++)
              _hori.pop();
          }

          return;
        }
      }

      // configure the tile
      _tile->mode( _determine_tile_display_mode(m) );
      _tile->box( _determine_tile_size(m, pl) );

      // save the tile's box state
      Detail::Box mbox( _tile->box() );

      // create the graphics
      _make_graphic(m,
        std::accumulate(_hori.data().begin(),_hori.data().end(),0.0),
       -std::accumulate(_vert.data().begin(),_vert.data().end(),0.0) );

      Menu::Layout ml = m.layout();

      // things to do when children are exposed
      // internal member _tile may be modified
      if( m.expanded() )
      {
        Direction pushpop = _determine_pushpop_direction(ml,pl);

        // do initial push so that children start in the correct place
        if( Direction::VERTICAL == pushpop )
          _vert.push(mbox.height());
        else
          _hori.push(mbox.width());

        traverse( m );

        // pop for all children
        if( ml == Menu::VERTICAL )
        {
          for(Menu::Items::const_iterator iter=m.items().begin(); iter!=m.items().end(); iter++)
            _vert.pop();
        }

        else
        {
          for(Menu::Items::const_iterator iter=m.items().begin(); iter!=m.items().end(); iter++)
            _hori.pop();
        }

        // pop off the initial move before traversing childen
        if( pushpop == Direction::VERTICAL )
          _vert.pop();
        else
          _hori.pop();
      }

      // push for this graphic so the next menu item will accumulate correctly
      if( pl == Menu::VERTICAL )
        _vert.push( mbox.height() );
      else
        _hori.push( mbox.width() );
    }

    template<typename T>
    void Mason<T>::apply(Button& b)
    {
      Menu* parent = b.parent();
      Menu::Layout pl;
      if( parent )
        pl = parent->layout();
      else
      {
        _scene = new osg::Group();  // catch initialization event
        _scene->setName( _scene_name );
        pl = Menu::HORIZONTAL;  // designates to be a horizontal layout
      }

      // configure tile
      _tile->mode( _determine_tile_display_mode(b) );
      _tile->box( _determine_tile_size( b, pl) );

      // generate the graphic
      _make_graphic(b,
        std::accumulate(_hori.data().begin(),_hori.data().end(),0.0f),
       -std::accumulate(_vert.data().begin(),_vert.data().end(),0.0f) );

      // push for this graphic so the next menu item will accumulate correctly
      if( pl == Menu::VERTICAL )
        _vert.push( _tile->box().height() );
      else
        _hori.push( _tile->box().width() );
    }

    template<typename T>
    typename Mason<T>::Direction Mason<T>::_determine_pushpop_direction(Menu::Layout ml, Menu::Layout pl)
    {
      if( pl == Menu::HORIZONTAL )
      {
        //if( ml == Menu::VERTICAL )
        //  return Mason<T>::VERTICAL;
        //else
          return Mason<T>::VERTICAL;   // always need to push vertically
      }

      else  // parent's layout is vertical
          return Mason<T>::HORIZONTAL;  // always need to push horizontally
    }

    template<typename T>
    void Mason<T>::_make_graphic(const Button& b, float dx, float dy)
    {
      // this method uses the state of the _tile for correctness
      osg::ref_ptr<osg::Node> node = _tile->operator()( b );

      // make a xform node and attach it to the scene.
      osg::MatrixTransform* xform = new osg::MatrixTransform();
      xform->setName(_xform_name);
      xform->setMatrix( osg::Matrix::translate(dx,dy,0.0) );

      // make the graphic via the tile and add it to the xform
      xform->addChild( node.get() );
      _scene->addChild( xform );
    }

    template<typename T>
    void Mason<T>::_make_graphic(const Menu& m, float dx, float dy)
    {
      // this method uses the state of the _tile for correctness
      osg::ref_ptr<osg::Node> node = _tile->operator()( m );

      // make a xform node and attach it to the scene.
      osg::MatrixTransform* xform = new osg::MatrixTransform();
      xform->setName(_xform_name);
      xform->setMatrix( osg::Matrix::translate(dx,dy,0.0) );

      // make the graphic via the tile and add it to the xform
      xform->addChild( node.get() );
      _scene->addChild( xform );
    }

    template<typename T>
    float Mason<T>::_determine_greatest(const Menu::Items& items, Menu::Layout direction)
    {
      float max_value(0.0f);
      if( direction == Menu::VERTICAL )
      {
        for(Menu::Items::const_iterator iter=items.begin(); iter!=items.end(); ++iter)
        {
          float temp = _tile->width( (*iter).get() );
          if( max_value < temp )
            max_value = temp;
        }
      }

      else // direction is HORIZONTAL
      {
        for(Menu::Items::const_iterator iter=items.begin(); iter!=items.end(); ++iter)
        {
          float temp = _tile->height( (*iter).get() );  ///\todo TODO: this call assumes menu and button are calculated with the same formula, could be improved
          if( max_value < temp )
            max_value = temp;
        }
      }

      return max_value;
    }

    template<typename T>
    Detail::Box Mason<T>::_determine_tile_size(const Menu& m,Menu::Layout parent_layout)
    {
      Detail::Box box;
      if( parent_layout == Menu::HORIZONTAL )
      {
        box.height( _tile->height(m) ); ///\todo TODO: could be 'tallest' child height
        box.width( _tile->width(m) );
      }

      else  // parent's layout is VERTICAL
      {
        box.height( _tile->height(m) );
        if( m.parent() )
          box.width( _determine_greatest( m.parent()->items(), parent_layout ) );
        else
          box.width( _tile->width(m) );
      }

      return box;
    }

    template<typename T>
    Detail::Box Mason<T>::_determine_tile_size(const Button& b,Menu::Layout parent_layout)
    {
      Detail::Box box;
      if( parent_layout == Menu::HORIZONTAL )
      {
        ///\todo TODO: could be 'tallest' child height
        box.height( _tile->height(b) );
        box.width( _tile->width(b) );
      }

      else  // parent's layout is VERTICAL
      {
        box.height( _tile->height(b) );
        if( b.parent() )
          box.width( _determine_greatest( b.parent()->items(), parent_layout ) );
        else
          box.width( _tile->width(b) );
      }

      return box;
    }

    template<typename T>
    typename Mason<T>::tile_type::DisplayMode Mason<T>::_determine_tile_display_mode(const Menu& m)
    {
      tile_type::DisplayMode dm( tile_type::NORMAL );
      if( m.marked() || m.expanded() )
        dm = tile_type::HIGHLIGHT;
      if( !m.enabled() )
        dm = tile_type::DISABLED;

      return dm;
    }

    template<typename T>
    typename Mason<T>::tile_type::DisplayMode Mason<T>::_determine_tile_display_mode(const Button& b)
    {
      tile_type::DisplayMode dm( tile_type::NORMAL );
      if( b.marked() || b.expanded() )
        dm = tile_type::HIGHLIGHT;
      if( !b.enabled() )
        dm = tile_type::DISABLED;

      return dm;
    }

  };
};

#endif
