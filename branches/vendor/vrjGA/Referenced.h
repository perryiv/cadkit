#ifndef _VRJGA_REFERENCED_
#define _VRJGA_REFERENCED_

#include "vrjGA.h"

#include "boost/intrusive_ptr.hpp"

namespace vrjGA
{

  class VRJ_GA_EXPORT Referenced
  {
  public:
    void ref();
    void unref();

  protected:
    Referenced();
    virtual ~Referenced();

  private:
    unsigned int _refCount;
  };

};

namespace boost
{
  void VRJ_GA_EXPORT intrusive_ptr_add_ref(vrjGA::Referenced*);
  void VRJ_GA_EXPORT intrusive_ptr_release(vrjGA::Referenced*);
};

#define VRJGA_DECLARE_POINTER(classname) \
  typedef boost::intrusive_ptr<classname> Ptr


#endif
