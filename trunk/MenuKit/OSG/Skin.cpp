
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MenuKit/OSG/Skin.h"

#include "MenuKit/OSG/DrawableFunctor.h"
#include "MenuKit/OSG/BackgroundBox.h"
#include "MenuKit/OSG/Word.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/Separator.h"

#include "Usul/Trace/Trace.h"

#include "osg/MatrixTransform"
#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osgText/Text"

#include <algorithm>
#include <functional>

using namespace MenuKit;
using namespace OSG;

Skin::Skin ( osgText::Font* f,
             const osg::Vec4& tcn,
             const osg::Vec4& tch,
             const osg::Vec4& tcd,
             const osg::Vec4& bcn,
             const osg::Vec4& bch,
             const osg::Vec4& bcd,
             float h, 
             float w, 
             float m,
             float is,
             BGDM bgm)    : BaseClass(),
                            _font                ( f   ),
                            _text_color_normal   ( tcn ),
                            _text_color_highlight( tch),
                            _text_color_disabled ( tcd),
                            _bg_color_normal     ( bcn ),
                            _bg_color_highlight  ( bch ),
                            _bg_color_disabled   ( bcd ),
                            _height              ( h   ), 
                            _width               ( w   ), 
                            _margin              ( m   ),
                            _icon_space          ( is  ),
                            _mode                ( Skin::NORMAL ),
                            _bg_draw_mode        ( bgm )
{
  USUL_TRACE_SCOPE;
}

Skin::Skin ( const Skin& s )  : BaseClass ( s ),
                                _font                ( s._font              ), 
                                _text_color_normal   ( s._text_color_normal ),
                                _text_color_highlight( s._text_color_highlight ),
                                _text_color_disabled ( s._text_color_disabled ),
                                _bg_color_normal     ( s._bg_color_normal   ), 
                                _bg_color_highlight  ( s._bg_color_highlight   ), 
                                _bg_color_disabled   ( s._bg_color_disabled   ), 
                                _height              ( s._height            ), 
                                _width               ( s._width             ),
                                _margin              ( s._margin            ),
                                _icon_space          ( s._icon_space      ),
                                _mode                ( s._mode              ),
                                _bg_draw_mode        ( s._bg_draw_mode )
{
  USUL_TRACE_SCOPE;
}

Skin& Skin::operator = ( const Skin& s )
{
  USUL_TRACE_SCOPE;
  BaseClass::operator = ( s );
  _font                 = s._font;
  _text_color_normal    = s._text_color_normal;
  _text_color_highlight = s._text_color_highlight;
  _text_color_disabled  = s._text_color_disabled;
  _bg_color_normal      = s._bg_color_normal;
  _bg_color_highlight   = s._bg_color_highlight;
  _bg_color_disabled    = s._bg_color_disabled;
  _height               = s._height;
  _width                = s._width;
  _margin               = s._margin;
  _icon_space           = s._icon_space;
  _mode                 = s._mode;
  _bg_draw_mode         = s._bg_draw_mode;

  return (*this);
}


const osg::Vec4 &Skin::_properTextColor()
{
  USUL_TRACE_SCOPE;
  if ( HIGHLIGHT == this->mode() )
    return _text_color_highlight;
  else if ( DISABLED == this->mode() )
    return _text_color_disabled;
  else
    return _text_color_normal;
}


const osg::Vec4 &Skin::_properBackgroundColor()
{
  USUL_TRACE_SCOPE;
  if ( HIGHLIGHT == this->mode() )
    return _bg_color_highlight;
  else if ( DISABLED == this->mode() )
    return _bg_color_disabled;
  else
    return _bg_color_normal;
}



class Arrow: public DrawableFunctor
{
public:
  Arrow(float h): DrawableFunctor(h) {}
  ~Arrow() {}

  virtual osg::Drawable* operator() ();
};

class Disk: public DrawableFunctor
{
public:
  enum Mode
  {
    HALLOW,
    FILL
  };

