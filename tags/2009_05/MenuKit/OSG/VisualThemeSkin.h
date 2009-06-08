
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_visual_themeskin_h_
#define _menukit_osg_visual_themeskin_h_

#include "MenuKit/OSG/ThemeSkin.h"   // the base class
#include "MenuKit/OSG/Border.h"      // graphics helpers
#include "MenuKit/OSG/FlatBox.h"
#include "MenuKit/OSG/Word.h"
#include "MenuKit/OSG/Arrow.h"
#include "MenuKit/OSG/Disk.h"

#include "MenuKit/Bits.h"
#include "MenuKit/Box.h"
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
///\todo TODO: #include "Item.h"

#include "osgText/Text"
#include "osg/PrimitiveSet"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/MatrixTransform"
#include "osg/BoundingBox"

#include <string>

/*
-------------------------------------------|
|b o r d e r                               |
|o                                         |
|r                                         |
|d    ---------- m a r g i n    ---------- |
|e    |        | a |---------|  |        | |
|r    | MIDDLE | r | T E X T |  | MIDDLE | |
|     |        | g |---------|  |        | |
|     ---------- i              ---------- |
|                n                         |
|           b a c k g r o u n d            |
|------------------------------------------|
*/

namespace MenuKit
{

  namespace OSG
  {

    /**\todo
      * define a base_class::ModeMap functor and use that
      * to initialize the base_class constructor
      */

    /** @class VisualThemeSkin
      *
      * The VisualThemeSkin class is a simple implementation
      * of the ThemeSkin base class.  Its goal is to provide
      * Menu Items with a similar appearance to the Menu Items
      * in Microsoft's Visual Studio IDE.
      * 
      * This class is done with only an OSG implementation.
      * It is intended to be a leaf class, which has many
      * OSG specific methods.
      *
      * @param _margin Margin is the distance buffering the text.
      * @param _border Border is the distance sourrounding any placeable attributes.
      */

    typedef std::map<std::string,osg::Vec4> VTSColorMap;
    struct VTSModeMapFunctor
    {
      typedef MenuKit::OSG::ThemeSkin<VTSColorMap>::ModeMap return_type;
      typedef MenuKit::OSG::ThemeSkin<VTSColorMap>::tile_type tile_type;

      return_type operator ()()
      {
        osg::Vec4 yellow(1.0f,1.0f,0.0f,1.0f);
        osg::Vec4 red(1.0f,0.0f,0.0f,1.0f);
        osg::Vec4 green(0.0f,0.0f,1.0f,1.0f);
        osg::Vec4 teal(0.0f,1.0f,1.0f,1.0f);

        osg::Vec4 blue(0.0f,0.0f,1.0f,1.0f);
        osg::Vec4 faintblue(0.7f,0.7f,1.0f,1.0f);

        osg::Vec4 black(0.0f,0.0f,0.0f,1.0f);
        osg::Vec4 gray(0.7f,0.7f,0.7f,1.0f);
        osg::Vec4 lightgray(0.9f,0.9f,0.9f,1.0f);
        osg::Vec4 darkgray(0.2f,0.2f,0.6f,1.0f);

        // make 3 ModeMap::value_types for 3 ModeMap::key_types
        VTSColorMap normal;
        normal["text"] = black;
        normal["middle"] = gray;
        normal["border"] = blue;
        normal["horizontal_background"] = gray;
        normal["vertical_background"] = lightgray;
        normal["special"] = red;

        VTSColorMap hilight;
        hilight["text"] = black;
        hilight["middle"] = gray;
        hilight["border"] = blue;
        hilight["horizontal_background"] = faintblue;
        hilight["vertical_background"] = faintblue;
        hilight["special"] = red;

        VTSColorMap disabled;
        disabled["text"] = gray;
        disabled["middle"] = gray;
        disabled["border"] = red;
        disabled["horizontal_background"] = gray;
        disabled["vertical_background"] = lightgray;
        disabled["special"] = red;

        return_type mm;
        mm[tile_type::DISABLED] = disabled;
        mm[tile_type::HIGHLIGHT] = hilight;
        mm[tile_type::NORMAL] = normal;

        return mm;
      }
    };

    class VisualThemeSkin : public MenuKit::OSG::ThemeSkin<VTSColorMap>
    {
    public:
      typedef ThemeSkin<VTSColorMap> base_class;
      MENUKIT_DECLARE_POINTER ( VisualThemeSkin );

