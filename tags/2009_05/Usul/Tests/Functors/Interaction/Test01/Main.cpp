
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for configuring interaction functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
#endif

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Functors/Interaction/Common/Sequence.h"
#include "Usul/Functors/Interaction/Input/JoystickHorizontal.h"
#include "Usul/Functors/Interaction/Input/JoystickVertical.h"
#include "Usul/Functors/Interaction/Input/WandPitch.h"
#include "Usul/Functors/Interaction/Input/WandYaw.h"
#include "Usul/Functors/Interaction/Matrix/IdentityMatrix.h"
#include "Usul/Functors/Interaction/Matrix/InverseMatrix.h"
#include "Usul/Functors/Interaction/Matrix/MatrixPair.h"
#include "Usul/Functors/Interaction/Navigate/Direction.h"
#include "Usul/Functors/Interaction/Navigate/Rotate.h"
#include "Usul/Functors/Interaction/Navigate/Translate.h"
#include "Usul/Functors/Interaction/Wand/WandMatrix.h"
#include "Usul/Functors/Interaction/Wand/WandPosition.h"
#include "Usul/Functors/Interaction/Wand/WandRotation.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Program class.
//
///////////////////////////////////////////////////////////////////////////////

class Program
{
public:

  // Misc typedefs.
  typedef XmlTree::Node::Children Children;

  // Inputs.
  typedef Usul::Functors::Interaction::Input::AnalogInput AnalogInput;
  typedef Usul::Functors::Interaction::Input::JoystickHorizontal JoystickHorizontal;
  typedef Usul::Functors::Interaction::Input::JoystickVertical JoystickVertical;
  typedef Usul::Functors::Interaction::Input::WandPitch WandPitch;
  typedef Usul::Functors::Interaction::Input::WandYaw WandYaw;

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

  // Constructor and destructor.
  Program ( int argc, char **argv, std::ostream *trace );
  ~Program();

  // Run the test.
  void          run();

private:

