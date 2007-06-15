
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ADD_LAYER_JOB_H__
#define __MINERVA_CORE_ADD_LAYER_JOB_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Scene/SceneManager.h"
#include "Minerva/Core/Layers/Layer.h"

#include "Usul/Jobs/Job.h"

namespace Minerva {
namespace Core {
namespace Viz {

class MINERVA_EXPORT AddLayerJob : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;

  AddLayerJob ( Minerva::Core::Scene::SceneManager* manager, 
		Minerva::Core::Layers::Layer* layer,
		Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  // Use reference counting.
  virtual ~AddLayerJob();

  // Do the work here.
  virtual void       _started();
  virtual void       _finished();

private:

  Minerva::Core::Layers::Layer::RefPtr _layer;
  Minerva::Core::Scene::SceneManager::RefPtr _manager;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};

}
}
}


#endif // __MINERVA_CORE_ADD_LAYER_JOB_H__
