
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_FUNCTOR_HELPERS_H__
#define __VRV_CORE_FUNCTOR_HELPERS_H__

#include "Usul/Functors/Interaction/Input/JoystickHorizontal.h"
#include "Usul/Functors/Interaction/Input/JoystickVertical.h"
#include "Usul/Functors/Interaction/Input/WandPitch.h"
#include "Usul/Functors/Interaction/Input/WandYaw.h"
#include "Usul/Functors/Interaction/Input/WandRoll.h"
#include "Usul/Functors/Interaction/Matrix/IdentityMatrix.h"
#include "Usul/Functors/Interaction/Matrix/InverseMatrix.h"
#include "Usul/Functors/Interaction/Matrix/MatrixPair.h"
#include "Usul/Functors/Interaction/Navigate/Direction.h"
#include "Usul/Functors/Interaction/Navigate/Rotate.h"
#include "Usul/Functors/Interaction/Navigate/Translate.h"
#include "Usul/Functors/Interaction/Wand/WandMatrix.h"
#include "Usul/Functors/Interaction/Wand/WandPosition.h"
#include "Usul/Functors/Interaction/Wand/WandRotation.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/Factory/ObjectFactory.h"

#include "XmlTree/Node.h"

namespace VRV {
namespace Core {

  // Misc typedefs.
  typedef XmlTree::Node::Children    Children;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Inputs.
  typedef Usul::Functors::Interaction::Input::AnalogInput AnalogInput;
  typedef Usul::Functors::Interaction::Input::JoystickHorizontal JoystickHorizontal;
  typedef Usul::Functors::Interaction::Input::JoystickVertical JoystickVertical;
  typedef Usul::Functors::Interaction::Input::WandPitch WandPitch;
  typedef Usul::Functors::Interaction::Input::WandYaw WandYaw;
  typedef Usul::Functors::Interaction::Input::WandRoll WandRoll;

  // Matrices.
  typedef Usul::Functors::Interaction::Matrix::IdentityMatrix IdentityMatrix;
  typedef Usul::Functors::Interaction::Matrix::InverseMatrix InverseMatrix;
  typedef Usul::Functors::Interaction::Matrix::MatrixFunctor MatrixFunctor;
  typedef Usul::Functors::Interaction::Matrix::MatrixPair MatrixPair;
  typedef Usul::Functors::Interaction::Wand::WandPosition WandPosition;
  typedef Usul::Functors::Interaction::Wand::WandRotation WandRotation;
  typedef Usul::Functors::Interaction::Wand::WandMatrix WandMatrix;

  // Directions.
  typedef Usul::Functors::Interaction::Navigate::Direction DirectionFunctor;

  // Transforms.
  typedef Usul::Functors::Interaction::Navigate::Transform TransformFunctor;
  typedef Usul::Functors::Interaction::Navigate::Translate TranslateFunctor;
  typedef Usul::Functors::Interaction::Navigate::Rotate RotateFunctor;
  typedef Usul::Functors::Interaction::Common::Sequence FavoriteFunctor;

