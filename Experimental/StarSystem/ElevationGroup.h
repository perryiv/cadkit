
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_ELEVATION_GROUP_H__
#define __STAR_SYSTEM_ELEVATION_GROUP_H__

#include "StarSystem/Export.h"
#include "StarSystem/RasterGroup.h"

namespace osg { class Image; }

namespace StarSystem {

class STAR_SYSTEM_EXPORT ElevationGroup : public RasterGroup
{
public:

  typedef RasterGroup BaseClass;

  USUL_DECLARE_REF_POINTERS ( ElevationGroup );

  ElevationGroup();
  
  /// Clone.
  virtual IUnknown*     clone() const;

protected:

  virtual ~ElevationGroup();

  ElevationGroup ( const ElevationGroup& );
  
  virtual ImagePtr                _createBlankImage ( unsigned int width, unsigned int height ) const;
  virtual void                    _compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, float alpha, Usul::Jobs::Job * );

private:
  ElevationGroup& operator= ( const ElevationGroup& );
  
  SERIALIZE_XML_DEFINE_MEMBERS ( ElevationGroup );
};

}

#endif // __STAR_SYSTEM_ELEVATION_GROUP_H__
