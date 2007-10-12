#ifndef _PF_DCS_H_
#define _PF_DCS_H_

#include "Performer/pf/pfSCS.h"

class pfMatrix;


class pfDCS : public pfSCS
{
public:

  pfDCS() : pfSCS ( pfMatrix ( SL_MATRIX_44_IDENTITY_F ) ){}

  virtual const char *          className() const { return "pfDCS"; }

  void                          setMat ( const pfMatrix &matrix ){}

  virtual void                  write ( std::ostream &out ) const;

protected:

  virtual ~pfDCS(){}

  virtual pfNode *              _clone() { return new pfDCS; }
};


inline void pfDCS::write ( std::ostream &out ) const
{
  pfSCS::write ( out );
}

#endif
