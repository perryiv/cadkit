
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for selecting the intersected portion of the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SELECT_FUNCTOR_H_
#define _CV_SELECT_FUNCTOR_H_

#include "CadViewer/Pick/Intersect.h"
#include "osg/ref_ptr"

#include "osg/ref_ptr"

namespace osg { class Material; };


namespace CV {
namespace Pick {


class Select : public Intersect
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Select );
  typedef Intersect BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef osg::Material Material;
  typedef osg::ref_ptr<Material> MaterialPtr;

  Select ( 
    Unknown *unknown, 
    Direction *dir,
    const std::string& name);
  Select ( const Select &cb );

  // Select the intersected portion of the scene.
  virtual void          operator()();

  // Select/unselect the current node, if any.
  void                  select();
  void                  unselect();

  // Set/get the selection-material.
  const Material *      material() const;
  void                  material ( Material *m );

  // Reset any states.
  virtual void          reset();

  // Get the intersected node. It could be null.
  const osg::Node *     node() const { return _node; }
  osg::Node *           node()       { return _node; }

protected:

  virtual ~Select();

  Material *            _cloneMaterial() const;

  void                  _pushMaterial ( osg::Node* node );
  void                  _pushMaterial ( const osg::Material* mat, osg::Node* node );
  void                  _popMaterial  ( osg::Node* node );

  void                  _select   ( osg::Node* node );
  void                  _unselect ( osg::Node* node );

private:

  USUL_REF_POINTER ( osg::Node ) _node;
  MaterialPtr _material;
};


}; // namespace Pick
}; // namespace CV


#endif // _CV_SELECT_FUNCTOR_H_