      enum ItemBits
      {
        NONE      = 0x00000000,
        MENU      = 0x00000001,
        CHECKED   = 0x00000002,
        TOGGLE    = 0x00000004,
        RADIO     = 0x00000008,
        DISABLED  = 0x00000010,
        EXPANDED  = 0x00000020,
        MARKED    = 0x00000040,
      };

      VisualThemeSkin(): base_class(), _margin(6.0f), _border(0.0f)
      {
        VTSModeMapFunctor mmf;
        this->mode_map( mmf() );
      }

      virtual osg::Node* operator ()(const Menu& m);
      virtual osg::Node* operator ()(const Button& b);
      // TODO: virtual osg::Node* operator =(const Item& b);

      virtual float height(const Menu& m) const;
      virtual float height(const Button& b) const;
      virtual float height(const Item* i) const;

      virtual float width(const Menu& m) const;
      virtual float width(const Button& b) const;
      virtual float width(const Item* i) const;

      void border(float m) { _border=m; }
      float border() const { return _border; }

      void margin(float m) { _margin=m; }
      float margin() const { return _margin; }

    protected:
      virtual ~VisualThemeSkin() {}

      float _total_height() const;
      float _word_width(const std::string& word) const;
      float _item_width(const std::string& w,const Menu* m) const;
      osg::Node* _item_graphic(const std::string& txt, const Menu* parent, ItemBits b);
      osg::Node* _separator_graphic(MenuKit::Menu::Layout parent_layout);

      ItemBits _gather_bits(unsigned int menubits);

    private:
      ///\todo TODO: evaluate if this class is copyable

      // not implemented by design
      VisualThemeSkin(const VisualThemeSkin& s);//: base_class(s),
      //  _margin(s._margin), _text(s._text), _border(s._border)
      //{}

      VisualThemeSkin& operator= (const VisualThemeSkin& ct);
      //{
      //  base_class::operator =(ct);
      //  _margin = ct._margin;
      //  _text = ct._text;
      //  _border = ct._border;
      //  return( *this );
      //}

      float _margin,  // distance between box's edge to an add-on
            _border;  // distance from the margin to the graphic's edge
    };

  };

};

// ------------------------ //
//   SKIN IMPLEMENTATION
// ------------------------ //

using namespace MenuKit;
using namespace OSG;

osg::Node* VisualThemeSkin::operator ()(const Menu& m)
{
  typedef MenuKit::Bits<ItemBits> setter;

  ItemBits bits = NONE;    // set bits to nothing
  bits = setter::add( bits , MENU );

  osg::Node* node = 0x0;

  // check for a 'SEPARATOR'  >:-/
  if( m.separator() )
  {
    MenuKit::Menu::Layout pl(MenuKit::Menu::HORIZONTAL);
    const MenuKit::Menu* p(m.parent());
    if( p )
      pl = p->layout();
    node = _separator_graphic(pl);
  }

  else
  {
    ItemBits b = this->_gather_bits( m.flags() );
    bits = setter::add( b , bits );
    node = _item_graphic( m.text(), m.parent(), bits );
  }

  return( node );
}

osg::Node* VisualThemeSkin::operator ()(const Button& b)
{
  osg::Node* node = 0x0;

  // check for a 'SEPARATOR'  >:-/
  if( b.separator() )
  {
    MenuKit::Menu::Layout pl(MenuKit::Menu::HORIZONTAL);
    const MenuKit::Menu* p(b.parent());
    if( p )
      pl = p->layout();
    node = _separator_graphic(pl);
  }

  else
  {
    ItemBits bits = this->_gather_bits( b.flags() );
    node = _item_graphic( b.text(), b.parent(), bits );
  }

  return( node );
}

VisualThemeSkin::ItemBits VisualThemeSkin::_gather_bits(unsigned int itembits)
{
  // set up any bits necessary
  typedef MenuKit::Bits<unsigned int> cause;
  typedef MenuKit::Bits<ItemBits> effect;
  ItemBits bits = NONE;

  if( cause::has(itembits, MenuKit::Item::EXPANDED) )
    bits = effect::add( bits , EXPANDED );
  if( cause::has(itembits, MenuKit::Item::TOGGLE) )
    bits = effect::add( bits , TOGGLE );
  if( !cause::has(itembits, MenuKit::Item::ENABLED) )
    bits = effect::add( bits , DISABLED );
  if( cause::has(itembits, MenuKit::Item::RADIO) )
    bits = effect::add( bits , RADIO );
  if( cause::has(itembits, MenuKit::Item::CHECKED) )
    bits = effect::add( bits , CHECKED );
  if( cause::has(itembits, MenuKit::Item::MARKED) )
    bits = effect::add( bits , MARKED );

  return bits;
}

