/*
 *  GlassBoundingBox.h
 *  OsgTools
 *
 *  Created by Michael A Jackson on 7/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _OSG_TOOLS_GLASS_BOUNDING_BOX_H_
#define _OSG_TOOLS_GLASS_BOUNDING_BOX_H_

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Array"

namespace osg { class Group; class Node; class Drawable; }

namespace OsgTools {
  
  
class OSG_TOOLS_EXPORT GlassBoundingBox : public Usul::Base::Referenced
{
public:
    // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GlassBoundingBox );
  
  //Construction
  GlassBoundingBox();
  GlassBoundingBox(const float xMin, const float yMin, float const zMin,
                   const float xMax, const float yMax, float const zMax);
  
  //Get the OSG Node to render
  void         addBoundingGlass( osg::Group *root, bool outline, bool glass, bool numbers = false );
  
    //Get/Set the Max and Min Values for the XYZ coordinates
  void                    xMax ( float );
  float                   xMax ();
  void                    xMin ( float );
  float                   xMin ();
  void                    yMax ( float );
  float                   yMax ();
  void                    yMin ( float );
  float                   yMin ();
  void                    zMax ( float );
  float                   zMax ();
  void                    zMin ( float );
  float                   zMin ();



protected:

  osg::Node*          _makeBoxOutline ( osg::Vec3Array& vertices );
  osg::Node*          _makeBoxGlass   ( osg::Vec3Array& vertices );
  osg::Node*          _makeNumbers    ( );
  osg::Drawable*      _makeNumber     ( const std::string& name, const osg::Vec3& pos, const osg::Vec4& color, float size );

  //init each of the six sides of the Cube;
  osg::Node*          _initXYMax();
  osg::Node*          _initXYMin();
  osg::Node*          _initXZMax();
  osg::Node*          _initXZMin();
  osg::Node*          _initYZMax();
  osg::Node*          _initYZMin();

private:
  float _xMax;
  float _xMin;
  float _yMax;
  float _yMin;
  float _zMax;
  float _zMin;
  
  //Color for each Face
  
  //Other Stuff we might need.
};


} // namespace OsgTools


#endif // _OSG_TOOLS_GLASS_BOUNDING_BOX_H_


