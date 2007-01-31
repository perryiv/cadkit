
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSSIM_OSSIM_PLANET_LAYER_OPERATION_H__
#define __OSSIM_OSSIM_PLANET_LAYER_OPERATION_H__

#include "Export.h"

#include "osg/Referenced"
#include "osg/ref_ptr"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"

namespace Magrathea
{
  class MAGRATHEA_EXPORT LayerOperation : public osg::Referenced
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
    LayerOperation() :
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

    virtual ~LayerOperation()
    {
    }

  protected:
    osg::ref_ptr< ossimPlanetLand >               mLand;

  };
}

#endif // __OSSIM_OSSIM_PLANET_LAYER_OPERATION_H__
