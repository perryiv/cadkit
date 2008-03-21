
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "WmsLayerItem.h"

#include "Usul/Convert/Convert.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerItem::WmsLayerItem ( XmlTree::Node* node, QTreeWidget *parent ) : BaseClass ( parent ),
  _node ( node ),
  _extents( -180, -90, 180, 90 )
{
  if ( _node.valid() )
  {
    typedef XmlTree::Node::Children    Children;
    typedef XmlTree::Node::Attributes  Attributes;
    typedef Usul::Convert::Type<std::string,double> ToDouble;
    
    Children nameNode ( _node->find ( "Name", false ) );
    Children titleNode ( _node->find ( "Title", false ) );
    
    std::string name ( nameNode.empty() ? "" : nameNode.front()->value() );
    std::string title ( titleNode.empty() ? "" : titleNode.front()->value() );
    
    this->setText ( 0, name.c_str() );
    this->setText ( 1, title.c_str() );
    
    Children bbNode ( _node->find ( "LatLonBoundingBox", false ) );
    if ( false == bbNode.empty() )
    {
      XmlTree::Node::ValidRefPtr bb ( bbNode.front() );
      Attributes& attributes ( bb->attributes() );
      
      Extents e (  ToDouble::convert ( attributes["minx"] ),
                   ToDouble::convert ( attributes["miny"] ),               
                   ToDouble::convert ( attributes["maxx"] ),
                   ToDouble::convert ( attributes["maxy"] ) );
      _extents = e;
    }
    
    Children stylesNodes ( _node->find ( "Styles", true ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerItem::~WmsLayerItem()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerItem::Extents WmsLayerItem::extents () const
{
  return _extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string WmsLayerItem::name() const
{
  return this->text ( 0 ).toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the style.
//
///////////////////////////////////////////////////////////////////////////////

std::string WmsLayerItem::style() const
{
  return "";
}
