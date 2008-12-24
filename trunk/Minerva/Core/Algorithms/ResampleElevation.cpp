
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Algorithms/ResampleElevation.h"
#include "Minerva/Core/Algorithms/SubRegion.h"
#include "Minerva/Core/Functions/ParameterizeExtents.h"

#include "GN/Algorithms/Fill.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Config/UsulConfig.h"
#include "GN/Evaluate/Point.h"
#include "GN/Splines/Surface.h"
#include "GN/Write/XML.h"

#if 0
#include "boost/gil/gil_all.hpp"
#include "boost/gil/typedefs.hpp"
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Return elevation data for the request extents.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Minerva::Core::Algorithms::resampleElevation ( const osg::Image& image, const Extents& extents, const Extents& request, const Usul::Math::Vec4d& region )
{
  // Get the parameterized region to use.
  // This function does not work in all cases.  Pass in the region to use for now.
  //Usul::Math::Vec4d region ( Minerva::Core::Functions::paramertizeExtents ( extents, request ) );

  // Get the size of the image.
  Usul::Math::Vec2ui size ( image.s(), image.t() );

#if 0 //ndef __linux
#if 1
  //
  // Use boost gil to resample the image.  I'm still learning boost gil, so there may be cleaner ways to code this.
  //
  
  // Make a boost image view from the elevation data.
  const float *pointer ( reinterpret_cast<const float*> ( image.data() ) );
  boost::gil::gray32f_image_t srcImage ( size[0], size[1] );
  boost::gil::gray32f_view_t srcView ( boost::gil::view ( srcImage ) );
  
  // Copy the pixels into the boost image.
  const unsigned int numPixels ( size[0] * size[1] );
  for ( unsigned int i = 0; i < numPixels; ++i )
  {
    const float value ( pointer[i] );
    srcView[i] = value;
  }
  
  // Make an image twice the size.
  boost::gil::gray32f_image_t larger ( size[0] * 2, size[1] * 2 );
  boost::gil::gray32f_view_t largerView ( boost::gil::view ( larger ) );
  
  // Resize using bilinear interpolation.
  boost::gil::resize_view ( srcView, largerView, boost::gil::bilinear_sampler() );
  
  // Make a new osg::Image from the boost image.
  osg::ref_ptr<osg::Image> final ( new osg::Image );
  final->allocateImage ( size[0], size[1], 1, GL_LUMINANCE, GL_FLOAT );
  
  const int startS ( larger.width() * region[0] );
  const int startV ( larger.height() * region[2] );
  
  // Copy the result into the osg::Image.
  for ( int s = 0; s < static_cast<int> ( size[0] ); ++s )
  {
    for ( int t = 0; t < static_cast<int> ( size[1] ); ++t )
    {
      const float value ( largerView ( s + startS, t + startV )[0] );
      *reinterpret_cast<float*> ( final->data ( s, t ) ) = value;
    }
  }

  // Set the elevation to our answer.
  return final.release();
#else

  // Typedefs.
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorChecker;
  typedef GN::Config::UsulConfig < double, double, unsigned int, ErrorChecker > Config;
  typedef GN::Splines::Surface < Config > Surface;
  typedef Surface::IndependentSequence IndependentSequence;
  typedef Surface::DependentContainer DependentContainer;
  typedef Surface::DependentSequence DependentSequence;
  typedef Surface::IndependentType Parameter;
  typedef Surface::SizeType SizeType;
  typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorChecker > KnotVectorBuilder;
  
  // We want one dependent variable.
  const SizeType dimension ( 1 );
  
  // We want bi-cubic.
  const SizeType order ( 4 );
  
  // Make a surface.
  Surface surface;
  
  // Resize.
  surface.resize ( dimension, order, order, size[0], size[1], false );
  
  // Get a pointer to the data.
  const float *pointer ( reinterpret_cast<const float*> ( image.data() ) );
  
  // We should have this many control points.
  const unsigned int numPixels ( size[0] * size[1] );
  
  // Set the control points.
  for ( unsigned int i = 0; i < numPixels; ++i )
  {
    surface.controlPoint ( 0, i ) = pointer[i];
  }
  
  // Make the independent sequence for u and v.
  IndependentSequence u ( size[0], 0 ), v ( size[1], 0 );
  
  // Fill u and v with even distribution of values from 0 to 1.
  GN::Algorithms::fill ( u, 0, 1.0 );
  GN::Algorithms::fill ( v, 0, 1.0 );
  
  // Get knot vectors.
  IndependentSequence& knots0 ( surface.knotVector ( 0 ) );
  IndependentSequence& knots1 ( surface.knotVector ( 1 ) );
  
  // Make the knots.
  KnotVectorBuilder::build ( u, order, knots0 );
  KnotVectorBuilder::build ( v, order, knots1 );
  
  surface.check();
  std::ostringstream os;
  GN::Write::xml ( surface, 2, os );
  ::printf ( "%s\n", os.str().c_str() );
  
  // Get the range for the u and v values.
  const double deltaU ( region[1] - region[0] );
  const double deltaV ( region[3] - region[2] );
  
  // Step sizes.
  const double stepU ( deltaU / size[0] );
  const double stepV ( deltaV / size[1] );
  
  // Make our new elevation.
  osg::ref_ptr<osg::Image> final ( new osg::Image );
  final->allocateImage ( size[0], size[1], 1, GL_LUMINANCE, GL_FLOAT );
  
  // Loop and fill our image data.
  //float *data ( reinterpret_cast<float*> ( final->data() ) );
  for ( unsigned int i = 0; i < size[0]; ++i )
  {
    for ( unsigned int j = 0; j < size[1]; ++j )
    {
      const double u ( region[0] + ( stepU * i ) );
      const double v ( region[2] + ( stepV * j ) );
      
      Surface::Vector point ( surface.dimension() );
      GN::Evaluate::point ( surface, u, v, point );
      
      // Set the value.
      *reinterpret_cast<float*> ( final->data( i, j ) ) = point[0];
      //*data++ = static_cast<float> ( point[0] );
    }
  }

  return final.release();
      
#endif
#else
  return Minerva::Core::Algorithms::subRegion<float> ( image, region, GL_LUMINANCE, GL_FLOAT );
#endif
}
