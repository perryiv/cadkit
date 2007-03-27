#ifndef _PF_SCS_H_
#define _PF_SCS_H_

#include "Performer/pf/pfGroup.h"
#include "Performer/pr/pfLinMath.h"

#include "Standard/SlMatrix44IO.h"


class pfSCS : public pfGroup
{
public:

  pfSCS ( const pfMatrix &matrix ) : pfGroup(), _m ( matrix ){}

  virtual const char *          className() const { return "pfSCS"; }

  virtual void                  write ( std::ostream &out ) const;

protected:

  pfMatrix _m;

  virtual ~pfSCS(){}

  virtual pfNode *              _clone() { return new pfSCS ( _m ); }
};


inline void pfSCS::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_m = " << _m[0] << " " << _m[4] << " " << _m[8]  << " " << _m[12] << std::endl;
  out << _indent << "            "                 << _m[1] << " " << _m[5] << " " << _m[9]  << " " << _m[13] << std::endl;
  out << _indent << "            "                 << _m[2] << " " << _m[6] << " " << _m[10] << " " << _m[14] << std::endl;
  out << _indent << "            "                 << _m[3] << " " << _m[7] << " " << _m[11] << " " << _m[15] << std::endl;
  pfGroup::write ( out );
}


#endif
