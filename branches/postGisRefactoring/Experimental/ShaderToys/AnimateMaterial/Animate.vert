
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

uniform mat4 osg_ViewMatrix;

uniform float param;


varying vec4 color;

void main(void)
{
	vec4 ecPosition;
	vec3 ecPosition3;
	vec3 normal;
	vec4 myDiffuse = vec4 ( 0.0, 1.0, 0.0, 1.0);
	vec4 myAmbient = vec4 ( 0.0, 0.0, 0.0, 1.0);
	float mySpec = 40.0;
	
	vec3 minColor = vec3 ( 1.0, 1.0, 0.0 );
	vec3 maxColor = vec3 ( 1.0, 0.0, 0.0 );
	
	myDiffuse.x = minColor.x + ( param * ( maxColor.x - minColor.x ) );
	myDiffuse.y = minColor.y + ( param * ( maxColor.y - minColor.y ) );
	myDiffuse.z = minColor.z + ( param * ( maxColor.z - minColor.z ) );

	vec3 eye   = -osg_ViewMatrix[3].xyz / osg_ViewMatrix[3].w;
	
	ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ecPosition3 = ( vec3 ( ecPosition ) ) / ecPosition.w;
	eye = ecPosition.xyz;
		
	normal = gl_NormalMatrix * gl_Normal;
	
	vec4 ambient  = vec4 ( 0.0 );
	vec4 diffuse  = vec4 ( 0.0 );
	vec4 specular = vec4 ( 0.0 );
	
	//if ( gl_LightSource[0].position.w == 0.0 )
		DirectionalLight ( 0, normal, ambient, diffuse, specular );
	//else if ( gl_LightSource[0].spotCutoff == 180.0 )
		//PointLight ( 0, eye, ecPosition3, normal, ambient, diffuse, specular );
	//else
	//	SpotLight( 0, eye, ecPosition3, normal, ambient, diffuse, specular );
	
	color = gl_FrontLightModelProduct.sceneColor + ambient * myAmbient + diffuse * myDiffuse;
	
	//color += specular + mySpec;
	
	// Transform vertex to clip space
	gl_Position = ftransform();
}
