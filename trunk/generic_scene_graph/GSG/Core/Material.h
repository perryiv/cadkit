
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

#include "GSG/Core/Referenced.h"
#include "GSG/Core/Color.h"


namespace GSG {


class GSG_CORE_EXPORT Material : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Material );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Material, Referenced );
  typedef Color::value_type value_type;

  // Constructors
  explicit Material();
  Material ( const Material &m );
  Material ( const Color &ambient, 
             const Color &diffuse, 
             const Color &specular, 
             const Color &emissive, 
             value_type shininess );

  // Access to the colors.
  Color &                 ambient();
  Color &                 diffuse();
  Color &                 specular();
  Color &                 emissive();
  value_type &            shininess();

  // Get the colors.
  const Color &           ambient() const;
  const Color &           diffuse() const;
  const Color &           specular() const;
  const Color &           emissive() const;
  const value_type &      shininess() const;

  // Set the material.
  void                    set ( const Material &m );
  void                    set ( const Color &ambient, 
                                const Color &diffuse, 
                                const Color &specular, 
                                const Color &emissive, 
                                value_type shininess );

  // Are they equal?
  bool                    equal ( const Material &m ) const;

  // Assignment
  Material &              operator = ( const Material &rhs );

private:

  Color _ambient;
  Color _diffuse;
  Color _specular;
  Color _emissive;
  value_type _shininess;
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
