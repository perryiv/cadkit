
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MAGRATHEA_LAYERMANIPULATOR_H__
#define _MAGRATHEA_LAYERMANIPULATOR_H__

#include "Export.h"

#include "osg/Referenced"
#include "osg/ref_ptr"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"

namespace Magrathea
{
  class MAGRATHEA_EXPORT LayerManipulator : public osg::Referenced
  {
  public:
    /*
    enum OperationType
    {
      TOP = 1, 
      REFERENCE,
      OPACITY, 
      HORIZONTAL_SWIPE,
      VERTICAL_SWIPE,
      BOX_SWIPE,
      CIRCLE_SWIPE,
      ABSOLUTE_DIFFERENCE,
      FALSE_COLOR_REPLACEMENT
    };
    */
    LayerManipulator() :
      osg::Referenced()
    {      
    }   

    float                                         opacity() const ;
    void                                          opacity( const float& );

    void                                          setLand( ossimPlanetLand* );
    
    /*
      void                                        top(); 
      void                                        reference();
      void                                        horizontalSwipe( const float& );
      void                                        verticalSwipe( const float& );
      void                                        boxSwipe( const float& );    
      void                                        circleSwipe( const float& );
      void                                        absoluteDifference();
      void                                        falseColorReplacement();
    */

  protected:

    virtual ~LayerManipulator()
    {
    }

  protected:
    osg::ref_ptr< ossimPlanetLand >               _land;

  };
}

#endif // _MAGRATHEA_LAYERMANIPULATOR_H__