  Usul::CommandLine::Arguments &_args;
  Usul::Factory::ObjectFactory _factory;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::Program ( int argc, char **argv, std::ostream *trace ) :
  _args ( Usul::CommandLine::Arguments::instance() ),
  _factory()
{
  USUL_TRACE_SCOPE;

  // Set trace file.
  Usul::Trace::Print::init ( trace );

  // Grab the command-line arguments.
  _args.set ( argc, argv );

  // Populate the factory.
  _factory.add ( new Usul::Factory::TypeCreator<JoystickHorizontal> ( "horizontal joystick" ) );
  _factory.add ( new Usul::Factory::TypeCreator<JoystickVertical>   ( "vertical joystick"   ) );
  _factory.add ( new Usul::Factory::TypeCreator<WandPitch>          ( "wand pitch"          ) );
  _factory.add ( new Usul::Factory::TypeCreator<WandYaw>            ( "wand yaw"            ) );

  _factory.add ( new Usul::Factory::TypeCreator<IdentityMatrix>     ( "identity matrix"     ) );
  _factory.add ( new Usul::Factory::TypeCreator<InverseMatrix>      ( "inverse matrix"      ) );
  _factory.add ( new Usul::Factory::TypeCreator<MatrixPair>         ( "matrix pair"         ) );
  _factory.add ( new Usul::Factory::TypeCreator<WandMatrix>         ( "wand matrix"         ) );
  _factory.add ( new Usul::Factory::TypeCreator<WandPosition>       ( "wand position"       ) );
  _factory.add ( new Usul::Factory::TypeCreator<WandRotation>       ( "wand rotation"       ) );

  _factory.add ( new Usul::Factory::TypeCreator<DirectionFunctor>   ( "direction"           ) );

  _factory.add ( new Usul::Factory::TypeCreator<TranslateFunctor>   ( "translate"           ) );
  _factory.add ( new Usul::Factory::TypeCreator<RotateFunctor>      ( "rotate"              ) );

  _factory.add ( new Usul::Factory::TypeCreator<FavoriteFunctor>    ( "sequence"            ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::~Program()
{
  USUL_TRACE_SCOPE;

  _factory.clear();

  // Set the mutex factory to null so that we can find late uses of it.
  Usul::Threads::Mutex::createFunction ( 0x0 );
}


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
  template <> struct FromString < Usul::Math::Vec3f >
  {
    static Usul::Math::Vec3f convert ( const std::string &s, const Usul::Math::Vec3f &defaultValue )
    {
      Usul::Math::Vec3f v ( defaultValue );
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
  void add ( const Setter &setter, Usul::Factory::ObjectFactory &factory, XmlTree::Node::Children &kids, FunctorMap &functors )
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
    typedef Program::AnalogInput AnalogInput;
    void operator () ( XmlTree::Node &node, AnalogInput &functor ) const
    {
      functor.range ( Helper::FromString<AnalogInput::Vec2>::convert ( node.attributes()["range"], AnalogInput::Vec2 ( -1.0f, 1.0f ) ) );
    }
  };

  struct MatrixSetter
  {
    typedef Program::MatrixFunctor MatrixFunctor;
    void operator () ( XmlTree::Node &node, MatrixFunctor &functor ) const
    {
      // TODO: handle composite matrix types.
    }
  };

  struct DirectionSetter
  {
    typedef Program::DirectionFunctor DirectionFunctor;
    typedef Program::MatrixFunctors MatrixFunctors;
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
    typedef Program::TransformFunctor TransformFunctor;
    typedef Program::DirectionFunctors DirectionFunctors;
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
    typedef Program::FavoriteFunctor FavoriteFunctor;
    typedef Program::AnalogInputs AnalogInputs;
    typedef Program::AnalogInput AnalogInput;
    typedef Program::TransformFunctors TransformFunctors;
    typedef Program::TransformFunctor TransformFunctor;
    typedef XmlTree::Node::Children Children;
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
          TransformFunctor::RefPtr transform ( _transforms[transformName] );
          if ( true == transform.valid() )
          {
            const std::string inputName ( node->attributes()["input"] );
            AnalogInput::RefPtr analog ( _inputs[inputName] );
            if ( true == analog.valid() )
            {
              transform->analog ( analog.get() );
              functor.append ( transform.get() );
            }
          }
        }
      }
    }
  private:
    AnalogInputs &_inputs;
    TransformFunctors &_transforms;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run()
{
  // Check number of arguments.
  if ( _args.argc() < 2 )
  {
    std::cout << "Usage:\n\t" << Usul::File::base ( _args.program() ) << " <filename>" << std::endl;
    return;
  }

  // Initialize and finalize use of xerces.
  XmlTree::XercesLife life;

  // Open the input file.
  const std::string file ( _args.argv(1) );
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( file );

  // Initialize the caller.
  Usul::Interfaces::IUnknown::RefPtr caller ( 0x0 );

  // Find all important functors.
  Children analogs    ( document->find ( "analog",    true ) );
  Children digitals   ( document->find ( "digital",   true ) );
  Children matrices   ( document->find ( "matrix",    true ) );
  Children directions ( document->find ( "direction", true ) );
  Children transforms ( document->find ( "transform", true ) );
  Children favorites  ( document->find ( "favorite",  true ) );

  // The maps of the various functors.
  AnalogInputs analogInputs;
  MatrixFunctors matrixFunctors;
  DirectionFunctors directionFunctors;
  TransformFunctors transformFunctors;
  FavoriteFunctors favoriteFunctors;

  // Setters.
  Helper::AnalogSetter analogSetter;
  Helper::MatrixSetter matrixSetter;
  Helper::DirectionSetter directionSetter ( matrixFunctors );
  Helper::TransformSetter transformSetter ( directionFunctors );
  Helper::FavoriteSetter favoriteSetter   ( analogInputs, transformFunctors );

  // Make the functors.
  Helper::add ( analogSetter,    _factory, analogs,    analogInputs  );
  Helper::add ( matrixSetter,    _factory, matrices,   matrixFunctors );
  Helper::add ( directionSetter, _factory, directions, directionFunctors );
  Helper::add ( transformSetter, _factory, transforms, transformFunctors );
  Helper::add ( favoriteSetter,  _factory, favorites,  favoriteFunctors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test ( int argc, char **argv, std::ostream *trace )
{
  USUL_TRACE_SCOPE_STATIC;
  Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );
  Program program ( argc, argv, trace );
  program.run();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
#ifdef _DEBUG

  // Trace file.
  std::ofstream trace ( "trace.csv" );

  // Call test function.
  Usul::Functions::safeCallV1V2V3 ( _test, argc, argv, &trace, "1586869674" );

#else

  // Call test function.
  Usul::Functions::safeCallV1V2V3 ( _test, argc, argv, static_cast<std::ostream *> ( 0x0 ), "3826752243" );

#endif

  return 0;
}
