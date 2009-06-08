///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to set the material of osg types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osg_tools_material_setter_h_
#define _osg_tools_material_setter_h_

#include "OsgTools/Export.h"

#include "osg/Vec4"

namespace osg
{
  class StateSet;
  class Drawable;
  class Node;
};

namespace OsgTools
{

  class OSG_TOOLS_EXPORT MaterialSetter
  {
  public:
    MaterialSetter();
    MaterialSetter(const MaterialSetter& ms);
    virtual ~MaterialSetter();

    MaterialSetter& operator =(const MaterialSetter& cs);

    enum Face
    {
      BACK=0,
      FRONT=1
    };

    void operator ()(osg::Drawable* d) const;
    void operator ()(osg::Node* g) const;

    void adjust_stateset(osg::StateSet* ss) const;

    // TODO: test setters/getters
    void ambient(const osg::Vec4& c,Face f=FRONT) { f ? _af=c : _ab=c; }
    const osg::Vec4& ambient(Face f=FRONT);

    void diffuse(const osg::Vec4& c,Face f=FRONT) { f ? _df=c : _db=c; }
    const osg::Vec4& diffuse(Face f=FRONT);

    void emissive(const osg::Vec4& c,Face f=FRONT) { f ? _ef=c : _eb=c; }
    const osg::Vec4& emissive(Face f=FRONT);

    void shininess(float c,Face f=FRONT) { f ? _shf=c : _shb=c; }
    float shininess(Face f=FRONT);

    void specular(const osg::Vec4& c,Face f=FRONT) { f ? _spf=c : _spb=c; }
    const osg::Vec4& specular(Face f=FRONT);

  private:
    osg::Vec4 _af, _df, _ef, _spf; // front properties
    osg::Vec4 _ab, _db, _eb, _spb; // back properties
    float _shb,  _shf;
  };

};

#endif
