#ifndef _PF_LOD_H_
#define _PF_LOD_H_

#include "Performer/pf/pfGroup.h"
#include "Performer/pr/pfLinMath.h"

#include <vector>


class pfLOD : public pfGroup
{
public:

  pfLOD() : pfGroup(){}

  virtual const char *          className() const { return "pfLOD"; }

  void                          setCenter ( pfVec3 &center ) { _center.setValue ( center ); }
  void                          setRange ( int index, float range );
  
  virtual void                  write ( std::ostream &out ) const;

protected:

  pfVec3 _center;
  std::vector<float> _ranges;

  virtual ~pfLOD(){}

  virtual pfNode *_clone() { return new pfLOD; }
};


inline void pfLOD::setRange ( int index, float range )
{
  if ( index < 0 || ((unsigned int) index ) >= _ranges.size() )
    _ranges.resize ( index + 1, 0.0f );
  _ranges[index] = range;
}


inline void pfLOD::write ( std::ostream &out ) const
{
  out << _indent << this->className() << std::endl;
  pfGroup::write ( out );
}

#endif
