
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "GFX.h"
#include "Skin.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Trace/Trace.h"

#include "osg/MatrixTransform"
#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osgText/Text"

using namespace MenuKit;
using namespace OSG;

GFX::GFX(Skin* s, Item *a): BaseClass(), _skin(s), _active(a)
{
}

GFX::GFX(const GFX& g): BaseClass(g), _skin(g._skin), _active(g._active)
{
  USUL_TRACE_SCOPE;
}

GFX& GFX::operator = (const GFX& g)
{
  USUL_TRACE_SCOPE;
  BaseClass::operator = (g);
  _skin = g._skin;
  _active = g._active;
  return (*this);
}

osg::Node* GFX::operator() ( const Item* top )
{
  USUL_TRACE_SCOPE;
  osg::ref_ptr<osg::Group> scene = new osg::Group;

  float pw = _skin->graphic_width( top );

  std::string text ( "" );

  // This is hack until building the osg scene is refactored.
  if ( const MenuKit::Button *b = dynamic_cast < const MenuKit::Button * > ( top ) )
    text = b->text ();
  else if ( const MenuKit::Menu *m = dynamic_cast < const MenuKit::Menu * > ( top ) )
    text = m->text ();

  if( text.size()>0 )
  {
    _skin->width( pw );
    scene->addChild( _skin->create(top) );
  }

  const Menu* menu = dynamic_cast<const Menu*>(top);
  if( menu )
  {
    osg::ref_ptr<osg::Node> node = this->traverse( menu,pw );
    scene->addChild( node.get() );
  }

  return (scene.release());
}

osg::Node* GFX::traverse(const Menu* menu,float mwidth)
{
  USUL_TRACE_SCOPE;
  osg::ref_ptr<osg::Group> scene = new osg::Group();

  if( menu->expanded() && (menu->items().size()>0) )
    {
      float lwidth = _skin->find_max_width( menu->items() );
      float xaccum(0.0);  // variable for when menu is HORIZONTAL
      float yaccum(0.0);  // variable for when menu is VERTICAL

      for(Menu::const_iterator iter=menu->begin(); iter!=menu->end(); ++iter)
        {
          // set up the skin's width
          if( menu->layout() == Menu::VERTICAL )
            _skin->width( lwidth );
          else
            _skin->width( _skin->graphic_width( (*iter).get() ) );

          this->_setSkinMode ( iter->get() );
          osg::ref_ptr<osg::Node> node = _skin->create( (*iter).get() );

          // calculate the position of new graphic
          unsigned int index = iter-(menu->begin());
          Menu::const_iterator previous;
          if(index>0)
            previous = (menu->begin()+(index-1));
          else  // send a NULL-like value when @ index=0
            previous = menu->end();

          osg::Vec3 move = this->calculate_move(menu,iter->get(), ( previous == menu->end() ? 0x0 : previous->get() ),
            index,mwidth,lwidth,xaccum,yaccum);

          // add (and position) the new graphic
          osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
          mt->setMatrix( osg::Matrix::translate( move ) );
          scene->addChild( mt.get() );
          mt->addChild( node.get() );

          // look for more content that needs to be displayed
          const Menu* submenu = dynamic_cast<const Menu*>((*iter).get());
          if( submenu )
            {
              if( menu->layout()==Menu::VERTICAL )
                mt->addChild( traverse( submenu,lwidth ) );
              else
              {
                float iwidth = _skin->graphic_width( menu );
                mt->addChild( traverse( submenu,iwidth ) );
              }
            }
        }
    }

  return (scene.release());
}

osg::Vec3 GFX::calculate_move(const Menu* menu, const Item* child, const Item* prevchild, unsigned int index,
                              float menuwidth, float localwidth,float& xaccum, float& yaccum)
{
  USUL_TRACE_SCOPE;
  osg::Vec3 move(0.0,0.0,0.0);
  switch( menu->layout() )
  {
  case Menu::HORIZONTAL:
    {
      float dx,dy;

      if( index==0 )
        xaccum += 0.5*menuwidth+0.5*( _skin->graphic_width(child) );

      if( index>0 )
        {
          double prev = _skin->graphic_width( prevchild );
          double curr = _skin->graphic_width( child );
          xaccum += 0.5*prev + 0.5*curr;
        }

      dx = xaccum;

      const Menu* parent = menu->parent();
      if( parent )
        {
          switch( parent->layout() )
            {
            case Menu::VERTICAL:
              dy = 0.0;  break;

            case Menu::HORIZONTAL:
                dy = -_skin->height();  break;
            }
        }

      else
      {
        dy = 0.0f;
      }

      move.set(dx,dy,0.0);
    } break;

  case Menu::VERTICAL:
    {
      float dx(0.0f);
      const Menu* parent = menu->parent();
      if( parent )
        {
          switch( parent->layout() )
            {
            case Menu::VERTICAL:
              {
                dx = 0.5*menuwidth+0.5*localwidth;
                //dy = -float(index)*(_skin->height());
                if( index==0 )
                  yaccum = 0.0;
                else
                  yaccum += -( 0.5*_skin->graphic_height(prevchild)+0.5*_skin->graphic_height(child) );
              } break;

            case Menu::HORIZONTAL:
              {
                float iwidth = _skin->graphic_width( menu );
                dx = 0.5*localwidth - 0.5*iwidth;
                //dy = -(float(index)+1.0)*_skin->height();
                if( index==0 )
                  yaccum = -( _skin->graphic_height(menu) );
                else
                  yaccum += -( 0.5*_skin->graphic_height(prevchild)+0.5*_skin->graphic_height(child) );
              } break;
            }
        }

      else
          dx = 0.0f;

      float dy = yaccum;
      move.set(dx,dy,0.0);
    } break;
  }

  return move;
}

void GFX::_setSkinMode ( const Item *item )
{
  USUL_TRACE_SCOPE;
  if ( !item->enabled() )
    _skin->mode ( Skin::DISABLED );
  else if ( this->_isInActivePath ( item ) )
    _skin->mode ( Skin::HIGHLIGHT );
  else
    _skin->mode ( Skin::NORMAL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the give item is somewhere in the path of active items.
//
///////////////////////////////////////////////////////////////////////////////

bool GFX::_isInActivePath ( const Item *item )
{
  USUL_TRACE_SCOPE;
  // Get the active item.
  const Item *active = _active.get();

  // Loop until we reach the top.
  while ( active )
  {
    // See if the given item is active.
    if ( item == active )
      return true;

    // Walk up the hierarchy.
    active = active->parent();
  }

  // If we get to here then we failed.
  return false;
}
