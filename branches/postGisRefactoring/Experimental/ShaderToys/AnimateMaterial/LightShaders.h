
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __LIGHT_SHADERS_H__
#define __LIGHT_SHADERS_H__

static const char* directionalLightVertSource = 
{ 
  "void DirectionalLight(in int i, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)\n"
  "{\n"
  "  float nDotVP;    // normal . light direction \n"
  "  float nDotHV;    // normal . light half vector \n"
  "  float pf;        // power factor \n"
  " \n"
  "  nDotVP = max ( 0.0, dot( normal, normalize ( vec3 ( gl_LightSource[i].position ) ) ) ); \n"
  "  nDotHV = max ( 0.0, dot( normal, vec3 ( gl_LightSource[i].halfVector ) ) ); \n"
  " \n"
  "  if ( nDotVP == 0.0 ) \n"
  "     pf = 0.0; \n"
  "  else \n"
  "     pf = pow ( nDotHV, gl_FrontMaterial.shininess ); \n"
  "  ambient   += gl_LightSource[i].ambient; \n"
  "  diffuse   += gl_LightSource[i].diffuse * nDotVP; \n"
  "  specular  += gl_LightSource[i].specular * pf; \n"
  "}\n"
};


static const char* pointLightVertSource = 
{
  "void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)\n"
  "{\n"
  "  float nDotVP;       // normal . light direction \n"
  "  float nDotHV;       // normal . light half vector \n"
  "  float pf;           // power factor \n"
  "  float attenuation;  // computed attenuation factor \n"
  "  float d;            // distance from surce to light source \n"
  "  vec3  VP;           // direction from surface to light position \n"
  "  vec3  halfVector;   // diretion of maximum hightlights \n"
  "  \n"
  "  // Compute vector from surface to light position. \n"
  "  VP = vec3 ( gl_LightSource[i].position ) - ecPosition3; \n"
  "  \n"
  "  // Compute distance between surface and light position \n"
  "  d = length ( VP ); \n"
  "  \n"
  "  // Normalize the vector from surface to light position \n"
  "  VP = normalize ( VP ); \n"
  "  \n"
  "  // Compute attenuation. \n"
  "  attenuation = 1.0 / ( gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * d + gl_LightSource[i].quadraticAttenuation * d * d ); \n "
  "  \n"
  "  halfVector = normalize ( VP + eye ); \n"
  "  \n"
  "  nDotVP = max ( 0.0, dot ( normal, VP ) ); \n"
  "  nDotHV = max ( 0.0, dot ( normal, halfVector ) ); \n"
  "  \n"
  "  if ( nDotVP == 0.0 ) \n"
  "    pf = 0.01; \n"
  "  else \n"
  "    pf = pow ( nDotHV, gl_FrontMaterial.shininess ); \n"
  "  \n"
  "  ambient  += gl_LightSource[i].ambient * attenuation; \n"
  "  diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation; \n"
  "  specular += gl_LightSource[i].specular * pf * attenuation; \n"
  "}\n"
};

static const char* spotLightVertSource = 
{
  "void SpotLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)\n"
  "{\n"
  "  float nDotVP;           // normal . light direction \n"
  "  float nDotHV;           // normal . light half vector \n"
  "  float pf;               // power factor \n"
  "  float spotDot;          // cosine of angle between spotlight. \n"
  "  float spotAttenuation;  // spotlight attenuation factor. \n"
  "  float attenuation;      // computed attenuation factor \n"
  "  float d;                // distance from surce to light source \n"
  "  vec3  VP;               // direction from surface to light position \n"
  "  vec3  halfVector;       // diretion of maximum hightlights \n"
  "  \n"
  "  // Compute vector from surface to light position. \n"
  "  VP = vec3 ( gl_LightSource[i].position ) - ecPosition3; \n"
  "  \n"
  "  // Compute distance between surface and light position \n"
  "  d = length ( VP ); \n"
  "  \n"
  "  // Normalize the vector from surface to light position \n"
  "  VP = normalize ( VP ); \n"
  "  \n"
  "  // Compute attenuation. \n"
  "  attenuation = 1.0 / ( gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * d + gl_LightSource[i].quadraticAttenuation * d * d ); \n "
  "  \n"
  "  // See if point on surface is inside cone of illumination. \n"
  "  spotDot = dot ( -VP, gl_LightSource[i].spotDirection );"
  "  \n"
  "  if( spotDot < gl_LightSource[i].spotCosCutoff ) \n"
  "    spotAttenuation = 0.0;  // light adds no contribution. \n"
  "  else \n"
  "    spotAttenuation = pow ( spotDot, gl_LightSource[i].spotExponent ); \n"
  "  \n"
  "  attenuation *= spotAttenuation; \n"
  "  \n"
  "  halfVector = normalize ( VP + eye ); \n"
  "  \n"
  "  nDotVP = max ( 0.0, dot ( normal, VP ) ); \n"
  "  nDotHV = max ( 0.0, dot ( normal, halfVector ) ); \n"
  "  \n"
  "  if ( nDotVP == 0.0 ) \n"
  "    pf = 0.01; \n"
  "  else \n"
  "    pf = pow ( nDotHV, gl_FrontMaterial.shininess ); \n"
  "  \n"
  "  ambient  += gl_LightSource[i].ambient * attenuation; \n"
  "  diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation; \n"
  "  specular += gl_LightSource[i].specular * pf * attenuation; \n"
  "}\n"
};


#endif // __LIGHT_SHADERS_H__
