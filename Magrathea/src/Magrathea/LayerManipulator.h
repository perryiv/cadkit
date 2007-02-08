
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_LAYERMANIPULATOR_H__
#define __MAGRATHEA_LAYERMANIPULATOR_H__

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

    LayerManipulator() :
      osg::Referenced()
    {      
    }   

    float                                         opacity() const ;
    void                                          opacity( const float& );

    float                                         horizontalSwipe() const;
    void                                          horizontalSwipe( const float& );    

    float                                         verticalSwipe() const;
    void                                          verticalSwipe( const float& );  

    /*
    float                                         boxSwipe() const;
    void                                          boxSwipe( const float& );    
      
    float                                         circleSwipe() const;
    void                                          circleSwipe( const float& );  
    */

    void                                          top(); 
    
    void                                          reference();
    
    void                                          absoluteDifference();

    void                                          falseColorReplacement();

    void                                          setLand( ossimPlanetLand* );    
 
    float                                         shaderVal() const;
    void                                          shaderVal( const float& );    

  protected:

    virtual ~LayerManipulator()
    {
    }

  protected:
    osg::ref_ptr< ossimPlanetLand >               _land;

  };
}

#endif // __MAGRATHEA_LAYERMANIPULATOR_H__