///\todo TODO: this needs a lot of clean up
osg::Node* VisualThemeSkin::_item_graphic(const std::string& txt,const Menu* parent, ItemBits itembits)
{
  Detail::Box thebox(this->_total_height(),this->graphic_width());

  Menu::Layout pl;
  if( parent )
    pl = parent->layout();
  else
  {
    if( txt == "" )
    {
      osg::Node* notext = new osg::Node();
      notext->setName("MK_OSG_notext");
      return notext;
    }
    pl = Menu::HORIZONTAL;
  }

  const base_class::ModeMap& mmap = this->mode_map();
  base_class::ModeMap::const_iterator modeiter = mmap.find( this->mode() );

  if( modeiter == mmap.end() )
  {
    ///\todo implement exception code here
  }
  base_class::ModeMap::value_type modetheme = *modeiter;
  base_class::ModeMap::value_type::second_type scheme = modetheme.second;

  base_class::theme_type::const_iterator middleiter = scheme.find("middle");
  base_class::theme_type::const_iterator hbgiter = scheme.find("horizontal_background");
  base_class::theme_type::const_iterator vbgiter = scheme.find("vertical_background");
  base_class::theme_type::const_iterator textiter = scheme.find("text");
  base_class::theme_type::const_iterator borderiter = scheme.find("border");

  // make the background box(es)
  FlatBox borderbox( thebox.height(),thebox.width(),-0.003);
  if( pl==Menu::HORIZONTAL )
  {
    if( hbgiter != scheme.end() )
      borderbox.color( hbgiter->second );
  }

  else
  {
    if( vbgiter != scheme.end() )
      borderbox.color( vbgiter->second );
  }

  osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  backgeode->setName("MK_OSG_CTS_backgroundbox_geode");
  backgeode->addDrawable( borderbox() );

  // make the text graphic functor
  Word word;
  word.draw_mode( osgText::Text::TEXT /*|osgText::Text::BOUNDINGBOX | osgText::Text::ALIGNMENT*/);
  word.height( this->letter_height() );
  word.text( txt );
  word.font( font() );
  if( textiter != scheme.end() )
    word.color( textiter->second );
  osg::ref_ptr<osg::Drawable> worddrawable = word();

  // make a boundingbox for convenience, used below, to move the word
  const osg::BoundingBox& bb = worddrawable->getBound();
  float wordheight = bb.yMax() - bb.yMin();
  float wordwidth = bb.xMax() - bb.xMin();

  // make the word geode
  osg::ref_ptr<osg::Geode> wordgeode = new osg::Geode();
  wordgeode->setName("MK_OSG_CTS_word_geode");
  wordgeode->addDrawable( worddrawable.get() );

  // move the word's node
  osg::ref_ptr<osg::MatrixTransform> wordmt = new osg::MatrixTransform();
  wordmt->setName("MK_OSG_CTS_word_xform");
  wordmt->addChild( wordgeode.get() );
  osg::Vec3 wordcorrection(-0.5*thebox.width()+thebox.height()+_border+_margin,
                           -0.5*thebox.height()+_border+_margin,
                           0.0f );
  if( pl==Menu::HORIZONTAL )
    wordcorrection[0] = -0.5*wordwidth;//+_border+_margin;
  wordmt->setMatrix( osg::Matrix::translate(wordcorrection) );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->setName("MK_OSG_CTS_leaf_group");
  group->addChild( backgeode.get() );
  group->addChild( wordmt.get() );

  typedef MenuKit::Bits<unsigned int> quickchecker;
  if( pl == Menu::HORIZONTAL && quickchecker::has(itembits, MARKED) )
  {
    Detail::Box inner(thebox.height()-2.0*_border,thebox.width()-2.0*_border);
    Border fence(thebox,inner,0.001);
    if( borderiter != scheme.end() )
      fence.color( borderiter->second );
    osg::ref_ptr<osg::Node> noexpandborder = fence();
    noexpandborder->setName("MK_OSG_noexpandborder");
    group->addChild( noexpandborder.get() );
  }

  // the total correction
  osg::MatrixTransform* mt = new osg::MatrixTransform();
  mt->setName("MK_OSG_CTS_leafxform");
  mt->addChild( group.get() );
  mt->setMatrix( osg::Matrix::translate(0.5*thebox.width(),-0.5*thebox.height(),0.0) );

  // escape if  null parent or parent has a horizontal layout
  if( !parent || pl==Menu::HORIZONTAL )
    return mt;

  ///\todo TODO: make other graphic boxes.  completed?
  // left side
  FlatBox sidebox(borderbox.height(),borderbox.height()+_border,-0.001);
  if( middleiter != scheme.end() )
    sidebox.color( middleiter->second );
  osg::ref_ptr<osg::Drawable> ld = sidebox();
  osg::ref_ptr<osg::Geode> lg = new osg::Geode();
  lg->setName("MK_OSG_CTS_leftleaf");
  lg->addDrawable( ld.get() );
  osg::ref_ptr<osg::MatrixTransform> lgmt = new osg::MatrixTransform();
  lgmt->setName("MK_OSG_CTS_leftleafxform");
  lgmt->addChild( lg.get() );
  float lxmov = -0.5*thebox.width() + 0.5*sidebox.width();
  lgmt->setMatrix( osg::Matrix::translate( osg::Vec3(lxmov,0.0,0.0) ) );
  group->addChild( lgmt.get() );

  // right side
  osg::ref_ptr<osg::MatrixTransform> rgmt = new osg::MatrixTransform();
  rgmt->setName("MK_OSG_CTS_rightleafxform");
  float rxmov = 0.5*thebox.width() - 0.5*sidebox.width() - _border;
  rgmt->setMatrix( osg::Matrix::translate( osg::Vec3(rxmov,0.0,0.0) ) );
  group->addChild( rgmt.get() );

  ///\todo TODO: add graphics for each supported bit! , Feb 20, 2005: finished now?
  typedef MenuKit::Bits<unsigned int> checker;
  if( checker::has(itembits, VisualThemeSkin::TOGGLE) )
  {
    // the border
    Detail::Box outy(thebox.height()-2.0*_border,thebox.height()-2.0*_border);
    Detail::Box inny(outy.height()-2.0*_border,outy.height()-2.0*_border);
    Border brdr(outy,inny,0.001);
    if( borderiter != scheme.end() )
      brdr.color( borderiter->second );

    lgmt->addChild( brdr() );

    if( checker::has(itembits, CHECKED) )
    {
      FlatBox markclosed(0.5*inny.height(),0.5*inny.width(),0.002);
      if( borderiter != scheme.end() )
        markclosed.color( borderiter->second );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      osg::ref_ptr<osg::Geode> mbgeo = new osg::Geode();
      mbgeo->setName("MK_OSG_CTS_markbox");
      mbgeo->addDrawable( mcdraw.get() );
      lgmt->addChild( mbgeo.get() );
    }
  }  // end of TOGGLE add-on

  if( checker::has(itembits, RADIO) )
  {
    Disk markdisc(this->letter_height(),0.0);
    if( textiter != scheme.end() )
      markdisc.color( textiter->second );
    osg::ref_ptr<osg::Drawable> mddraw = markdisc();

    Disk markopen(0.75*markdisc.height(),0.001);
    if( middleiter != scheme.end() )
      markopen.color( middleiter->second );
    osg::ref_ptr<osg::Drawable> modraw = markopen();

    osg::ref_ptr<osg::Geode> mdgeo = new osg::Geode();
    mdgeo->setName("MK_OSG_CTS_radiobitsgeode");
    mdgeo->addDrawable( mddraw.get() );
    mdgeo->addDrawable( modraw.get() );

    if( checker::has(itembits, CHECKED) )
    {
      Disk markclosed(0.75*markopen.height(),0.002);
      if( textiter != scheme.end() )
        markclosed.color( textiter->second );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      mdgeo->addDrawable( mcdraw.get() );
    }

    lgmt->addChild( mdgeo.get() );
  }  // end of CHECKED add-on

  if( checker::has(itembits, MENU) )
  {
    Arrow mark(0.5*word.height(),0.0);
    if( textiter != scheme.end() )
      mark.color( textiter->second );

    osg::ref_ptr<osg::Drawable> md = mark();
    osg::ref_ptr<osg::Geode> mgeo = new osg::Geode();
    mgeo->setName("MK_OSG_CTS_menubitsgeode");
    mgeo->addDrawable( md.get() );

    if( !checker::has(itembits, EXPANDED) )
    {
      Arrow expander(0.5*mark.height(),0.001);
      if( pl==Menu::HORIZONTAL )
      {
        if( hbgiter != scheme.end() )
          expander.color( hbgiter->second );
      }
      else
      {
        if( vbgiter != scheme.end() )
          expander.color( vbgiter->second );
      }
      osg::ref_ptr<osg::Drawable> exdraw = expander();
      mgeo->addDrawable( exdraw.get() );
    }

    else  // it is expanded and children are shown
    {
      if( parent->layout()==Menu::VERTICAL )  // only for vertical parents
      {
        Detail::Box inner(thebox.height()-2.0*_border,thebox.width()-2.0*_border);
        Border fence(thebox,inner,0.001);
        if( borderiter != scheme.end() )
          fence.color( borderiter->second );
        osg::ref_ptr<osg::Node> fencenode = fence();
        mt->addChild( fencenode.get() );
      }
    }

    ///\todo TODO: if expanded and parent is vertical, draw border

    rgmt->addChild( mgeo.get() );
  }  // end of MENU add-on

  if( checker::has(itembits , MARKED) )
  {
    Detail::Box inner(thebox.height()-2.0*_border,thebox.width()-2.0*_border);
    Border fence(thebox,inner,0.001);
    if( borderiter != scheme.end() )
      fence.color( borderiter->second );
    osg::ref_ptr<osg::Node> fencenode = fence();
    mt->addChild( fencenode.get() );
  }  // end of MARKED add-on

  return mt;
}