  Disk(float h,Mode m=HALLOW,unsigned int p=80,float d=0.0):
    DrawableFunctor(h),_mode(m),_points(p),_depth(d) {}
  ~Disk() {}

  virtual osg::Drawable* operator() ();

  void depth(float d) { _depth = d; }
  float depth() const { return _depth; }

  void mode(Mode m) { _mode = m; }
  Mode mode() const { return _mode; }

  void points(unsigned int p) { _points = p; }
  unsigned int points() const { return _points; }

private:
  Mode _mode;
  unsigned int _points;
  float _depth;
};

//--- end of helper classes ---//

osg::Node* Skin::create(const MenuKit::Item* itm)
{
  USUL_TRACE_SCOPE;
  osg::Node* scene ( 0x0 );
  const MenuKit::Button* b = dynamic_cast<const MenuKit::Button*>( itm );
  if( b )
  {
    scene = this->create_button(b);
  }

  if( itm && itm->separator() )
    scene = this->create_separator( itm );

  const MenuKit::Menu* m = dynamic_cast<const MenuKit::Menu*>( itm );
  if( m )
    scene = this->create_menu(m);

  return( scene );
}

osg::Node* Skin::create_menu(const Menu* menu)
{
  USUL_TRACE_SCOPE;
  float magic = -0.2*(this->height());

  Word word( menu->text(), _font.get() );
  word.height( 0.5*(this->height()) );
  word.color ( this->_properTextColor() );

  osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
  geode2->addDrawable( word() );

  BackgroundBox box(_height,_width,-0.001,osg::PrimitiveSet::Mode(_bg_draw_mode));
  box.color ( this->_properBackgroundColor() );

  osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
  geode1->addDrawable( box() );

  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->addChild( geode2.get() );
  float dxtext;
  const MenuKit::Menu* parent = menu->parent();
  if( parent )
  {
    if( parent->layout()==Menu::VERTICAL )
      dxtext = -0.5*_width + _margin + _icon_space;
    else
      dxtext = -0.5*_width + _margin;
  }
  else
    dxtext = -0.5*_width + _margin;
  osg::Vec3 movetext (dxtext,magic,0.0);
  mt->setMatrix( osg::Matrix::translate(movetext) );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->addChild( geode1.get() );
  group->addChild( mt.get() );
  group->setName( menu->text() );

  // arrow
  if( parent )
  {
    if( (menu->layout()==Menu::VERTICAL) && (parent->layout()!=Menu::HORIZONTAL) && (menu->items().size()>0) )
    {
      Arrow arrow(0.5*_icon_space);
      arrow.color(this->_properTextColor());
      osg::ref_ptr<osg::Geode> geode3 = new osg::Geode();
      geode3->addDrawable( arrow() );
      float dxarrow = -0.5*_icon_space + 0.5*_width;
      osg::Vec3 moveicon1(dxarrow,0.0,0.0);
      osg::ref_ptr<osg::MatrixTransform> arrowt = new osg::MatrixTransform();
      arrowt->setMatrix( osg::Matrix::translate(moveicon1) );
      arrowt->addChild( geode3.get() );
      group->addChild( arrowt.get() );
    }
  }

  return group.release();
}