  // Maps.
  typedef std::map < std::string, AnalogInput::RefPtr > AnalogInputs;
  typedef std::map < std::string, MatrixFunctor::RefPtr > MatrixFunctors;
  typedef std::map < std::string, DirectionFunctor::RefPtr > DirectionFunctors;
  typedef std::map < std::string, TransformFunctor::RefPtr > TransformFunctors;
  typedef std::map < std::string, FavoriteFunctor::RefPtr > FavoriteFunctors;

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to convert from strings.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > struct FromString
  {
    static T convert ( const std::string &s, T defaultValue )
    {
      T t ( defaultValue );
      std::istringstream in ( s );
      in >> t;
      return t;
    }
  };
  template <> struct FromString < Usul::Math::Vec2f >
  {
    static Usul::Math::Vec2f convert ( const std::string &s, const Usul::Math::Vec2f &defaultValue )
    {
      Usul::Math::Vec2f v ( defaultValue );
      std::istringstream in ( s );
      in >> v[0] >> v[1];
      return v;
    }
  };
  template <> struct FromString < Usul::Math::Vec3d >
  {
    static Usul::Math::Vec3d convert ( const std::string &s, const Usul::Math::Vec3d &defaultValue )
    {
      Usul::Math::Vec3d v ( defaultValue );
      std::istringstream in ( s );
      in >> v[0] >> v[1] >> v[2];
      return v;
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add a new functor to the given map.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Setter, class FunctorMap > 
  void add ( const Setter &setter, Usul::Factory::ObjectFactory &factory, XmlTree::Node::Children &kids, FunctorMap &functors, Unknown *caller = 0x0 )
  {
    typedef typename FunctorMap::mapped_type SmartPointerType;
    typedef typename SmartPointerType::element_type ObjectType;
    typedef XmlTree::Node::Children Children;

    for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
    {
      XmlTree::Node::RefPtr node ( i->get() );
      if ( true == node.valid() )
      {
        const std::string type ( node->attributes()["type"] );
        SmartPointerType pointer ( dynamic_cast < ObjectType * > ( factory.create ( type ) ) );
        if ( true == pointer.valid() )
        {
          const std::string name ( node->attributes()["name"] );
          if ( false == name.empty() )
          {
            pointer->name ( name );
            setter ( *node, *pointer );
            pointer->caller ( caller );
            functors[name] = pointer;
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to set the functor's members from the node's attributes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct AnalogSetter
  {
    void operator () ( XmlTree::Node &node, AnalogInput &functor ) const
    {
      functor.range ( Helper::FromString<AnalogInput::Vec2>::convert ( node.attributes()["range"], AnalogInput::Vec2 ( -1.0f, 1.0f ) ) );
    }
  };

  struct MatrixSetter
  {
    void operator () ( XmlTree::Node &node, MatrixFunctor &functor ) const
    {
      // TODO: handle composite matrix types.
    }
  };

  struct DirectionSetter
  {
    DirectionSetter ( MatrixFunctors &matrices ) : _matrices ( matrices ){}
    void operator () ( XmlTree::Node &node, DirectionFunctor &functor ) const
    {
      functor.original ( Helper::FromString<DirectionFunctor::Vector>::convert ( node.attributes()["vector"], DirectionFunctor::Vector ( 0.0f, 0.0f, -1.0f ) ) );
      functor.matrix ( _matrices[node.attributes()["matrix"]] );
    }
  private:
    MatrixFunctors &_matrices;
  };

  struct TransformSetter
  {
    TransformSetter ( DirectionFunctors &directions ) : _directions ( directions ){}
    void operator () ( XmlTree::Node &node, TransformFunctor &functor ) const
    {
      functor.speed ( Helper::FromString<float>::convert ( node.attributes()["speed"], 0.1f ) );
      functor.direction ( _directions[node.attributes()["direction"]] );
    }
  private:
    DirectionFunctors &_directions;
  };

  struct FavoriteSetter
  {
    FavoriteSetter ( AnalogInputs &inputs, TransformFunctors &transforms ) : _inputs ( inputs ), _transforms ( transforms ){}
    void operator () ( XmlTree::Node &node, FavoriteFunctor &functor ) const
    {
      // Find all important functors.
      Children components ( node.find ( "component", true ) );

      // Loop through the children.
      for ( Children::iterator i = components.begin(); i != components.end(); ++i )
      {
        XmlTree::Node::RefPtr node ( i->get() );
        if ( true == node.valid() )
        {
          const std::string name ( node->name() );
          const std::string transformName ( node->attributes()["transform"] );
          TransformFunctor::RefPtr transform ( static_cast < TransformFunctor* > ( _transforms[transformName]->clone() ) );
          if ( true == transform.valid() )
          {
            const std::string inputName ( node->attributes()["input"] );
            AnalogInput::RefPtr analog ( static_cast < AnalogInput* > ( _inputs[inputName]->clone() ) );
            if ( true == analog.valid() )
            {
              transform->analog ( analog.get() );
              functor.append ( transform.get() );
            }

            // If the "component" in the functor xml file has an attribute 
            // called "speed", then this will override the speed that was 
            // set when the transform was made.
            transform->speed ( Helper::FromString<float>::convert ( node->attributes()["speed"], transform->speed() ) );
          }
        }
      }
    }
  private:
    AnalogInputs &_inputs;
    TransformFunctors &_transforms;
  };
}

}
}

#endif // __VRV_CORE_FUNCTOR_HELPERS_H__
