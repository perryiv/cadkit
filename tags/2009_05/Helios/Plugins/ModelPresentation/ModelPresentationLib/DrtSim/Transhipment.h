
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Wei Chen, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transhipments loader
// Load information which contains the details about the transfer of stock from one hospital to other hospitals
//									and from central stockpile to the hospitals.

// SERV projects -- Santanam Version 2.0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_CLASS_TRANSHIPMENT_H_
#define	_CLASS_TRANSHIPMENT_H_

#include "../ModelPresentationLib.h"

#include <osg/Node>
#include <osg/Switch>
#include <osg/Sequence>

#include <osgText/Text>
#include <osg/Geometry>
#include <osg/Vec3>
#include <osg/ShapeDrawable>

#include <string>
#include <vector>



class Transhipment
{
public:
      

	bool			setTranshipmentFileName ( const std::string &fn );
	osg::Group*		buildTranshipmentScene ( );

	void			setAreaSizeX	( const unsigned int &areaX )	{ _areaSizeX = areaX; }
	void			setAreaSizeY	( const unsigned int &areaY )	{ _areaSizeY = areaY; }
	void			setNumOfSteps	( const unsigned int &num   )   { _numOfSteps = num;  }
	
	void			setNumOfHospitals	( const unsigned int &nh )	{ _numHospitals = nh;	}
	void			setHospitalReserve ( )		{ _hospitals.reserve( _numHospitals );	}
	void			setHospitalCoordinates( osg::Vec3 &hos )		{ _hospitals.push_back( hos );	}

  void      setWorkingDir( const std::string &dir ){ _workingDir = dir; }
  void      setWriter( ModelPresentationLib* writer ){ _mpdWriter = writer; }

	osg::Switch*	createTranshipmentSwitch ( );
	osg::Sequence*	createTranshipmentSequence ( );

	// pass to StockDetails and AgentDetails to get control of time ( sequence )
	unsigned int	getNumOfTranshipments ( )		{ return _tranSteps.size(); }
	unsigned int    getNumOfTranMovements ( )		{ return (_numBezierPoints+1); }
	unsigned int	getStepOfTranshipments ( const unsigned int n );




	void	hide ();
	void	show ();
	bool	shown () const;

	void	resume ();
	void	startOver ();


	Transhipment(void) :
		_seqTime			( 1.0f ),
		_sTime				( 12.0f ),
		_filename			( "" ),
		_fromCenter			( false ),
		_areaSizeX			( 0 ),
		_areaSizeY			( 0 ),
		_numOfSteps			( 0 ),
		_numHospitals		( 0 ),
		_numBezierPoints	( 0 ),
		_tranDetails		( 0 ),
		_tranSteps			( 0 ),
		_hospitals			( 0 ),
		_BezierPoints		( 0 ),
		_root				( 0 ),
		_tranSwitch			( 0 ),
		_tranSequence		( 0 )
	{	
	}
	
	~Transhipment(void)	
	{
		_tranDetails.clear ( );
		_tranSteps.clear ( );
		_hospitals.clear ( );
	}



private:

	bool			_TranDetailsLoader ( );
	osg::Vec3		_pointsOnQuadraticBezier( osg::Vec3 *cp, const float t );
	void			_computeQuadraticBezier( );
	osg::Vec3		_findMiddleQuadraticBezierCP( const osg::Vec3 &v1, const osg::Vec3 &v2 );
	osg::Geode*		_createTranshipmentDetailsbyStep( unsigned int id, unsigned int sId, unsigned int eId );

	void			_pickUpRandomColors( );


	const std::string	_intToString( const int& i );
	osgText::Text*		_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geode*			_createTextAsNode( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geometry*		_drawLines( osg::Vec3Array* vertices, osg::Vec4Array* colors, const osg::PrimitiveSet::Mode& mode, const float& width );
	osg::ShapeDrawable*	_createShapeOfSphere( const osg::Vec3 &center, const float &radius, const osg::Vec4 &color );


	struct	TranDetail	{
		unsigned int		tranStep;
		int					fromHospID;			// in this part, the HospitalID can be "-1"
		int					toHospID;
		unsigned int		quantityST;
		char				stockType;
	};


	float			_seqTime;
	float			_sTime;
	
	std::string		_filename;
	bool			_fromCenter;

	unsigned int	_areaSizeX;
	unsigned int	_areaSizeY;
	unsigned int    _numOfSteps;
	unsigned int	_numHospitals;
	unsigned int	_numBezierPoints;		// per curve

	std::vector< TranDetail >		_tranDetails;
	std::vector< unsigned int >		_tranSteps;
	std::vector< osg::Vec3 >		_hospitals;
	std::vector< osg::Vec3 >		_BezierPoints;

	osg::ref_ptr< osg::Vec4Array >	_tranColors;

	osg::ref_ptr< osg::Group >		_root;

	osg::ref_ptr< osg::Switch >		_tranSwitch;
	osg::ref_ptr< osg::Sequence >	_tranSequence;

  std::string                   _workingDir; 

  ModelPresentationLib::RefPtr  _mpdWriter;

};

#endif	// _CLASS_TRANSHIPMENT_H_