///\todo TODO: support vertical separators for horizontal menus, low priority
osg::Node* VisualThemeSkin::_separator_graphic(MenuKit::Menu::Layout pl)
{
  FlatBox bgbox(this->separator_height(),
                this->graphic_width(),
                -0.002);

  const base_class::ModeMap& mmap = this->mode_map();
  base_class::ModeMap::const_iterator modeiter = mmap.find( this->mode() );

  if( modeiter == mmap.end() )
  {
    ///\todo implement exception code here
  }
  base_class::ModeMap::value_type modetheme = *modeiter;
  base_class::ModeMap::value_type::second_type scheme = modetheme.second;

  base_class::theme_type::const_iterator middleiter = scheme.find("middle");
  base_class::theme_type::const_iterator hbgiter = scheme.find("horizontal_background");
  base_class::theme_type::const_iterator vbgiter = scheme.find("vertical_background");
  base_class::theme_type::const_iterator textiter = scheme.find("text");
  //base_class::theme_type::const_iterator specialiter = scheme.find("special");
  //base_class::theme_type::const_iterator borderiter = scheme.find("border");

  if( pl==MenuKit::Menu::HORIZONTAL )
  {
    if( hbgiter != scheme.end() )
      bgbox.color( hbgiter->second );
  }
  else
  {
    if( vbgiter != scheme.end() )
      bgbox.color( vbgiter->second );
  }
  osg::ref_ptr<osg::Drawable> bgdraw = bgbox();

  // make the background geode and add the geometry
  osg::ref_ptr<osg::Geode> bggeo = new osg::Geode();
  bggeo->setName("MK_OSG_CTS_separator_background_geode");
  bggeo->addDrawable( bgdraw.get() );

  // make the left box
  FlatBox leftbox(this->separator_height(),
                  this->_total_height()+_border,
                  -0.001);
  if( middleiter != scheme.end() )
    leftbox.color( middleiter->second );
  osg::ref_ptr<osg::Drawable> leftboxdraw = leftbox();

  // add the left box's geometry to a node
  osg::ref_ptr<osg::Geode> leftgeo = new osg::Geode();
  leftgeo->setName("MK_OSG_CTS_separator_leftgeo");
  leftgeo->addDrawable( leftboxdraw.get() );

  // move the left box node
  osg::ref_ptr<osg::MatrixTransform> leftxform = new osg::MatrixTransform();
  leftxform->setName("MK_OSG_CTS_separator_leftxform");
  leftxform->addChild( leftgeo.get() );
  leftxform->setMatrix( osg::Matrix::translate( osg::Vec3(-0.5*bgbox.width()+0.5*leftbox.width(),
                                                          0.0,
                                                          0.0) ) );

  // make the colored stripe geometry
  FlatBox stripe(0.5*(this->separator_height()),
                 this->graphic_width()-leftbox.width()-_margin-_border,
                 -0.001);
  if( textiter != scheme.end() )
    stripe.color( textiter->second );
  osg::ref_ptr<osg::Drawable> stripedraw = stripe();

  // add the stripe geometry to a node
  osg::ref_ptr<osg::Geode> stripegeo = new osg::Geode();
  stripegeo->setName("MK_OSG_CTS_separator_stripe_geode");
  stripegeo->addDrawable( stripedraw.get() );

  // move the stripe node
  osg::ref_ptr<osg::MatrixTransform> xs = new osg::MatrixTransform();
  xs->setName("MK_OSG_CTS_separator_stripe_xform");
  xs->addChild( stripegeo.get() );

  // use the equation for a stripe, offset for the left box
  xs->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*bgbox.width()-0.5*stripe.width(),0.0,0.0) ) );

  // add the parts
  osg::MatrixTransform* group = new osg::MatrixTransform();
  group->setName("MK_OSG_CTS_separator_xform");
  group->addChild( bggeo.get() );
  group->addChild( leftxform.get() );
  group->addChild( xs.get() );

  // position the graphic
  group->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*bgbox.width(),
                                                      -0.5*bgbox.height(),
                                                      0.0) ) );
  return group;
}

