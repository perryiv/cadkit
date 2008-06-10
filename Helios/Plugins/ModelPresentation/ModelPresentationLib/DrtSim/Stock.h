
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Wei Chen, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StockDetails loader
// Load information which contains the amount of stock ( Medicine, Vaccine, Doctors, Nurses, Beds ) of the hospital
//									in each step and also the status of the hospital ( Excess or Demand )

// SERV projects -- Santanam Version 2.0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_CLASS_STOCK_H_
#define	_CLASS_STOCK_H_

#include "../ModelPresentationLib.h"

#include <osg/Node>
#include <osg/Switch>
#include <osg/Sequence>

#include <osgText/Text>
#include <osg/Geometry>
#include <osg/Vec3>

#include <string>
#include <vector>




class Stock
{
public:

	bool			setStockFileName ( const std::string &fn );
	osg::Group*		buildStockScene ( );

	void			setAreaSizeX		( const unsigned int &areaX )	{ _areaSizeX = areaX; }
	void			setAreaSizeY		( const unsigned int &areaY )	{ _areaSizeY = areaY; }
	void			setNumOfHospitals	( const unsigned int &nh )		{ _numHospitals = nh;  }

	void			setHospitalReserve( )						{  _hospitals.reserve( _numHospitals );	}
	void			setHospitalCoordinates ( osg::Vec3 &hos )	{  _hospitals.push_back( hos ); }

	void			setNumofTranshipmentSteps( const unsigned int &ns )	{ _numTranSteps = ns; }
	void			setNumofTranshipmentMovements( const unsigned int &nm )	{ _numTranMovements = nm; }
	void			setTranshipmentReserve( )					{ _transhipment.reserve( _numTranSteps ); }
	void			setTranshipmentSteps( const unsigned int &n )		{ _transhipment.push_back( n ); }

	void			setWindowHeight ( const unsigned int &wh )  { _winHeight = wh;    }
	void			setWindowWidth  ( const unsigned int &ww )	{ _winWidth  = ww;	  }
	void			setWindowMargin ( const unsigned int &wm )	{ _winMargin = wm;	  }

  void      setWorkingDir ( const std::string &dir ){ _workingDir = dir; }
  void      setWriter( ModelPresentationLib* writer ){ _mpdWriter = writer; }

	unsigned int	getNumOfStockSteps ( )		{ return _stockSteps.size( ); }
  unsigned int  getTotalSteps() { return ( _stockSteps.size( ) + ( _numTranSteps * _numTranMovements ) ); } 

	osg::Geode*		createStockColorCodeBar ( );

	osg::Switch*	createStockStepSwitch ( );
	osg::Switch*	createStockSwitch ( );

	osg::Sequence*	createStockStepSequence ( );
	osg::Sequence*	createStockSequence ( );


	int		getMedicineStockMaxNum ( )		{ return _medicineStockMax; }
	int		getVaccineStockMaxNum  ( )		{ return _vaccineStockMax;  }
	int		getDoctorsStockMaxNum  ( )		{ return _doctorsStockMax;  }
	int		getNursesStockMaxNum   ( )		{ return _nursesStockMax;   }
	int		getBedsStockMaxNum     ( )		{ return _bedsStockMax;	    }

	void	hide ();
	void	show ();
	bool	shown () const;

	void	resume ();
	void	startOver ();

	Stock( void );
	~Stock( void );

private:

	bool			_StockDetailsLoader ( );
	osg::Geode*		_createShapeOfCylinder ( const osg::Vec3& center, const float &radius, const float &height, const osg::Vec4 &color );
	osg::Geode*		_createShapeOfBox( const osg::Vec3 &center, const float &width, const osg::Vec4 &color );
	osg::Geode*		_createShapeOfBox( const osg::Vec3 &center, const float &lengthX, const float &lengthY, const float &lengthZ, const osg::Vec4 &color );

	osg::Group*		_createStockDetailsbyStep ( const unsigned int &step, unsigned int &counter, unsigned int &tCount );

	osg::MatrixTransform*	_transformTheShapeDrawable( const osg::Vec3 &pos, const osg::Vec3 &scale, osg::Geode *geode);


	const std::string	_intToString( const int& i );
	osgText::Text*	_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osgText::Text*	_createVerticalText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geode*		_createTextAsNode( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geometry*	_createFilledRectangle( const osg::Vec3& lowLeftPos, const osg::Vec3& delta, const osg::Vec4& color, const float& lineWidth );
	osg::Geometry*	_drawPoints( osg::Vec3Array* vertices, osg::Vec4Array* colors, const float& pSize );


	struct	StockDetail	{
		unsigned int		stockStep;
		unsigned int		hospitalID;
		unsigned int		medicineStock;
		unsigned int		vaccineStock;
		unsigned int		doctorsStock;
		unsigned int		nursesStock;
		unsigned int		bedsStock;
		char				hospitalStatus;
	};


	float			_seqTime;
	float			_sTime;
	
	unsigned int	_winHeight;
	unsigned int	_winWidth;
	unsigned int    _winMargin;
	std::string		_filename;

	unsigned int	_areaSizeX;
	unsigned int	_areaSizeY;
	unsigned int	_numHospitals;

	unsigned int    _numTranSteps;
	unsigned int    _numTranMovements;		// == numBezierPoints
	std::vector< unsigned int > _transhipment;

	int				_medicineStockMax;		// set all minimum of stocks zero
	int				_vaccineStockMax;
	int				_doctorsStockMax;
	int				_nursesStockMax;
	int				_bedsStockMax;

	osg::Vec4		_medicineStockColor;
	osg::Vec4		_vaccineStockColor;
	osg::Vec4		_doctorsStockColor;
	osg::Vec4		_nursesStockColor;
	osg::Vec4		_bedsStockColor;

	std::vector< StockDetail >		_stockDetails;
	std::vector< unsigned int >		_stockSteps;

	osg::ref_ptr< osg::Geode >	_medicineCylinder;
	osg::ref_ptr< osg::Geode >	_vaccineCylinder;
	osg::ref_ptr< osg::Geode >	_doctorsCylinder;
	osg::ref_ptr< osg::Geode >  _nursesCylinder;
	osg::ref_ptr< osg::Geode >	_bedsCylinder;

	osg::ref_ptr< osg::Geode >	_medicineBox;
	osg::ref_ptr< osg::Geode >	_vaccineBox;
	osg::ref_ptr< osg::Geode >	_doctorsBox;
	osg::ref_ptr< osg::Geode >  _nursesBox;
	osg::ref_ptr< osg::Geode >	_bedsBox;

	std::vector< osg::Vec3 >		_hospitals;
	osg::ref_ptr < osg::Group >     _root;

	osg::ref_ptr< osg::Switch >		_stockStepSwitch;
	osg::ref_ptr< osg::Switch >		_stockSwitch;
	osg::ref_ptr< osg::Sequence >	_stockStepSequence;
	osg::ref_ptr< osg::Sequence >	_stockSequence;

  std::string _workingDir;
  ModelPresentationLib::RefPtr _mpdWriter;

};

#endif	// _CLASS_STOCK_H_
