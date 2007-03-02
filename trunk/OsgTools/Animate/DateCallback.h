
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_ANIMATE_DATE_CALLBACK_H__
#define __OSGTOOLS_ANIMATE_DATE_CALLBACK_H__

#include "OsgTools/Export.h"
#include "OsgTools/Animate/Settings.h"
#include "OsgTools/Animate/Date.h"
#include "OsgTools/Animate/Functor.h"

#include "osg/NodeCallback"

#include <vector>

namespace OsgTools
{
  namespace Animate
  {
    class OSG_TOOLS_EXPORT DateCallback : public osg::NodeCallback
    {
    public:
      typedef osg::NodeCallback BaseClass;

      DateCallback ( OsgTools::Animate::Settings* settings, const OsgTools::Animate::Date& first, const OsgTools::Animate::Date& last );

      virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

      void addFunctor ( OsgTools::Animate::Functor* functor );
      void removeFunctor ( OsgTools::Animate::Functor* functor );

    protected:
      // Use reference counting;
      virtual ~DateCallback();

      // Do not use.
      DateCallback();
      DateCallback ( const DateCallback& );
      DateCallback& operator=( const DateCallback& );

      OsgTools::Animate::Settings::RefPtr _settings;
      OsgTools::Animate::Date _first;
      OsgTools::Animate::Date _last;

      typedef std::vector < OsgTools::Animate::Functor::RefPtr > Functors;
      Functors _functors;
    };
  }
}


#endif // __OSGTOOLS_ANIMATE_DATE_CALLBACK_H__
