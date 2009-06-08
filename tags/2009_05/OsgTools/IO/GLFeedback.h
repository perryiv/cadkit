
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//	GLFeedback: The feedback buffer renderer class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_OPENGL_FEEDBACK_RENDERER_H_
#define _OSG_TOOLS_OPENGL_FEEDBACK_RENDERER_H_

#include "OsgTools/Export.h"

namespace OsgTools {
namespace IO {


class OSG_TOOLS_EXPORT GLFeedback
{
public:

	enum SortDirection
	{
		BACK_TO_FRONT,	// Sort from back to front, default.
		FRONT_TO_BACK,	// Sort from front to back.
		UNSORTED		// Write to file in order drawn.
	} sortDirection;

	enum SortOption
	{
		MIN_Z,			// Use the minimum z-value of the triangle when sorting, default.
		MAX_Z,			// Use the minimum z-value of the triangle when sorting.
		AVERAGE			// Use the average debth of the triangle's vertices.
	} sortOption;

	bool backFaces;	// Set to false to ignore back faces. Default is true;

	GLFeedback();
	virtual ~GLFeedback();

	enum Result
	{
		FAILED_TO_ALLOCATE_BUFFER,
		FAILED_TO_ALLOCATE_PRIMITIVES,
		FAILED_TO_CALCULATE_BUFFER_SIZE,
		FAILED_TO_COUNT_PRIMITIVES,
		BUFFER_TOO_SMALL,
		NO_RENDER_ERROR
	};

	// Call these immediately before and after you render.
	Result							preRender();
	Result							postRender();

protected:

	struct GL3DColorFeedback
	{
		float x;
		float y;
		float z;
		float r;
		float g;
		float b;
		float a;
	};

	struct Primitive
	{
		float *buffer;
		float depth;
	};

	Primitive* _primitive;
	int _numPrimitives;
	int _maxBufferSize;
	int _actualBufferSize;
	float *_buffer;
	bool _clientSuppliedBuffer;

	virtual bool					_calculateBufferSize ( int &size ) const;
	void							    _calculatePrimitiveDebths();
	static int					  _compareBackToFront ( const void *a, const void *b );
	static int					  _compareFrontToBack ( const void *a, const void *b );
	int							      _countPrimitives() const;

	void							    _deleteBuffer();
	void							    _deletePrimitives();
	
	Result							  _preparePrimitives();

	bool	                _reallocateBufferIfNeeded ( int &size );
	bool							    _reallocatePrimitivesIfNeeded ( int &numPrimitives );
};


}
}

#endif
