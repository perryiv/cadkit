#ifndef c6c0d98f_edae_4e5a_96ad_b8bc7415116a
#define c6c0d98f_edae_4e5a_96ad_b8bc7415116a

#include "Translators/Jupiter2Performer/TrJt2PfApi.h"


class pfMemory
{
public:
  pfMemory() : _ref ( 0 ){}
  void ref() { ++_ref; }
  void unref() { --_ref; if ( 0 == _ref ) delete this; }
  int getRef() { return _ref; }
protected:
  int _ref;
  virtual ~pfMemory(){}
};

namespace CadKit
{
TR_JT_2_PF_API void _incrementPointerReferenceCount ( pfMemory *p );
TR_JT_2_PF_API void _decrementPointerReferenceCount ( pfMemory *p );
};

#endif
