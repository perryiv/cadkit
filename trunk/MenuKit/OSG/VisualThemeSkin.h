#ifndef _menukit_osg_colorthemeskin_h_
#define _menukit_osg_colorthemeskin_h_

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

#include "MenuKit/OSG/osg_types.h"

/*
-----------------------------------------------|
|                                              |
| ----------              ----------           |
| |        |              |        |           |
| | middle |   T E X T    | middle |           |
| |        |              |        |           |
| ----------              ----------           |
|             background                       |
|----------------------------------------------|
               border
*/

namespace MenuKit
{

  namespace OSG
  {

    /** VisualThemeSkin
      * The VisualThemeSkin class is a simple implementation
      * of the ThemeSkin base class.  Its goal is to provide
      * Menu Items with a similar appearance to the Menu Items
      * in Microsoft's Visual Studio IDE.
      * 
      * This class is done with only an OSG implementation.
      * It is intended to be a leaf class, which has many
      * OSG specific methods.
      */
    class VisualThemeSkin : public ThemeSkin<osg_color_map>
    {
    public:
      typedef ThemeSkin<osg_color_map> base_class;
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

      VisualThemeSkin(): base_class(), _margin(0.2), _text(0.8), _border(0.02), _font(new osgText::Font()) {}

      virtual osg::Node* operator ()(const Menu& m);
      virtual osg::Node* operator ()(const Button& b);
      // TODO: virtual osg::Node* operator =(const Item& b);

      virtual float height(const Menu& m);
      virtual float height(const Button& b);
      virtual float height(const Item* i);

      virtual float width(const Menu& m);
      virtual float width(const Button& b);
      virtual float width(const Item* i);
      // TODO: virtual float width(const Button& b);

      void font(osgText::Font* f) { _font = f; }
      const osgText::Font* font() const { return _font.get(); }
      osgText::Font* font() { return _font.get(); }

      void border(float m) { _border=m; }
      float border() const { return _border; }

      void margin(float m) { _margin=m; }
      float margin() const { return _margin; }

      void text_ratio(float t) { _text=t; }
      float text_ratio() const { return _text; }

    protected:
      virtual ~VisualThemeSkin() {}

      float _word_width(const std::string& word);
      float _item_width(const std::string& w,const Menu* m);
      osg::Node* _item_graphic(const std::string& txt, const Menu* parent, ItemBits b);
      osg::Node* _separator_graphic();

      ItemBits _gather_bits(unsigned int menubits);

    private:
      ///\todo TODO: evaluate if this class is copyable
      // not implemented by design
      VisualThemeSkin(const VisualThemeSkin& s);//: base_class(s),
      //  _margin(s._margin), _text(s._text), _border(s._border), _font(ts._font)
      //{}

      VisualThemeSkin& operator= (const VisualThemeSkin& ct);
      //{
      //  base_class::operator =(ct);
      //  _margin = ct._margin;
      //  _text = ct._text;
      //  _border = ct._border;
      //  _font = ts._font;
      //  return( *this );
      //}

      float _margin,  // distance between box's edge to an add-on
            _border;  ///\todo TODO: rename this // must be an extra width parameter, don't remember
      float _text;    // percentage of box height
      osg::ref_ptr<osgText::Font> _font;
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
    node = _separator_graphic();

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
    node = _separator_graphic();

  else
  {
    ItemBits bits = this->_gather_bits( b.flags() );
    node = _item_graphic( b.text(), b.parent(), bits );
  }

