
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//	EPS writer class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_ENCAPSULATED_POSTSCRIPT_WRITER_H_
#define _OSG_TOOLS_ENCAPSULATED_POSTSCRIPT_WRITER_H_

#include "OsgTools/Export.h"

namespace OsgTools {
namespace IO {


class OSG_TOOLS_EXPORT WriteEPS
{
public:

	WriteEPS();
	virtual ~WriteEPS();

	// Write the feedback buffer to file.
	SlBool							write();

	// Set/get the threshold for the Gouraud shading algorithm that gets written 
	// to the EPS file. The acceptable range is [0.001,1]. The default is 0.1. 
	// Passing a value out of range will have no effect.
	void							setGouraudShadingThreshold ( const SlFloat64 &value );
	const SlFloat64 &				getGouraudShadingThreshold() const { return _gouraudThreshold; }

	// Set the callback for rendering.
	void							setRenderCallback ( WriteEPSCB *callback, const void *data );

	// Set/get the line smoothing factor. The acceptable range is [0.001,1].
	// The default is 0.06. Passing a value out of range will have no effect.
	void							setLineSmoothingFactor ( const SlFloat64 &value );
	const SlFloat64 &				getLineSmoothingFactor() const { return _lineSmoothingFactor; }

protected:

	SlFloat64 _gouraudThreshold;
	SlFloat64 _lineSmoothingFactor;
	WriteEPSCB *_callback;
	const void *_data;
	SlUint64 _currentMaxBufSize;

	virtual SlBool					_calculateBufferSize ( SlInt32 &size ) const;

	SlBool							_writeBuffer ( const SlFloat32 &pointSize, FILE *out ) const;
	SlBool							_writeFile() const;
	SlBool							_writePrimitive ( const SlFloat32 &pointSize, const SlInt32 &whichPrim, FILE *out ) const;
};


} // namespace OsgTools
} // namespace IO


#endif // _OSG_TOOLS_ENCAPSULATED_POSTSCRIPT_WRITER_H_
