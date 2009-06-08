
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Atmospheric scattering.  See: 
//  Ray tracing of light through atmosphere: http://www.gamedev.net/reference/articles/article2093.asp
//  Explaination of formulas: http://nis-lab.is.s.u-tokyo.ac.jp/~nis/cdrom/skymul.pdf
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Utilities/Atmosphere.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Algorithms/Sphere.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/Interpolate.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Trace/Trace.h"

#include "osgUtil/CullVisitor"

using namespace Minerva::Core::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Atmosphere::Atmosphere() : BaseClass(),
  _dirty ( true ),
  _samples ( 4 ),
  _Kr ( 0.0025 ),
  _Kr4PI ( 0.0 ),
  _Km ( 0.0015 ),
  _Km4PI ( 0.0 ),
  _intensity ( 15.0 ),
  _g ( -0.85 ),
  _wavelength ( 0.650, 0.570, 0.475 ), // Red, Green, and Blue wavelengths.
  _wavelength4 (),
  _rayleighScaleDepth ( 0.25 ),
  _mieScaleDepth ( 0.1 ),
  _opticalDepthBuffer ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // Set the constant values.
  _Kr4PI = _Kr * 4.0 * Usul::Math::PIE;
  _Km4PI = _Km * 4.0 * Usul::Math::PIE;
  _wavelength4[0] = Usul::Math::pow ( _wavelength[0], 4.0 );
  _wavelength4[1] = Usul::Math::pow ( _wavelength[1], 4.0 );
  _wavelength4[2] = Usul::Math::pow ( _wavelength[2], 4.0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Atmosphere::~Atmosphere()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Atmosphere::_destroy ), "2794653228" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::dirty ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Atmosphere::dirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::traverse ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;
  
  BaseClass::traverse ( nv );

  this->_buildOpticalDepthBuffer();
  
  switch ( nv.getVisitorType() ) 
  {
  case osg::NodeVisitor::CULL_VISITOR:
    if ( osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor*> ( &nv ) )
      this->_updateColors( *cv );
    break;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update colors.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::_updateColors ( osgUtil::CullVisitor& cv )
{
  USUL_TRACE_SCOPE;
  
  osg::ref_ptr<osg::Vec3Array> vertices ( this->vertices() );
  osg::ref_ptr<osg::Vec4Array> colors ( this->colors() );
  
  // Return now if we don't have valid data.
  if ( false == vertices.valid() || false == colors.valid() )
    return;
  
  // Get the eye.
  const osg::Vec3f &eye ( cv.getViewPointLocal() );
  
  osg::ref_ptr<osg::RefMatrix> modelView ( cv.getModelViewMatrix() );
  osg::Matrix m ( modelView.valid() ? *modelView : osg::Matrix::identity() );
  
  const osg::Vec3 lightDirection ( osg::Matrix::transform3x3 ( osg::Vec3 ( 0.0, 0.0, -1.0 ), m ) );
  //osg::Vec3 lightDirection ( -eye); lightDirection.normalize();
  
  // Loop over every vertex.
  for ( osg::Vec3Array::const_iterator iter = vertices->begin(); iter != vertices->end(); ++iter )
  {
    const unsigned int index ( iter - vertices->begin() );
    this->_updateColor ( eye, lightDirection, *iter, colors->at ( index ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update single color.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::_updateColor ( const osg::Vec3& e, const osg::Vec3& lightDirection, const osg::Vec3& vertex, osg::Vec4& color )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  const double innerRadius ( this->innerRadius() );
  const double outerRadius ( this->outerRadius() );
  
  osg::Vec3 eye ( e );
  osg::Vec3 pos ( vertex );
  
  // Get the ray from the eye to the vertex.
  osg::Vec3 ray ( vertex - eye );
  
  // Normalize.  Returns length before normalization.
  double dFar ( ray.normalize() );
  
  // Calculate the intersection point of the ray and outer atmosphere.
  // This is a quadratic equation: http://en.wikipedia.org/wiki/Quadratic_equation
  const double B ( 2.0f * ( eye * ray ) );
  const double C ( ( eye * eye ) - ( outerRadius * outerRadius ) );
  const double discriminant ( Usul::Math::maximum ( 0.0, B * B - 4.0 * C ) );
  double dNear ( 0.5 * ( -B - Usul::Math::sqrt ( discriminant ) ) );
  
  // If distance is negative, camera is inside of atmosphere.
  const bool cameraInAtmosphere ( dNear <= 0.0 );
  
  // Variables needed below.
  bool cameraAbove ( true );
  osg::Vec4f cameraDepth ( 0.0, 0.0, 0.0, 0.0 ), lightDepth ( 0.0, 0.0, 0.0, 0.0 ), sampleDepth ( 0.0, 0.0, 0.0, 0.0 );
  
  // If distance is negative, camera is inside of atmosphere.
  if ( cameraInAtmosphere )
  {
    dNear = 0.0;
    const double height ( eye.length() );
    const double altitude ( height - innerRadius );
    cameraAbove = height >= vertex.length();
    double angle ( ( ( cameraAbove ? -ray : ray ) * eye ) / height );
    this->_valueOpticalDepthBuffer( cameraDepth, altitude, 0.5 - angle * 0.5 );
  }
  else
  {
    eye += ray * dNear;
    dFar -= dNear;
    dNear = 0;
  }
  
  const double close ( 0.0000001 );
  
  // If the distance between the points on the ray is negligible, don't bother to calculate anything
  if( dFar <= close )
  {
    color.set (0, 0, 0, 1);
    return;
  }
  
  const double scale ( 1.0 / ( outerRadius - innerRadius) );
  
  // Initialize a few variables to use inside the loop
  Usul::Math::Vec3d rayleighSum ( 0, 0, 0 );
  Usul::Math::Vec3d mieSum ( 0, 0, 0 );
  const double sampleLength ( dFar / _samples );
  const double scaledLength ( sampleLength * scale );
  osg::Vec3 sampleRay ( ray * sampleLength );
  
  // Start at the center of the first sample ray, and loop through each of the others
  pos = eye + sampleRay * 0.5f;
  for( unsigned int i=0; i< _samples; i++ )
  {
    const double height ( pos.length() );
    
    // Start by looking up the optical depth coming from the light source to this point
    const double lightAngle ( ( lightDirection * pos ) / height );
    const double altitude ( ( height - innerRadius ) * scale );
    this->_valueOpticalDepthBuffer( lightDepth, altitude, 0.5f - lightAngle * 0.5f);
    
    // If no light light reaches this part of the atmosphere, no light is scattered in at this point
    if( lightDepth[0] < close )
      continue;
    
    // Get the density at this point, along with the optical depth from the light source to this point
    double rayleighDensity ( scaledLength * lightDepth[0] );
    double rayleighDepth ( lightDepth[1] );
    double mieDensity ( scaledLength * lightDepth[2] );
    double mieDepth ( lightDepth[3] );
    
    // If the camera is above the point we're shading, we calculate the optical depth from the sample point to the camera
    // Otherwise, we calculate the optical depth from the camera to the sample point
    if( cameraAbove)
    {
      const double sampleAngle = (-ray * pos) / height;
      this->_valueOpticalDepthBuffer( sampleDepth, altitude, 0.5f - sampleAngle * 0.5f);
      rayleighDepth += sampleDepth[1] - cameraDepth[1];
      mieDepth += sampleDepth[3] - cameraDepth[3];
    }
    else
    {
      const double sampleAngle = (ray * pos) / height;
      this->_valueOpticalDepthBuffer( sampleDepth, altitude, 0.5f - sampleAngle * 0.5f);
      rayleighDepth += cameraDepth[1] - sampleDepth[1];
      mieDepth += cameraDepth[3] - sampleDepth[3];
    }
    
    // Now multiply the optical depth by the attenuation factor for the sample ray
    rayleighDepth *= _Kr4PI;
    mieDepth *= _Km4PI;
    
    // Calculate the attenuation factor for the sample ray
    Usul::Math::Vec3d attenuation;
    attenuation[0] = expf( -rayleighDepth / _wavelength4[0] - mieDepth);
    attenuation[1] = expf( -rayleighDepth / _wavelength4[1] - mieDepth);
    attenuation[2] = expf( -rayleighDepth / _wavelength4[2] - mieDepth);
    
    rayleighSum[0] += rayleighDensity * attenuation[0];
    rayleighSum[1] += rayleighDensity * attenuation[1];
    rayleighSum[2] += rayleighDensity * attenuation[2];
    
    mieSum[0] += mieDensity * attenuation[0];
    mieSum[1] += mieDensity * attenuation[1];
    mieSum[2] += mieDensity * attenuation[2];
    
    // Move the position to the center of the next sample ray
    pos += sampleRay;
  }
  
  // Calculate the angle and phase values (this block of code could be handled by a small 1D lookup table, or a 1D texture lookup in a pixel shader)
  const double angle ( ray * lightDirection );
  Usul::Math::Vec2d phase ( 0.0, 0.0 );
  const double angle2 ( angle * angle );
  const double g2 ( _g * _g );
  phase[0] = 0.75f * ( 1.0f + angle2 );
  phase[1] = 1.5f * ((1 - g2) / (2 + g2)) * (1.0f + angle2) / powf(1 + g2 - 2* _g * angle, 1.5f);
  phase[0] *= _Kr * _intensity;
  phase[1] *= _Km * _intensity;
  
  // Calculate the in-scattering color and clamp it to the max color value
  color[0] = rayleighSum[0] * phase[0] / _wavelength4[0] + mieSum[0] * phase[1];
  color[1] = rayleighSum[1] * phase[0] / _wavelength4[1] + mieSum[1] * phase[1];
  color[2] = rayleighSum[2] * phase[0] / _wavelength4[2] + mieSum[2] * phase[1];
  color[0] = Usul::Math::minimum ( color[0], 1.0f );
  color[1] = Usul::Math::minimum ( color[1], 1.0f );
  color[2] = Usul::Math::minimum ( color[2], 1.0f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build lookup table.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::_buildOpticalDepthBuffer()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  if ( 0x0 != _opticalDepthBuffer.get() )
    return;
  
  const double innerRadius ( this->innerRadius() );
  const double outerRadius ( this->outerRadius() );
  
  const unsigned int size ( 128 );
  const unsigned int samples ( 50 );
  const double scale ( 1.0 / ( outerRadius - innerRadius ) );
  
  _opticalDepthBuffer = new osg::Image;
  _opticalDepthBuffer->allocateImage ( size, size, 1, GL_RGBA, GL_FLOAT );
  
  unsigned int index ( 0 );
  float * buffer ( reinterpret_cast< float *> ( _opticalDepthBuffer->data() ) );
  
  for ( unsigned int i = 0; i < size; ++i )
  {
    const double cos ( 1.0 - ( static_cast<double> ( i + i ) / static_cast<double> ( size ) ) );
    const double angle ( Usul::Math::acos ( cos ) );
    osg::Vec3 ray ( Usul::Math::sin ( angle ), Usul::Math::cos ( angle ), 0.0 );
    
    for ( unsigned int j = 0; j < size; ++j )
    {
      const double height ( innerRadius + ( ( outerRadius - innerRadius ) * i ) / size );
      osg::Vec3 pos ( 0.0, height, 0.0 );
      
      const double B ( 2.0f * ( pos * ray ) );
			const double Bsq ( B * B );
			const double Cpart ( pos * pos );
			double C ( Cpart - innerRadius * innerRadius );
			double discriminant ( Bsq - 4.0f * C );
			bool visible ( discriminant < 0 || (0.5f * (-B - Usul::Math::sqrt( discriminant ) ) <= 0 ) && ( 0.5f * (-B + Usul::Math::sqrt(discriminant)) <= 0)) ;

      double rayleighDensityRatio ( 0.0 );
			double mieDensityRatio ( 0.0 );
			if( visible )
			{
				rayleighDensityRatio = expf(-( height - innerRadius) * scale / _rayleighScaleDepth);
				mieDensityRatio = expf(-( height - innerRadius) * scale / _mieScaleDepth);
			}
			else
			{
				// Smooth the transition from light to shadow (it is a soft shadow after all)
				rayleighDensityRatio = buffer[index - size*4] * 0.75f;
				mieDensityRatio = buffer[index+2 - size*4] * 0.75f;
			}

			// Determine where the ray intersects the outer radius (the top of the atmosphere)
			// This is the end of our ray for determining the optical depth (vPos is the start)
			C = Cpart - outerRadius * outerRadius;
			discriminant = Bsq - 4.0f * C;
			double distance ( 0.5f * (-B + sqrtf( discriminant ) ) );

			// Next determine the length of each sample, scale the sample ray, and make sure position checks are at the center of a sample ray
			const double sampleLength ( distance / samples );
			const double scaledLength ( sampleLength * scale );
      osg::Vec3 sampleRay ( ray * sampleLength );
			pos += sampleRay * 0.5f;

			// Iterate through the samples to sum up the optical depth for the distance the ray travels through the atmosphere
			double rayleighDepth ( 0.0 );
			double mieDepth ( 0.0 );
			for( unsigned int s = 0; s < samples; ++s )
			{
				const double length ( pos.length() );
				const double altitude ( Usul::Math::maximum ( ( length - innerRadius ) * scale, 0.0 ) );
				rayleighDepth += expf( -altitude / _rayleighScaleDepth );
				mieDepth += expf( -altitude / _mieScaleDepth );
				pos += sampleRay;
			}
      
			// Multiply the sums by the length the ray traveled
			rayleighDepth *= scaledLength;
			mieDepth *= scaledLength;

			// Store the results for Rayleigh to the light source, Rayleigh to the camera, Mie to the light source, and Mie to the camera
			buffer[index++] = rayleighDensityRatio;
			buffer[index++] = rayleighDepth;
			buffer[index++] = mieDensityRatio;
			buffer[index++] = mieDepth;
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get value from lookup table.
//
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::_valueOpticalDepthBuffer( osg::Vec4& value, double x, double y )
{
  const unsigned int width ( _opticalDepthBuffer->s() );
  const unsigned int height ( _opticalDepthBuffer->t() );
  
  double fX ( x * ( width - 1 ) );
  double fY ( y * ( height - 1 ) );
  const int nX ( Usul::Math::minimum<int> ( width-2, Usul::Math::maximum(0, (int)fX)) );
  const int nY ( Usul::Math::minimum<int>( height-2, Usul::Math::maximum(0, (int)fY)) );
  const double u ( fX - nX );
  const double v ( fY - nY );
  
  const float *buffer ( reinterpret_cast<const float*> ( _opticalDepthBuffer->data ( nX, nY ) ) );
  
  const unsigned int channels ( 4 );

  #if 1
  // Four neighboring pixels.
  const float *a ( buffer );
  const float *b ( buffer + channels );
  const float *c ( buffer + ( channels * ( width + 1 ) ) );
  const float *d ( buffer + ( channels * width ) );
  
  // Bilinear interpolate.
  value[0] = Usul::Math::Interpolate<float>::bilinear ( u, v, a[0], b[0], c[0], d[0] );
  value[1] = Usul::Math::Interpolate<float>::bilinear ( u, v, a[1], b[1], c[1], d[1] );
  value[2] = Usul::Math::Interpolate<float>::bilinear ( u, v, a[2], b[2], c[2], d[2] );
  value[3] = Usul::Math::Interpolate<float>::bilinear ( u, v, a[3], b[3], c[3], d[3] );
  
#else
  for( unsigned int i = 0; i < channels; ++i )
  {
    value[i] =	buffer[0] * ( 1-u ) * ( 1-v ) +
    buffer[channels] * ( u ) * ( 1-v ) +
    buffer[channels * width] * ( 1 - u ) * ( v ) +
    buffer[channels * ( width+1)] * ( u ) * ( v );
    buffer++;
  }
#endif
}