float VisualThemeSkin::height(const Menu& m) const
{
  if( m.separator() )
    return this->separator_height();
  else
    return this->_total_height();
}

float VisualThemeSkin::height(const Button& b) const
{
  if( b.separator() )
    return this->separator_height();
  else
    return this->_total_height();
}

float VisualThemeSkin::height(const Item* i) const
{
  if( i->separator() )
    return this->separator_height();
  else
    return this->_total_height();
}

/// wraps the equation for the graphic's total height
float VisualThemeSkin::_total_height() const
{
  return( (this->letter_height()) + 2.0*(_border+_margin) );
}

float VisualThemeSkin::width(const Menu& m) const
{
  return( _item_width( m.text() , m.parent() ) );
}

float VisualThemeSkin::width(const Button& b) const
{
  return( _item_width(b.text(), b.parent()) );
}

float VisualThemeSkin::width(const Item* i) const
{
  return( _item_width(i->text(), i->parent()) );
}

// wraps the equation for the graphic's total width
float VisualThemeSkin::_item_width(const std::string& s,const Menu* parent) const
{
  // check for text content
  if( s.empty() )
    return( 0.0 );

  // use graphics engine to calculate width of displayed text
  float wordwidth = this->_word_width( s );

  // check for top-level item
  Menu::Layout pl;
  if( !parent )
    pl = Menu::HORIZONTAL;
  else
    pl = parent->layout();

  // --- LAYOUT correcting --- //

  // the eqation for the width of a horizontal graphic
  // 2 margins,   2 x _margin
  // 2 borders,   2 x _border
  // 1 text box,  1 x graphic text width
  if( pl == Menu::HORIZONTAL )
    return( wordwidth + 2.0*(_margin+2.0*_border) );

  ///\todo TODO: When calculating this equation, include more spacing between pieces with _margin for better appearance, and use this new distance for positioning in graphic creation.
  // equation for the width of a vertical graphic:
  // 2 side boxes: 2 x _box's width (which is the height of the box + 2 x border)
  // 1 text box:   1 x  graphic text width
  // 2 margins:    2 x _margin
  else
    return( 2.0*(this->_total_height()+2.0*_border+_margin) + wordwidth );
}

float VisualThemeSkin::_word_width(const std::string& s) const
{
  Word word;
  word.height( this->letter_height() );
  word.text( s );
  const osgText::Font* cf = this->font();
  osgText::Font* f = const_cast<osgText::Font*>( cf );
  if( f )
    word.font( f );
  word.draw_mode( osgText::Text::BOUNDINGBOX );

  // use the functor to make the graphics
  osg::ref_ptr<osg::Drawable> drawable = word();

  // make a box for convenience, used below to move the word
  const osg::BoundingBox& bb = drawable->getBound();
  return( bb.xMax() - bb.xMin() );
}

#endif
