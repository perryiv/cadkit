
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
#include "OsgTools/Configure/OSG.h"

#include "OsgTools/IO/GLFeedback.h"

#include <string>
#include <cstdio>

namespace OsgTools { namespace Render { class Viewer; } }

namespace OsgTools {
namespace IO {


class OSG_TOOLS_EXPORT WriteEPS : public GLFeedback
{
public:

	WriteEPS( const std::string& filename, const std::string& creator = "" );
	virtual ~WriteEPS();

	// Write the feedback buffer to file.
  bool  							write( OsgTools::Render::Viewer& viewer );

	// Set/get the threshold for the Gouraud shading algorithm that gets written 
	// to the EPS file. The acceptable range is [0.001,1]. The default is 0.1. 
	// Passing a value out of range will have no effect.
	void							  setGouraudShadingThreshold ( const double &value );
	const double &			getGouraudShadingThreshold() const { return _gouraudThreshold; }

	// Set/get the line smoothing factor. The acceptable range is [0.001,1].
	// The default is 0.06. Passing a value out of range will have no effect.
	void							  setLineSmoothingFactor ( const double &value );
	const double &	  	getLineSmoothingFactor() const { return _lineSmoothingFactor; }

protected:

  std::string _filename;
  std::string _creator;
  bool        _showpage;
	double _gouraudThreshold;
	double _lineSmoothingFactor;
	
  unsigned int _currentMaxBufSize;

	virtual bool			  _calculateBufferSize ( int &size ) const;

	bool							  _writeBuffer ( const float &pointSize, FILE *out ) const;
	bool							  _writeFile() const;
	bool							  _writePrimitive ( const float &pointSize, const int &whichPrim, FILE *out ) const;

private:
  /// Do not use
  WriteEPS();

};


} // namespace OsgTools
} // namespace IO


#endif // _OSG_TOOLS_ENCAPSULATED_POSTSCRIPT_WRITER_H_
