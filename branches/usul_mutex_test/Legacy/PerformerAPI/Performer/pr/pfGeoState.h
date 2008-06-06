#ifndef _PF_GEO_STATE_H_
#define _PF_GEO_STATE_H_

#include "Performer/pr/pfMaterial.h"

#include "Standard/SlRefPtr.h"


#define PFSTATE_TRANSPARENCY    1034807790
#define PFSTATE_ENLIGHTING      1034807791
#define PFSTATE_FRONTMTL        1034807792
#define PFSTATE_BACKMTL         1034807793

#define PF_ON    true
#define PF_OFF   false
#define PFTR_ON  true
#define PFTR_OFF false


class pfGeoState : public pfObject
{
public:

  pfGeoState() : pfObject(), 
    _transparency ( false ), 
    _lighting ( false ), 
    _frontMaterial ( 0x0 ), 
    _backMaterial ( 0x0 ){}

  virtual const char *          className() const { return "pfGeoState"; }

  void                          setAttr ( int attribute, void *value );
  void                          setMode ( int mode, bool value );
  
  virtual void                  write ( std::ostream &out ) const;

protected:

  bool _transparency;
  bool _lighting;
  CadKit::SlRefPtr<pfMaterial> _frontMaterial;
  CadKit::SlRefPtr<pfMaterial> _backMaterial;

  virtual ~pfGeoState(){}
};


inline void pfGeoState::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_transparency = " << _transparency << std::endl;
  out << _indent << this->className() << "::_lighting     = " << _lighting << std::endl;

  out << _indent << this->className() << "::_frontMaterial = " << std::endl;
  
  if ( _frontMaterial.isValid() )
  {
    _indent += "  ";
    _frontMaterial->write ( out );
    _indent.resize ( _indent.size() - 2 );
  }

  out << _indent << this->className() << "::_backMaterial = " << std::endl;

  if ( _backMaterial.isValid() )
  {
    _indent += "  ";
    _backMaterial->write ( out );
    _indent.resize ( _indent.size() - 2 );
  }

  pfObject::write ( out );
}


inline void pfGeoState::setAttr ( int attribute, void *value )
{
  SL_ASSERT ( 0x0 != value );
  switch ( attribute )
  {
  case PFSTATE_FRONTMTL:
    _frontMaterial = ((pfMaterial *) value);
    break;
  case PFSTATE_BACKMTL:
    _backMaterial = ((pfMaterial *) value);
    break;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    break;
  }
}


inline void pfGeoState::setMode ( int mode, bool value )
{
  switch ( mode )
  {
  case PFSTATE_TRANSPARENCY:
    _transparency = value;
    break;
  case PFSTATE_ENLIGHTING:
    _lighting = value;
    break;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    break;
  }
}


#endif