osg::Node* Skin::create_button(const Button* button)
{
  USUL_TRACE_SCOPE;
  float magic = -0.2*(this->height());

  Word word( button->text(), _font.get() );
  word.height( 0.5*(this->height()) );
  word.color ( this->_properTextColor() );

  osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
  geode2->addDrawable( word() );

  BackgroundBox box(_height,_width,-0.001,osg::PrimitiveSet::Mode(_bg_draw_mode));
  box.color ( this->_properBackgroundColor() );

  osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
  geode1->addDrawable( box() );

  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->addChild( geode2.get() );
  float dx = _icon_space - 0.5*_width + _margin;
  osg::Vec3 move (dx,magic,0.0);
  mt->setMatrix( osg::Matrix::translate(move) );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->addChild( geode1.get() );
  group->addChild( mt.get() );
  group->setName( button->text() );

  // check box
  if( button->toggle() )
  {
    osg::ref_ptr<osg::Geode> checkbox = new osg::Geode();

    if( button->checked() )
    {
      BackgroundBox fillbox(0.75*0.5*_icon_space,0.75*0.5*_icon_space,0.0,osg::PrimitiveSet::QUADS);
      fillbox.color(this->_properTextColor());
      checkbox->addDrawable( fillbox() );
    }

    BackgroundBox hallowbox(0.5*_icon_space,0.5*_icon_space,0.0,osg::PrimitiveSet::LINE_LOOP);
    hallowbox.color( (this->_properTextColor()) );
    checkbox->addDrawable( hallowbox() );

    osg::ref_ptr<osg::MatrixTransform> ft = new osg::MatrixTransform();
    ft->addChild( checkbox.get() );
    float dxfill = 0.5*_icon_space - 0.5*_width;
    osg::Vec3 movefill(dxfill,0.0,0.0);
    ft->setMatrix( osg::Matrix::translate(movefill) );
    group->addChild( ft.get() );
  }

  else if( button->radio() )
  {
    osg::ref_ptr<osg::Geode> checkbox = new osg::Geode();

    if( button->checked() )
    {
      Disk filldisk(0.75*0.5*_icon_space,Disk::FILL);
      filldisk.color( (this->_properTextColor()) );
      checkbox->addDrawable( filldisk() );
    }

    Disk hallowdisk(0.5*_icon_space,Disk::HALLOW);
    hallowdisk.color( (this->_properTextColor()) );
    checkbox->addDrawable( hallowdisk() );

    osg::ref_ptr<osg::MatrixTransform> rt = new osg::MatrixTransform();
    rt->addChild( checkbox.get() );
    float dxrad = 0.5*_icon_space - 0.5*_width;
    osg::Vec3 moveradio(dxrad,0.0,0.0);
    rt->setMatrix( osg::Matrix::translate( moveradio ) );
    group->addChild( rt.get() );
  }

  return group.release();
}

osg::Node* Skin::create_separator(const MenuKit::Item* button)
{
  USUL_TRACE_SCOPE;
  BackgroundBox box(0.1*(this->height()),(this->width()));
  box.color( (this->_properBackgroundColor()) );

  BackgroundBox line(1.0*box.height(),0.95*box.width(),0.0);
  line.color( (this->_properTextColor()) );

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  geode->addDrawable( box() );
  geode->addDrawable( line() );

  return( geode.release() );
}

float Skin::graphic_height(const MenuKit::Item* item)
{
  USUL_TRACE_SCOPE;
  const MenuKit::Button* b = dynamic_cast<const MenuKit::Button*>( item );
  if( item && item->separator() )
    return( 0.1*(this->height()) );

  else
    return( (this->height()) );
}

float Skin::graphic_width(const Item* item)
{
  USUL_TRACE_SCOPE;
  std::string text ( "" );

  // This is hack until building the osg scene is refactored.
  if ( const MenuKit::Button *b = dynamic_cast < const MenuKit::Button * > ( item ) )
    text = b->text ();
  else if ( const MenuKit::Menu *m = dynamic_cast < const MenuKit::Menu * > ( item ) )
    text = m->text ();

  if ( text.empty() )
    return 0.0;

  Word word( text, _font.get() );
  word.height( 0.5*(this->height()) );
  osg::ref_ptr<osg::Drawable> wdraw = word();
  osg::BoundingBox wbox = wdraw->getBound();

  // define the equation that calculates the width of the object
  // when drawn with all the extras like margins and icons
  float total_width;
  const MenuKit::Menu* parent = item->parent();
  if( parent )
  {
    const MenuKit::Menu* menu = dynamic_cast<const MenuKit::Menu*>(item);
    if( menu )
    {
      if( parent->layout()==Menu::HORIZONTAL )
      {
        total_width = (wbox.xMax()-wbox.xMin())+2.0*_margin;
      }

      else  // parent is vertical
        total_width = (wbox.xMax()-wbox.xMin())+2.0*_icon_space+2.0*_margin;
    }

    else  // must be a Button
      total_width = (wbox.xMax()-wbox.xMin())+2.0*_icon_space+2.0*_margin;
  }

  else  // should never happen
    total_width = (wbox.xMax()-wbox.xMin())+2.0*_icon_space+2.0*_margin;

  BackgroundBox box(_height,total_width);
  osg::ref_ptr<osg::Drawable> drawable = box();
  osg::BoundingBox bbox = drawable->getBound();

  return ( bbox.xMax()-bbox.xMin() );
}

