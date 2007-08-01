
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_border_h_
#define _menukit_osg_border_h_

#include "MenuKit/OSG/DrawableFunctor.h"
#include "MenuKit/OSG/FlatBox.h"
#include "MenuKit/Box.h"

#include "osg/Geode"
#include "osg/MatrixTransform"

namespace MenuKit
{

  namespace OSG
  {

    class Border
    {
    public:
      Border(const Detail::Box& out,const Detail::Box& in,float d):
        _outer(out), _inner(in), _depth(d), _color(osg::Vec4(0.0,0.0,0.0,1.0))
      {}

      Border(const Border& b):
        _outer(b._outer), _inner(b._inner), _depth(b._depth), _color(b._color)
      {}

      Border& operator =(const Border& b)
      {
        _outer = b._outer;
        _inner = b._inner;
        _depth = b._depth;
        _color = b._color;
        return *this;
      }
      virtual ~Border() {}

      osg::Node* operator ()();

      void outer(const Detail::Box& b) { _outer = b; }
      const Detail::Box& outer() const { return _outer; }
      Detail::Box& outer() { return _outer; }

      void inner(const Detail::Box& b) { _inner = b; }
      const Detail::Box& inner() const { return _inner; }
      Detail::Box& inner() { return _inner; }

      void depth(float d) { _depth  = d; }
      float depth() const { return _depth; }

      void color(const osg::Vec4& c) { _color = c; }
      const osg::Vec4& color() const { return _color; }

    private:
      Detail::Box _outer, _inner;
      float _depth;
      osg::Vec4 _color;
    };

    inline osg::Node* Border::operator ()()
    {
      // need to make 4 FlatBoxes, but not have them overlap
      float vert = _outer.height()-_inner.height();
      FlatBox topbottom(vert,_outer.width(),_depth);
      topbottom.color( _color );

      osg::ref_ptr<osg::Geode> topgeode = new osg::Geode();
      topgeode->addDrawable( topbottom() );
      osg::ref_ptr<osg::MatrixTransform> top = new osg::MatrixTransform();
      top->addChild( topgeode.get() );
      top->setMatrix( osg::Matrix::translate( osg::Vec3(0.0,0.5*_outer.height()-0.5*vert,0.0) ) );

      osg::ref_ptr<osg::Geode> botgeode = new osg::Geode();
      botgeode->addDrawable( topbottom() );
      osg::ref_ptr<osg::MatrixTransform> bot = new osg::MatrixTransform();
      bot->addChild( botgeode.get() );
      bot->setMatrix( osg::Matrix::translate( osg::Vec3(0.0,-0.5*_outer.height()+0.5*vert,0.0) ) );

      float hori = _outer.width() - _inner.width();
      FlatBox rightleft(_inner.height(),hori,_depth);
      rightleft.color( _color );

      osg::ref_ptr<osg::Geode> rightgeode = new osg::Geode();
      rightgeode->addDrawable( rightleft() );
      osg::ref_ptr<osg::MatrixTransform> right = new osg::MatrixTransform();
      right->addChild( rightgeode.get() );
      right->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*_outer.width()-0.5*hori,0.0,0.0) ) );

      osg::ref_ptr<osg::Geode> leftgeode = new osg::Geode();
      leftgeode->addDrawable( rightleft() );
      osg::ref_ptr<osg::MatrixTransform> left = new osg::MatrixTransform();
      left->addChild( leftgeode.get() );
      left->setMatrix( osg::Matrix::translate( osg::Vec3(-0.5*_outer.width()+0.5*hori,0.0,0.0) ) );

      osg::Group* group = new osg::Group();
      group->addChild( top.get() );
      group->addChild( bot.get() );
      group->addChild( right.get() );
      group->addChild( left.get() );

      return group;
    }

  };

};

#endif