  return( node );
}

VisualThemeSkin::ItemBits VisualThemeSkin::_gather_bits(unsigned int itembits)
{
  // TODO: set up any bits necessary
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
  Detail::Box thebox = this->box();

  Menu::Layout pl;
  if( parent )
    pl = parent->layout();
  else
    pl = Menu::HORIZONTAL;

  const base_class::theme_type& scheme = this->theme();
  base_class::theme_type::const_iterator middleiter = scheme.find("middle");
  base_class::theme_type::const_iterator backgrounditer = scheme.find("background");
  base_class::theme_type::const_iterator textiter = scheme.find("text");
  base_class::theme_type::const_iterator specialiter = scheme.find("special");
  base_class::theme_type::const_iterator borderiter = scheme.find("border");

  // make the background box(es)
  FlatBox borderbox( thebox.height(),thebox.width(),-0.003);
  if( pl==Menu::HORIZONTAL )
  {
    if( middleiter != scheme.end() )
      borderbox.color( middleiter->second );
  }

  else
  {
    if( backgrounditer != scheme.end() )
      borderbox.color( backgrounditer->second );
  }

  osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  backgeode->setName("MK_OSG_CTS_backgroundbox_geode");
  backgeode->addDrawable( borderbox() );

  // make the text graphic functor
  Word word;
  word.height( _text*(thebox.height()-2.0*_border) );
  word.text( txt );
  word.font( font() );
  if( textiter != scheme.end() )
    word.color( textiter->second );
  word.draw_mode( osgText::Text::TEXT );
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
  osg::Vec3 wordcorrection(-0.5*thebox.width()+thebox.height()+_border+_margin,-0.5*wordheight,0.0);
  if( pl==Menu::HORIZONTAL )
    wordcorrection[0] = -0.5*wordwidth;
  wordmt->setMatrix( osg::Matrix::translate(wordcorrection) );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->setName("MK_OSG_CTS_leaf_group");
  group->addChild( backgeode.get() );
  group->addChild( wordmt.get() );

  // the total correction
  osg::MatrixTransform* mt = new osg::MatrixTransform();
  mt->setName("MK_OSG_CTS_leafxform");
  mt->addChild( group.get() );
  mt->setMatrix( osg::Matrix::translate(0.5*thebox.width(),-0.5*thebox.height(),0.0) );

  // escape if  null parent
  if( !parent || pl==Menu::HORIZONTAL )  // valid parent  TODO: implement
  {
    ///\todo TODO: what is this? : osg::Vec3 nosidecorrection(-0.5*wordwidth,-0.5*wordheight,0.0);
    ///\todo TODO: what is this? : wordmt->setMatrix( osg::Matrix::translate( nosidecorrection ) );
    return mt;
  }

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
  if( checker::has(itembits, TOGGLE) )
  {
    if( checker::has(itembits, CHECKED) )
    {
      // the border
      Detail::Box outy(thebox.height()-2.0*_border,thebox.height()-2.0*_border);
      Detail::Box inny(outy.height()-2.0*_border,outy.height()-2.0*_border);
      Border brdr(outy,inny,0.001);
      if( specialiter != scheme.end() )
        brdr.color( specialiter->second );

      lgmt->addChild( brdr() );

      FlatBox markclosed(0.5*inny.height(),0.5*inny.width(),0.002);
      if( specialiter != scheme.end() )
        markclosed.color( specialiter->second );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      osg::ref_ptr<osg::Geode> mbgeo = new osg::Geode();
      mbgeo->setName("MK_OSG_CTS_markbox");
      mbgeo->addDrawable( mcdraw.get() );
      lgmt->addChild( mbgeo.get() );
    }
  }  // end of TOGGLE add-on

  if( checker::has(itembits, RADIO) )
  {
    Disk markdisc(_text*word.height(),0.0);
    if( textiter != scheme.end() )
      markdisc.color( textiter->second );
    osg::ref_ptr<osg::Drawable> mddraw = markdisc();

    Disk markopen(_text*markdisc.height(),0.001);
    if( middleiter != scheme.end() )
      markopen.color( middleiter->second );
    osg::ref_ptr<osg::Drawable> modraw = markopen();

    osg::ref_ptr<osg::Geode> mdgeo = new osg::Geode();
    mdgeo->setName("MK_OSG_CTS_radiobitsgeode");
    mdgeo->addDrawable( mddraw.get() );
    mdgeo->addDrawable( modraw.get() );

    if( checker::has(itembits, CHECKED) )
    {
      Disk markclosed(_text*markopen.height(),0.002);
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
      if( backgrounditer != scheme.end() )
        expander.color( backgrounditer->second );
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
osg::Node* VisualThemeSkin::_separator_graphic()
{
  FlatBox bgbox(this->separator().height(),
                this->box().width(),         ///\todo TODO: possible problem area?
                -0.002);

  const base_class::theme_type& scheme = this->theme();
  base_class::theme_type::const_iterator backgrounditer = scheme.find("background");
  base_class::theme_type::const_iterator middleiter = scheme.find("middle");
  base_class::theme_type::const_iterator textiter = scheme.find("text");

  if( backgrounditer != scheme.end() )
    bgbox.color( backgrounditer->second );
  osg::ref_ptr<osg::Drawable> bgdraw = bgbox();

  // make the background geode and add the geometry
  osg::ref_ptr<osg::Geode> bggeo = new osg::Geode();
  bggeo->setName("MK_OSG_CTS_separator_background_geode");
  bggeo->addDrawable( bgdraw.get() );

  // make the left box
  FlatBox leftbox(this->separator().height(),
                  this->box().height()+_border,
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
  FlatBox stripe(_text*(this->separator().height()),
                 this->box().width()-this->box().height()-2.0*_border-_margin,
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
  xs->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*leftbox.width()+_border+0.5*_margin,
                                                   0.0,0.0) ) );

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

float VisualThemeSkin::height(const Menu& m)
{
  if( m.separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

float VisualThemeSkin::height(const Button& b)
{
  if( b.separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

float VisualThemeSkin::height(const Item* i)
{
  if( i->separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

float VisualThemeSkin::width(const Menu& m)
{
  return( _item_width( m.text() , m.parent() ) );
}

float VisualThemeSkin::width(const Button& b)
{
  return( _item_width(b.text(), b.parent()) );
}

float VisualThemeSkin::width(const Item* i)
{
  return( _item_width(i->text(), i->parent()) );
}

float VisualThemeSkin::_item_width(const std::string& s,const Menu* parent)
{
  // check for text content
  if( s.empty() )
    return( 0.0 );

  // use graphics engine to calculate width of displayed text
  float wordwidth = this->_word_width( s );

  // check for top-level item
  if( !parent )
    return( wordwidth + 2.0*(_margin+_border) );

  // --- LAYOUT correcting --- //

  // the eqation for the width of a horizontal graphic
  // 2 margins,   2 x _margin
  // 1 text box,  1 x graphic text width
  if( parent->layout() == Menu::HORIZONTAL )
    return( wordwidth + 2.0*(_margin+_border) );

  /// TODO: When calculating this equation, include more spacing between pieces with _margin for better appearance, and use this new distance for positioning in graphic creation.
  // equation for the width of a vertical graphic:
  // 2 side boxes: 2 x _box's width (which is the height of the box - 2 x margin)
  // 1 text box:   1 x  graphic text width
  // 2 margins:    2 x _margin
  else
    return( 2.0*(this->box().height()+_border+_margin) + wordwidth );
}

float VisualThemeSkin::_word_width(const std::string& s)
{
  Word word;
  word.height( _text*(this->box().height()) );
  word.text( s );
  word.font( font() );
  word.draw_mode( osgText::Text::BOUNDINGBOX );

  // use the functor to make the graphics
  osg::ref_ptr<osg::Drawable> drawable = word();

  // make a box for convenience, used below to move the word
  const osg::BoundingBox& bb = drawable->getBound();
  return( bb.xMax() - bb.xMin() );
}

#endif
