
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/DataObject.h"

#include "Minerva/Layers/Kml/Factory.h"

#include "XmlTree/Document.h"
#include "XmlTree/Node.h"

#include "gtest/gtest.h"


static const char* multiGeometryString01 = 
{
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<kml xmlns=\"http://earth.google.com/kml/2.0\">"
  "<MultiGeometry>"
    "<LineString>"
      "<altitudeMode>relativeToGround</altitudeMode>"
      "<coordinates>"
        "1,2,3, 4,5,6"
      "</coordinates>"
    "</LineString>"
  "</MultiGeometry>"
  "</kml>"
};


TEST(KmlParseMultiGeometryTest,OneGeometry)
{
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->loadFromMemory ( multiGeometryString01 );

  Minerva::Core::Data::DataObject::RefPtr object ( new Minerva::Core::Data::DataObject );
  Minerva::Layers::Kml::Factory::instance().createMultiGeometry ( *document->children().at ( 0 ), *object );

  EXPECT_EQ ( 1, object->geometries().size() );
}


static const char* multiGeometryString02 = 
{
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<kml xmlns=\"http://earth.google.com/kml/2.0\">"
  "<MultiGeometry>"
    "<LineString>"
      "<altitudeMode>relativeToGround</altitudeMode>"
      "<coordinates>"
        "1,2,3, 4,5,6"
      "</coordinates>"
    "</LineString>"
    "<Point>"
      "<coordinates>1,2,3</coordinates>"
    "</Point>"
  "</MultiGeometry>"
  "</kml>"
};


TEST(KmlParseMultiGeometryTest,TwoGeometries)
{
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->loadFromMemory ( multiGeometryString02 );

  Minerva::Core::Data::DataObject::RefPtr object ( new Minerva::Core::Data::DataObject );
  Minerva::Layers::Kml::Factory::instance().createMultiGeometry ( *document->children().at ( 0 ), *object );

  EXPECT_EQ ( 2, object->geometries().size() );
}
