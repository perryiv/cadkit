
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A material class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_MATERIAL_CLASS_H_
#define _GENERIC_SCENE_GRAPH_CORE_MATERIAL_CLASS_H_

#include "GSG/Core/Attribute.h"
#include "GSG/Core/Color.h"


namespace GSG {


class GSG_CORE_EXPORT Material : public Attribute
{
public:

  GSG_DECLARE_REFERENCED ( Material );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Material, Attribute );
  typedef Color::value_type value_type;

  // Which side.
  enum Side { FRONT, BACK, FRONT_AND_BACK };

  // Constructors
  explicit Material();
  Material ( const Material &m );
  Material ( const Color &ambient, 
             const Color &diffuse, 
             const Color &specular, 
             const Color &emissive, 
             value_type shininess,
             Side side );

  // Access to the members.
  const Color &           ambient()   const { return _ambient; }
  const Color &           diffuse()   const { return _diffuse; }
  const Color &           specular()  const { return _specular; }
  const Color &           emissive()  const { return _emissive; }
  const value_type &      shininess() const { return _shininess; }
  Side                    side()      const { return _side; }

  // Access to the members.
  void                    ambient   ( const Color & );
  void                    diffuse   ( const Color & );
  void                    specular  ( const Color & );
  void                    emissive  ( const Color & );
  void                    shininess ( value_type s );
  void                    side      ( Side s );

  // Set the material.
  void                    set ( const Color &ambient, 
                                const Color &diffuse, 
                                const Color &specular, 
                                const Color &emissive, 
                                value_type shininess,
                                Side side );

  // Are they equal?
  bool                    equal ( const Material &m ) const;

  // Assignment
  Material &              operator = ( const Material &rhs );

protected:

  virtual ~Material();

private:

  Color _ambient;
  Color _diffuse;
  Color _specular;
  Color _emissive;
  value_type _shininess;
  Side _side;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

inline bool operator == ( const Material &left, const Material &right )
{
  return left.equal ( right );
}
inline bool operator != ( const Material &left, const Material &right )
{
  return false == left.equal ( right );
}


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_MATERIAL_CLASS_H_
