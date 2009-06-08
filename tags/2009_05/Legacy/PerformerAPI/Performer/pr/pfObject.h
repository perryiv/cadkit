#ifndef _PF_OBJECT_H_
#define _PF_OBJECT_H_

#include "Performer/pr/pfMemory.h"

class pfObject : public pfMemory
{
public:

  virtual const char *          className() const { return "pfObject"; }

  virtual void                  write ( std::ostream &out ) const;

protected:

  pfObject() : pfMemory(){}
  virtual ~pfObject(){}
};

inline void pfObject::write ( std::ostream &out ) const
{
  pfMemory::write ( out );
}

#endif
