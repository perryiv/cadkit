#ifndef _menukit_detail_widest_item_finder_h_
#define _menukit_detail_widest_item_finder_h_

#include <algorithm>

namespace Detail
{

  /** WidestItemFinder
    * A class to hold the widest value
    * when iterated over a list of Items
    */
  template<class Evaluator>
  class WidestItemFinder: public std::unary_function<Item::Ptr,void>
  {
  public:
    WidestItemFinder():
        std::unary_function<Item::Ptr,void>(), _widest(0.0), _tile() {}
    ~WidestItemFinder() {}

    void operator ()(Item::Ptr item)    // TODO: wrong argument???
    {
      // TODO: until Item is rewritten, must do dynamic cast
      // TODO: to create Menu or Button
      float w(0.0);
      MenuKit::Menu* menuptr = dynamic_cast<MenuKit::Menu*>( item.get() );
      if( menuptr )
        w = _tile.width( *menuptr );

      //MenuKit::Button* butnptr = dynamic_cast<MenuKit::Button*>( item.get() );
      //if( butnptr )
      //  w = _tile.width( *butnptr );

      // TODO: after Item is rewritten, float w = _tile.width( item );
      if( w > _widest )
        _widest = w;
    }

    float widest() { return _widest; }

  private:
    Evaluator _tile;
    float _widest;
  };

};

#endif
