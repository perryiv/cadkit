/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SporeDetails loader
// Load information which contains the spore ( disease causing spores ) location and the number time duration in
//		which they are present.
//
// SERV projects -- Santanam Version 2.0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_CLASS_SPORE_H_
#define	_CLASS_SPORE_H_


#include <iostream>
#include <vector>

#include <osg/Node>
#include <osgText/Text>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Vec3>

#include <osg/Geode>
#include <osg/Switch>
#include <osg/Sequence>
#include <osg/ShapeDrawable>



class Spore
{

public:
	
	bool			setSporeFileName ( const std::string &fn );
	void			setNumofSteps ( const unsigned int &nstep )		{ _nSteps = nstep; }
	osg::Group*		buildSporeScene ( );

	void			setAreaSizeX	( const unsigned int &areaX )	{ _areaSizeX = areaX; }
	void			setAreaSizeY	( const unsigned int &areaY )	{ _areaSizeY = areaY; }

	void			setNumofTranshipmentSteps( const unsigned int &ns )	{ _numTranSteps = ns; }
	void			setNumofTranshipmentMovements( const unsigned int &nm ) { _numTranMovements = nm; }
	void			setTranshipmentReserve( )					{ _transhipment.reserve( _numTranSteps ); }
	void			setTranshipmentSteps( const unsigned int &n )		{ _transhipment.push_back( n ); }

	osg::Switch*	createSporeSwitch ( );
	osg::Sequence*	createSporeSequence ( );


	void	hide ();
	void	show ();
	bool	shown () const;

	void	resume ();
	void	startOver ();


	Spore(void):
		_seqTime			( 1.0f ),
		_sTime				( 12.0f ),
		_nSteps				( 0 ),
		_sporeStartingStep  ( 0 ),
		_sporeEndingStep	( 0 ),
		_areaSizeX			( 0 ),
		_areaSizeY			( 0 ),
		_numTranSteps		( 0 ),
		_numTranMovements	( 0 ),
		_transhipment		( 0 ),
		_filename			( "" ),
		_sporeDetails		( 0 ),
		_root				( 0 ),
		_sporeSwitch		( 0 ),
		_sporeSequence		( 0 )
	{
	}
	
	~Spore(void)
	{
		_sporeDetails.clear();
	}


private:

	struct SporeDetail {
		unsigned int	sporeX;
		unsigned int	sporeY;
	};


	bool					_SporeDetailsLoader ( );
	osg::Geode*				_createSporeDetails( );
	osg::ShapeDrawable*		_createShapeOfCylinder( const osg::Vec3 &center, const float &radius, const osg::Vec4 &color );

	osgText::Text*	_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geode*		_createTextAsNode( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geometry*	_drawLines( osg::Vec3Array* vertices, osg::Vec4Array* colors, const osg::PrimitiveSet::Mode& mode, const float& width );
	
	float			_seqTime;
	float			_sTime;

	unsigned int	_nSteps;
	unsigned int	_sporeStartingStep;
	unsigned int	_sporeEndingStep;

	unsigned int	_areaSizeX;
	unsigned int	_areaSizeY;

	unsigned int    _numTranSteps;
	unsigned int    _numTranMovements;
	std::vector< unsigned int > _transhipment;

	std::string		_filename;

	std::vector< SporeDetail >		_sporeDetails;

	osg::ref_ptr < osg::Group >		_root;

	osg::ref_ptr< osg::Switch >		_sporeSwitch;
	osg::ref_ptr< osg::Sequence >	_sporeSequence;


};

#endif	// _CLASS_SPORE_H_
