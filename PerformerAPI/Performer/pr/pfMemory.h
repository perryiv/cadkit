#ifndef _PF_MEMORY_H_
#define _PF_MEMORY_H_

#include "Translators/Jupiter2Performer/TrJt2PfApi.h"

#include <iostream>
#include <string>

class pfMemory
{
public:

  pfMemory() : _ref ( 0 ){}
  
  virtual const char *          className() const = 0;

  int                           getRef() const { return _ref; }

  void                          ref() { ++_ref; }
  void                          unref() { --_ref; if ( 0 == _ref ) delete this; }

  virtual void                  write ( std::ostream &out ) const = 0;

protected:
  
  static std::string _indent;
  int _ref;

  virtual ~pfMemory(){}
};


inline void pfMemory::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_ref = " << _ref << std::endl;
}


#endif