float Skin::find_max_width(const Menu::Items& items)
{
  USUL_TRACE_SCOPE;
  CompareItemsPredicate<Menu::Items::value_type> pred(this);
  Menu::const_iterator iter = std::max_element( items.begin(), items.end(), pred );

  return ( ( items.end() == iter ) ? 0.0f : graphic_width ( iter->get() ) );
}

osg::Drawable* Arrow::operator () ()
{
  USUL_TRACE_SCOPE;
  osg::Vec3Array* vertices = new osg::Vec3Array();
  float h_2 = 0.5f * height();
  vertices->push_back( osg::Vec3(0.0, h_2 , 0.0) );
  vertices->push_back( osg::Vec3(0.0,-h_2 , 0.0) );
  vertices->push_back( osg::Vec3(h_2, 0.0 , 0.0) );

  osg::Vec3Array* normals = new osg::Vec3Array();
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  osg::Vec4Array* colors = new osg::Vec4Array();
  colors->push_back( color() );

  osg::ref_ptr<osg::Geometry> patch = new osg::Geometry();
  patch->setVertexArray( vertices );
  patch->setNormalArray( normals );
  patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
  patch->setColorArray( colors );
  patch->setColorBinding( osg::Geometry::BIND_OVERALL );
  patch->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()) );

  return( patch.release() );
}

osg::Drawable* Disk::operator() ()
{
  USUL_TRACE_SCOPE;
  float radius = 0.5*height();

  osg::Vec3Array* vertices = new osg::Vec3Array();

  osg::PrimitiveSet::Mode drawmode;
  switch( _mode )
  {
  case HALLOW:
    {
      drawmode = osg::PrimitiveSet::LINE_LOOP;

      // define the circle
      float da = 2.0*osg::PI / float(_points);
      for(unsigned int i=0; i<_points; i++)
        {
          float angle = float(i)*da;
          float x = radius*cosf(angle);
          float y = radius*sinf(angle);
          vertices->push_back( osg::Vec3(x,y,_depth) );
        } break;
    }

  case FILL:
    {
      drawmode = osg::PrimitiveSet::TRIANGLE_FAN;

      // define the circle
      float da = 2.0*osg::PI / float(_points);
      for(unsigned int i=0; i<_points; i++)
      {
        vertices->push_back( osg::Vec3(0.0,0.0,_depth) );

        float angle = float(i)*da;
        float x = radius*cosf(angle);
        float y = radius*sinf(angle);
        vertices->push_back( osg::Vec3(x,y,_depth) );

        float nextangle;
        if( i==(_points-1) )
          nextangle = 0.0;
        else
          nextangle = float(i+1)*da;
        float nx = radius*cosf(nextangle);
        float ny = radius*sinf(nextangle);
        vertices->push_back( osg::Vec3(nx,ny,_depth) );
      }
    } break;
  }

  osg::Vec4Array* colors = new osg::Vec4Array();
  colors->push_back( color() );

  osg::Vec3Array* normals = new osg::Vec3Array();
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  osg::ref_ptr<osg::Geometry> disk = new osg::Geometry();
  disk->setVertexArray( vertices );
  disk->setNormalArray( normals );
  disk->setColorArray( colors );
  disk->setNormalBinding( osg::Geometry::BIND_OVERALL );
  disk->setColorBinding( osg::Geometry::BIND_OVERALL );
  disk->addPrimitiveSet( new osg::DrawArrays(drawmode,0,vertices->size()) );

  return( disk.release() );
}

//--- end of helper implementation ---//

