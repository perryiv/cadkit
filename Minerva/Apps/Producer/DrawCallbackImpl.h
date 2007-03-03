
#ifndef __DRAW_CALLBACK_IMPL_H__
#define __DRAW_CALLBACK_IMPL_H__

#include "Minerva/Core/DrawCallback.h"

namespace osgProducer { class Viewer; }

class DrawCallbackImpl : public Minerva::Core::DrawCallback
{
public:
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DrawCallbackImpl );

  DrawCallbackImpl ( osgProducer::Viewer& viewer );

  virtual void draw();

protected:
  virtual ~DrawCallbackImpl();

private:
  osgProducer::Viewer &_viewer;
};

#endif // __DRAW_CALLBACK_IMPL_H__


