#include "vrjGaPrecompiled.h"
#include "Referenced.h"

using namespace vrjGA;

Referenced::Referenced(): _refCount(0)
{
}

Referenced::~Referenced()
{
}

void Referenced::ref()
{
  ++_refCount;
}

void Referenced::unref()
{
  if( 0==--_refCount )
    delete this;
}


namespace boost
{
  void intrusive_ptr_add_ref(vrjGA::Referenced* ptr)
  {
    assert(ptr);
    ptr->ref();
  }

  void intrusive_ptr_release(vrjGA::Referenced* ptr)
  {
    assert(ptr);
    ptr->unref();
  }
};
