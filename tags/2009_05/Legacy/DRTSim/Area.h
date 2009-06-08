/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AreaDetails loader
// Load information which contains the total area dimensions, number of rows and columns, and location of hospitals

// SERV projects -- Santanam Version 2.0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_CLASS_AREA_H_
#define	_CLASS_AREA_H_


#include <osg/Node>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include <osgText/Text>
#include <osg/Geometry>
#include <osg/Vec3>

#include <string>
#include <vector>


class Area
{
public:
	
	bool					setAreaFileName ( const std::string& fn );
	osg::Geode*				buildAreaScene ( );	
	
	const unsigned int		getAreaSizeX ( ) const	{ return _areaSizeX; }
	const unsigned int		getAreaSizeY ( ) const	{ return _areaSizeY; }
	const unsigned int		getAreaRows ( )  const	{ return _areaRows;  }
	const unsigned int		getAreaCols ( )  const	{ return _areaCols;  }

	const unsigned int		getHospitalCoordinateX ( const unsigned int id );
	const unsigned int		getHospitalCoordinateY ( const unsigned int id );

	osg::Vec3				getHospitalCoordinates ( const unsigned int id );

	const unsigned int		getNumOfHospitals ( )	{ return _numHospitals; }

	// constructor and deconstructor
	Area ( ) :
		_areaSizeX		( 0 ),
		_areaSizeY		( 0 ),
		_areaRows		( 0 ),
		_areaCols		( 0 ),
		_numHospitals	( 0 ),
		_filename		( "" ),
		_areaDetails	( 0 )
	{
	}

	~Area( )	{ _areaDetails.clear ( ); }


private:

	bool					_AreaDetailsLoader ( );
	osg::ShapeDrawable*		_createShapeOfSphere( const osg::Vec3 &center, const float &radius, const osg::Vec4 &color );
	osg::MatrixTransform*	_transformTheShapeDrawable( const osg::Vec3 &pos, osg::Geode *geode );

	const std::string	_intToString( const int& i );
	osgText::Text*	_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geometry*	_drawLines( osg::Vec3Array* vertices, osg::Vec4Array* colors, const osg::PrimitiveSet::Mode& mode, const float& width );

//	osg::Geometry* createAreaFrame ( );
//	osg::Geometry* createAreaLines ( );
//	osg::Geometry* createHospitalDots ( );

	struct AreaDetail {
		unsigned int	hospitalID;
		unsigned int	hospitalGrid;
		unsigned int	hospitalX;
		unsigned int	hospitalY;
	};

	unsigned int	_areaSizeX;
	unsigned int	_areaSizeY;
	unsigned int	_areaRows;
	unsigned int	_areaCols;
	unsigned int	_numHospitals;
	std::string		_filename;

	std::vector< AreaDetail > _areaDetails;


};

#endif	// _CLASS_AREA_H_
