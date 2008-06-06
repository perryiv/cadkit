/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <ctime>

#include <osg/ref_ptr>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include "osgDB/WriteFile"

#include <sstream>


#include "Usul/Endian/Endian.h"
#include "Usul/File/Stats.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Format.h"

#include "Stock.h"

#define debug 1



// constructor and deconstructor
Stock::Stock( )
{
	_seqTime			= 1.0f;
	_sTime				= 12.0f;

	_winHeight			=	1024;
	_winWidth			=	1280;
	_winMargin			=	50;
	_filename			=	"";
	_areaSizeX			=	0;
	_areaSizeY			=	0;
	_numHospitals		=	0;
	_numTranSteps		=   0;
	_numTranMovements   =   1;

	_medicineStockMax	=	0;
	_vaccineStockMax	=	0;
	_doctorsStockMax	=	0;
	_nursesStockMax		=	0;
	_bedsStockMax		=	0;

	_medicineStockColor.set	( 1.0f, 1.0f, 102.0/255.0f, 1.0f );
	_vaccineStockColor.set	( 155.0/255.0f, 0.0f, 102.0/255.0f, 1.0f );
	_doctorsStockColor.set	( 0.0f, 204.0/255.0f, 1.0f,  1.0f );
	_nursesStockColor.set	( 204.0/255.0f, 102.0/255.0f, 1.0f, 1.0f );
	_bedsStockColor.set		( 0.0f, 1.0f, 153.0/255.0f, 1.0f ),

	_medicineCylinder		= 0x0;
	_vaccineCylinder		= 0x0;
	_doctorsCylinder		= 0x0;
	_nursesCylinder			= 0x0;
	_bedsCylinder			= 0x0;

	_medicineBox			= 0x0;
	_vaccineBox				= 0x0;
	_doctorsBox				= 0x0;
	_nursesBox				= 0x0;
	_bedsBox				= 0x0;

	_root                   = 0x0;
	_stockStepSwitch		= 0x0;
	_stockSwitch			= 0x0;
	_stockStepSequence		= 0x0;
	_stockSequence			= 0x0;

}


Stock::~Stock( )
{
	_stockDetails.clear ( );

	_stockSteps.clear ( );

}



osg::Group*		Stock::buildStockScene ( )
{
	_root = new osg::Group;

#if 1 // agent
  {
    std::string path = "C:/data/Santanam/output/text/";
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    std::string filename = Usul::Strings::format( path, "stockText.osg" );
    group->addChild( _createTextAsNode( osg::Vec3( 100.0f, -30.0f, 0.0f), osg::Vec4( 1.0f, 0.5f, 0.5f, 1.0f ), 20.0f, " Stock " ) );
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
    _root->addChild( group.get() ); 
  }
#endif
	_root->addChild( createStockSequence ( ) );

	return _root.get();

}




bool			Stock::setStockFileName( const std::string &fn )
{
	
	_medicineCylinder		= new osg::Geode;
	_vaccineCylinder		= new osg::Geode;
	_doctorsCylinder		= new osg::Geode;
	_nursesCylinder			= new osg::Geode;
	_bedsCylinder			= new osg::Geode;

	_medicineBox			= new osg::Geode;
	_vaccineBox				= new osg::Geode;
	_doctorsBox				= new osg::Geode;
	_nursesBox				= new osg::Geode;
	_bedsBox				= new osg::Geode;

	const osg::Vec3 center	( 0.0f, 0.0f, 0.0f );
	const float radius		( 4.0f );
	const float height		( 1.0f );

	_filename = fn;

	if( _StockDetailsLoader( ) )
	{
		// cylinders	
		_medicineCylinder	=  _createShapeOfCylinder( center, radius, height, _medicineStockColor );
		_vaccineCylinder	=  _createShapeOfCylinder( center, radius, height, _vaccineStockColor );
		_doctorsCylinder	=  _createShapeOfCylinder( center, radius, height, _doctorsStockColor );
		_nursesCylinder		=  _createShapeOfCylinder( center, radius, height, _nursesStockColor );
		_bedsCylinder		=  _createShapeOfCylinder( center, radius, height, _bedsStockColor );	// dynamic_cast< osg::Geode* > ( _createShapeOfCylinder( center, radius, height, _bedsStockColor ) );

		// boxes
		_medicineBox	=  _createShapeOfBox( center, radius, radius, height, _medicineStockColor );
		_vaccineBox		=  _createShapeOfBox( center, radius, radius, height, _vaccineStockColor );
		_doctorsBox		=  _createShapeOfBox( center, radius, radius, height, _doctorsStockColor );
		_nursesBox		=  _createShapeOfBox( center, radius, radius, height, _nursesStockColor );
		_bedsBox		=  _createShapeOfBox( center, radius, radius, height, _bedsStockColor );	


		return true;
	}

	return false;

}



osg::Geode*		Stock::createStockColorCodeBar( )
{

	osg::ref_ptr< osg::Geode > geode ( new osg::Geode );


	float characterSize = 26.0f;
	float linewidth		= 5.0f;
	float delta			= 80.0f;
	float zValue		= 0.0f;

	float cf0			= 1.2f;
	float cf1			= 3.0f;
	float cf2			= 0.7f;

	unsigned int	numRectangles = 5;
	unsigned int	layer		  = 7;

	osg::Vec3	rectangleDelta ( delta, delta/2.0, zValue );


	std::vector< osg::Vec3 >	rectPos  ( numRectangles );
	std::vector< osg::Vec3 >	colorPos ( numRectangles );

	for( unsigned i = 0; i < numRectangles; ++i )
	{
		rectPos[i].set(  osg::Vec3( _winMargin, _winHeight - cf1*_winMargin - cf2*layer*delta, zValue ) );
		colorPos[i].set( osg::Vec3( _winMargin+cf0*delta, _winHeight - cf1*_winMargin - cf2*layer*delta + delta/4.0f, zValue ) );
		
		layer++;
	}

	// add stock color code bar
	geode->addDrawable( _createFilledRectangle( rectPos[0], rectangleDelta, _medicineStockColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[0], _medicineStockColor, characterSize, "Medicine Stock"));

	geode->addDrawable( _createFilledRectangle( rectPos[1], rectangleDelta, _vaccineStockColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[1], _vaccineStockColor, characterSize, "Vaccine Stock"));

	geode->addDrawable( _createFilledRectangle( rectPos[2], rectangleDelta, _doctorsStockColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[2], _doctorsStockColor, characterSize, "Doctors Stock"));

	geode->addDrawable( _createFilledRectangle( rectPos[3], rectangleDelta, _nursesStockColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[3], _nursesStockColor, characterSize, "Nurses Stock"));

	geode->addDrawable( _createFilledRectangle( rectPos[4], rectangleDelta, _bedsStockColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[4], _bedsStockColor, characterSize, "Beds Stock"));


	return geode.release();

}



// Either Stock::createStockStepSwithc( ) or Agent::createAgentStepSwitch( )  // same num of steps
osg::Switch*	Stock::createStockStepSwitch ( )
{

	float zValue			= 0.0f;
	float characterSize		= 26.0f;

	osg::Vec3 pos ( _winMargin, _winHeight - _winMargin, zValue );
	osg::Vec4 layoutColor ( 0.8f, 0.6f, 0.4f, 1.0f );

	_stockStepSwitch = new osg::Switch;

	if( _stockDetails.empty() )
	{
		for( unsigned int i = 0; i < _stockSteps.size(); ++i )
		{
			std::string		str = " Current Step = " + _intToString( i );

			_stockStepSwitch->addChild( _createTextAsNode( pos, layoutColor, characterSize, str ) );
		}
	}

	_stockStepSwitch->setSingleChildOn ( 0 );


	return	_stockStepSwitch.get( );

}



osg::Switch*	Stock::createStockSwitch ( )
{

	_stockSwitch = new osg::Switch;

	if( !_stockDetails.empty() )
	{
		for( unsigned int i = 0; i < _stockSteps.size(); ++i )
			_stockSwitch->addChild( _createStockDetailsbyStep( i ) );

		_stockSwitch->setSingleChildOn( 0 );	
	}


	return _stockSwitch.get( );

}



// Either Stock::createStockStepSequence() or Agent::createAgentStepSequence()   // same num of steps
osg::Sequence*	Stock::createStockStepSequence ( )
{

	float zValue = 0.0f;
	float characterSize = 26.0f;

	osg::Vec3 pos ( _winMargin, _winHeight-_winMargin, zValue );
	osg::Vec4 layoutColor ( 0.8f, 0.6f, 0.4f, 1.0f );
	
	_stockStepSequence = new osg::Sequence;

	_stockStepSequence->addChild( new osg::Geode );
	_stockStepSequence->setTime( _stockStepSequence->getNumChildren()-1, _sTime );

	if ( !_stockDetails.empty() )
	{
		unsigned int count = 0;

		for( unsigned int i = 0; i < _stockSteps.size(); ++i )
		{
			std::string		str = "Current Step = " + _intToString( i + 1 );

			_stockStepSequence->addChild( _createTextAsNode( pos, layoutColor, characterSize, str ) );
			
			if( ( count < _numTranSteps ) && ( i == (_transhipment[count]-1) ) )
			{
				_stockStepSequence->setTime( _stockStepSequence->getNumChildren()-1, _seqTime*_numTranMovements );
				count++;
			}
			else
			{
				_stockStepSequence->setTime( _stockStepSequence->getNumChildren()-1, _seqTime );
			}

		}

		// loop through all children
		_stockStepSequence->setInterval( osg::Sequence::LOOP, 0,-1 );

		// real-time playback, repeat indefinitively
		_stockStepSequence->setDuration( 1.0f, -1 );

		// must be started explicitly
		_stockStepSequence->setMode( osg::Sequence::START );

	}


	return _stockStepSequence.get();

}



osg::Sequence*	Stock::createStockSequence ( )
{

	_stockSequence = new osg::Sequence;

	_stockSequence->addChild( new osg::Geode );
	_stockSequence->setTime( _stockSequence->getNumChildren()-1, _sTime );

	if( !_stockDetails.empty() )
	{
		unsigned int count ( 0 );

		for( unsigned int i = 0; i < _stockSteps.size(); ++i )
		{
			_stockSequence->addChild( _createStockDetailsbyStep( i ) );

			if( ( count < _numTranSteps ) && ( i == (_transhipment[count]-2) ) )		// before transhipment
			{
				_stockSequence->setTime( _stockSequence->getNumChildren()-1, _seqTime*_numTranMovements );
				count++;
			}
			else
			{
				_stockSequence->setTime( _stockSequence->getNumChildren()-1, _seqTime );
			}

		}

		// loop through all children
		_stockSequence->setInterval( osg::Sequence::LOOP, 0, -1 );

		// real-time playback, repeat indefinitively
		_stockSequence->setDuration( 1.0f, -1 );

		// must be started explicitly
		_stockSequence->setMode( osg::Sequence::START );

	}


	return _stockSequence.get();

}



// Box
osg::Geode*		Stock::_createShapeOfBox( const osg::Vec3 &center, const float &lengthX, const float &lengthY, const float &lengthZ, const osg::Vec4 &color )
{
	osg::ref_ptr< osg::Geode >	geode ( new osg::Geode );

		osg::ref_ptr< osg::ShapeDrawable >	box ( new osg::ShapeDrawable );
		// box->setUseDisplayList ( false );

		osg::ref_ptr< osg::TessellationHints >	hints ( new osg::TessellationHints );
		hints->setDetailRatio( 2.0f );
		
		box = new osg::ShapeDrawable( new osg::Box( center, lengthX, lengthY, lengthZ ), hints.get() );
		box->setColor( color );

	geode->addDrawable( box.release( ) );

	return geode.release();

}


// Cylinder
osg::Geode*		Stock::_createShapeOfCylinder( const osg::Vec3 &center, const float &radius, const float &height, const osg::Vec4 &color )
{

	osg::ref_ptr< osg::Geode >	geode ( new osg::Geode );

		osg::ref_ptr< osg::ShapeDrawable >		cylinder ( new osg::ShapeDrawable );
		// cylinder->setUseDisplayList ( false );

		osg::ref_ptr< osg::TessellationHints >	hints ( new osg::TessellationHints );
		hints->setDetailRatio( 2.0f );
		
		cylinder = new osg::ShapeDrawable( new osg::Cylinder( center, radius, height ), hints.get() );
		cylinder->setColor( color );

	geode->addDrawable( cylinder.release() );


	return geode.release();

}


osg::MatrixTransform*		Stock::_transformTheShapeDrawable( const osg::Vec3 &pos, const osg::Vec3 &scale, osg::Geode *geode )
{
	osg::ref_ptr< osg::MatrixTransform >	transform ( new osg::MatrixTransform );

		osg::Matrix		S = osg::Matrix::scale( scale );
		osg::Matrix		T = osg::Matrix::translate( pos );
		transform->setDataVariance ( osg::Object::STATIC );
		transform->setMatrix( S * T );

		transform->addChild ( geode );


	return transform.release();

}



osg::Group*		Stock::_createStockDetailsbyStep( const unsigned int &step )
{

	const float heightMax	( 100.0f );
	const float heightMin	( 0.2f );		// 0.2f
	float		height		( 1.0f );
	float		zValue		( 2.0f );			// yValue
	float		pSize		( 5.0f );
	
	osg::ref_ptr< osg::Group >	group ( new osg::Group );
	osg::ref_ptr< osg::Geode >  geode ( new osg::Geode );

	unsigned int startIndex = 0;
	for( unsigned int i = 0; i < step; ++i )
		startIndex += _stockSteps[i];

	osg::ref_ptr< osg::Vec3Array >	vertices	( new osg::Vec3Array );
	osg::ref_ptr< osg::Vec4Array >	colors		( new osg::Vec4Array );
	
	unsigned int	hospitalID ( 0 );
	unsigned int	count ( 0 );
	float			characterSize = 5.0f;		// for text ( num of stocks )
	float			delta	= 0.4f;		// 0.4f
	float			radius  = 4.0f;

	float			yPos	= 10.0f;

	osg::Vec3		center;
	osg::Vec3		centerDelta;
	osg::Vec3		textDelta;
	osg::Vec3		textPos;
	osg::Vec3		cylinderPos;
	osg::Vec3		scale;

	for( unsigned int n = startIndex; n < startIndex + _stockSteps[step]; ++n )
	{
		hospitalID	= _stockDetails[n].hospitalID;	
		center.set( _hospitals[hospitalID].x(), _areaSizeY - _hospitals[hospitalID].y(), zValue );
		vertices->push_back( center );

		switch( _stockDetails[n].hospitalStatus )
		{
			case 'D':
			case 'd':
				colors->push_back( osg::Vec4( 0.5f, 0.0f, 0.0f, 1.0f ) );
				break;

			case 'E':
			case 'e':
				colors->push_back( osg::Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
				break;

			default:
				colors->push_back( osg::Vec4( 0.7f, 0.7f, 0.7f, 1.0f ) );
				break;
		}

		
		{
			// medicine cylinder	-- all the minimum of stock set to zero ( 0 )
			count		= _stockDetails[n].medicineStock;
			if( count <= 0 )	height	= heightMin;
			else
				height = heightMin + ( (float)count / (float)( _medicineStockMax ) ) * ( heightMax - heightMin );

			centerDelta.set( -8.0*delta, yPos, height/2.0 + 10.0*delta );
			textDelta.set  ( -15.0*delta, centerDelta.y(), 2.0*centerDelta.z() );

			textPos.set( center + textDelta );
			cylinderPos.set( center + centerDelta );
			scale.set( 1.0f, 1.0f, height );

			geode->addDrawable( _createText( textPos, _medicineStockColor, characterSize, _intToString( count ) ) );

			// use transform ( cylinder or box )
			group->addChild ( _transformTheShapeDrawable( cylinderPos, scale, _medicineCylinder.get() ) );			// use 1.30G PF

			// using box
//			group->addChild( _createShapeOfCylinder( cylinderPos, height, _medicineStockColor ) );
//			group->addChild ( _transformTheShapeDrawable( cylinderPos, scale, _medicineBox.get() ) );		// use 1.30G PF

//			group->addChild( _createShapeOfCylinder( cylinderPos, radius, height, _medicineStockColor ) );		// use 1.45G PF
//			group->addChild( OsgTools::ShapeFactory::cylinder( radius, 6, osg::Vec3( 0.0f, 0.0f, 0.0f ), osg::Vec3( 0.0f, 1.0f, 0.0f), true ) );
			
			
		}


		{
			// vaccine cylinder
			count		= _stockDetails[n].vaccineStock;
			if( count <= 0 )	height	= heightMin;
			else
				height = heightMin + ( (float)count / (float)( _vaccineStockMax ) ) * ( heightMax - heightMin );

			centerDelta.set( -8.0*delta + 2.2*radius, yPos, height/2.0 + 10.0*delta );
			textDelta.set  ( -10.0*delta + 2.2*radius, centerDelta.y(), 2.0*centerDelta.z() );

			textPos.set( center + textDelta );
			cylinderPos.set( center + centerDelta );
			scale.set( 1.0f, 1.0f, height );

			geode->addDrawable( _createText( textPos, _vaccineStockColor, characterSize, _intToString( count ) ) );

			group->addChild ( _transformTheShapeDrawable( cylinderPos, scale, _vaccineCylinder.get() ) );
	
		}


		{
			// doctors cylinder
			count		= _stockDetails[n].doctorsStock;
			if( count <= 0 )	height	= heightMin;
			else
				height = heightMin + ( (float)count / (float)( _doctorsStockMax ) ) * ( heightMax - heightMin );

			centerDelta.set( -8.0*delta + 4.4*radius, yPos, height/2.0 + 10.0*delta );
			textDelta.set  ( -10.0*delta + 4.4*radius, centerDelta.y(), 2.0*centerDelta.z() );

			textPos.set( center + textDelta );
			cylinderPos.set( center + centerDelta );
			scale.set( 1.0f, 1.0f, height );

			geode->addDrawable( _createText( textPos, _doctorsStockColor, characterSize, _intToString( count ) ) );

			group->addChild ( _transformTheShapeDrawable( cylinderPos, scale, _doctorsCylinder.get() ) );
	
		}


		{
			// nurses cylinder
			count		= _stockDetails[n].nursesStock;
			if( count <= 0 )	height	= heightMin;
			else
				height = heightMin + ( (float)count / (float)( _nursesStockMax ) ) * ( heightMax - heightMin );

			centerDelta.set( -8.0*delta + 6.6*radius, yPos, height/2.0 + 10.0*delta );
			textDelta.set  ( -10.0*delta + 6.6*radius, centerDelta.y(), 2.0*centerDelta.z() );

			textPos.set( center + textDelta );
			cylinderPos.set( center + centerDelta );
			scale.set( 1.0f, 1.0f, height );

			geode->addDrawable( _createText( textPos, _nursesStockColor, characterSize, _intToString( count ) ) );

			group->addChild ( _transformTheShapeDrawable( cylinderPos, scale, _nursesCylinder.get() ) );
	
		}


		{
			// beds cylinder
			count		= _stockDetails[n].bedsStock;
			if( count <= 0 )	height	= heightMin;
			else
				height = heightMin + ( (float)count / (float)( _bedsStockMax ) ) * ( heightMax - heightMin );

			centerDelta.set( -8.0*delta + 8.8*radius, yPos, height/2.0 + 10.0*delta );
			textDelta.set  ( -10.0*delta + 8.8*radius, centerDelta.y(), 2.0*centerDelta.z() );

			textPos.set( center + textDelta );
			cylinderPos.set( center + centerDelta );
			scale.set( 1.0f, 1.0f, height );

			geode->addDrawable( _createText( textPos, _bedsStockColor, characterSize, _intToString( count ) ) );

			group->addChild ( _transformTheShapeDrawable( cylinderPos, scale, _bedsCylinder.get() ) );
	
		}

		

	}

	geode->addDrawable( _drawPoints( vertices.release(), colors.release(), pSize ) );


	group->addChild( geode.get( ) );
#if 1 // agent
  {
    std::string stepValue = "";
    if( step < 10 )
      stepValue = Usul::Strings::format( "00", step );
    if( step < 100 && step >= 10 )
      stepValue = Usul::Strings::format( "0", step );
    if( step >= 100 )
      stepValue = Usul::Strings::format( step );
    std::string path = "C:/data/Santanam/output/stock/";
    std::string filename = Usul::Strings::format( path, "stock", stepValue, ".osg" );
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
  }
#endif

	return group.release();

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// loader stock details data from input file
// Format for StockDetail.dat ( Java binary file, big Endian ):
//		Step ( int ), Hospital ID ( int ), Medicine count ( int ), Vaccine count ( int ), Doctors count ( int ),
//		Nurses count ( int ), Beds count ( int ), Hospital Status ( char )
//		... ... ( Number of stocks )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool			Stock::_StockDetailsLoader( )
{
	
	std::cout << "Stock filename: " << _filename << std::endl;

	const Usul::Types::Uint64 fileSize ( Usul::File::size ( _filename ) );

	std::ifstream infile( _filename.c_str(), std::ios::in | std::ios::binary);

	if (! infile)
	{
		std::cerr << "Error: unable to open StockDetails file!\n";
		return false;
	}

	// in Java binary, 1 byte size of "char" == 2 * 1 byte size of "char" in C / C++
	const Usul::Types::Uint64 rowBytes ( 7 * sizeof ( int ) + 2*sizeof ( char ) );
	const Usul::Types::Uint64 numStocks ( fileSize / rowBytes );
	USUL_ASSERT ( fileSize == numStocks * rowBytes );

#if debug
	clock_t		startTime		= ::clock();
	clock_t		updateDuration	= 1000;			// 1 second
#endif

	int		tValue ( 0 );
	char	status = ' ';

	int		numSteps ( 0 );
	int		count ( 0 );

	_stockDetails.reserve( numStocks );
	StockDetail tmp;

	for( Usul::Types::Uint64 i = 0; i < numStocks; ++i )
	{
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.stockStep = static_cast< unsigned int > ( ::abs ( tValue ) );
		
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.hospitalID = static_cast< unsigned int > ( ::abs ( tValue ) );

		
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.medicineStock = static_cast< unsigned int > ( ::abs ( tValue ) );

		// set _medicineStockMin zero even the minimum medicine count is not zero
		if( tValue > _medicineStockMax )		_medicineStockMax = tValue;		// medicine count

		
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.vaccineStock = static_cast< unsigned int > ( ::abs ( tValue ) );
		if( tValue > _vaccineStockMax )			_vaccineStockMax = tValue;   // vaccine count


		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.doctorsStock = static_cast< unsigned int > ( ::abs ( tValue ) );
		if( tValue > _doctorsStockMax )			_doctorsStockMax = tValue;   // doctors count


		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.nursesStock = static_cast< unsigned int > ( ::abs ( tValue ) );
		if( tValue > _nursesStockMax )			_nursesStockMax = tValue;   // nurses count
		

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.bedsStock = static_cast< unsigned int > ( ::abs ( tValue ) );
		if( tValue > _bedsStockMax )			_bedsStockMax = tValue;   // beds count
		

		// infile.read( ( char * ) &status, sizeof( char ) );		//== infile.get(status);
		infile.get( status );
		infile.get( status );
		tmp.hospitalStatus = status;

		_stockDetails.push_back(tmp);


		// put number of stockSteps
		if( tmp.stockStep == numSteps)
		{
			++count;
		}
		else
		{
			_stockSteps.push_back(count);

			++numSteps;
			count = 1;
		}

#if debug
		clock_t currentTime = ::clock();
		char c				= ' ';
		if ( ( currentTime - startTime ) > updateDuration )
		{
			startTime = currentTime;
			
			std::cout << tmp.stockStep << c << tmp.hospitalID << c << tmp.medicineStock << c << tmp.vaccineStock << c << tmp.doctorsStock << c << tmp.nursesStock << c << tmp.bedsStock << c << tmp.hospitalStatus << std::endl;
		}
#endif


	}

	
	_stockSteps.push_back(count);		// the last step


#if debug
//	for(unsigned int w=0; w < _stockSteps.size(); ++w)
//		std::cout << "stockStep " << w << ": " << _stockSteps[w] << std::endl;

//	std::cout << "Stock totalSteps ...: " << _stockSteps.size() << std::endl;

	std::cout << "stock maximum: M " << _medicineStockMax << " V " << _vaccineStockMax << " D " << _doctorsStockMax << " N " << _nursesStockMax << " B " << _bedsStockMax << std::endl;
	std::cout << "stock steps: " << _stockSteps.size() << std::endl;
	
#endif

	infile.close();
	
	return true;

}


void Stock::hide ()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0x0 );
}

void Stock::show()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0xff );
}

bool Stock::shown () const
{
	return _root.valid () ? 0x0 != _root->getNodeMask() : false;
}


void Stock::resume ()
{
	
	osg::Sequence::SequenceMode mode = _stockSequence->getMode();

    if ( mode == osg::Sequence::STOP ) 
	{
        mode = osg::Sequence::START;
        // std::cerr << "Start" << std::endl;
    }
    else if ( mode == osg::Sequence::PAUSE ) 
	{
        mode = osg::Sequence::RESUME;
        // std::cerr << "Resume" << std::endl;
    }
    else {
        mode = osg::Sequence::PAUSE;
        // std::cerr << "Pause" << std::endl;
    }
    
	_stockSequence->setMode( mode );
	// _stockStepSequence->setMode( mode );

}


void Stock::startOver ()
{
	osg::Sequence::SequenceMode mode = osg::Sequence::START;

	_stockSequence->setMode( mode );

}


////////////// Utility

const std::string	Stock::_intToString(const int &num)
{
	std::stringstream ss;

	ss << num;
	
	return ss.str();
}


osgText::Text*		Stock::_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str)
{
	osg::ref_ptr< osgText::Text> text ( new osgText::Text );
	osg::ref_ptr< osgText::Font> font = osgText::readFontFile( "fonts/arial.ttf" );

		text->setAxisAlignment( osgText::Text::SCREEN );	// billboard-style text always facing the viewpoint
		text->setAlignment( osgText::Text::LEFT_BASE_LINE );
		text->setFontResolution( 64, 64 );

		text->setFont( font.get() );
        text->setColor( layoutColor );
        text->setCharacterSize( characterSize );
        text->setPosition( pos );

        // the default layout is left to right, typically used in languages
        // originating from europe such as English, French, German, Spanish etc..
        text->setLayout( osgText::Text::LEFT_TO_RIGHT );

		text->setText( str );
    
	return text.release();

}



osg::Geode*		Stock::_createTextAsNode( const osg::Vec3 &pos, const osg::Vec4 &layoutColor, const float &characterSize, const std::string &str )
{
	osg::ref_ptr< osg::Geode > geode (new osg::Geode );

	geode->addDrawable ( _createText( pos, layoutColor, characterSize, str ) );

	return geode.release();

}



osgText::Text*	Stock::_createVerticalText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str)
{
	osg::ref_ptr< osgText::Text> text ( new osgText::Text );
	osg::ref_ptr< osgText::Font> font = osgText::readFontFile( "fonts/arial.ttf" );

		text->setAxisAlignment( osgText::Text::SCREEN );	// billboard-style text always facing the viewpoint
		text->setAlignment( osgText::Text::LEFT_BASE_LINE );
		text->setFontResolution( 64, 64 );

		text->setFont( font.get() );
        text->setColor( layoutColor );
        text->setCharacterSize( characterSize );
        text->setPosition( pos );

        // the default layout is left to right, typically used in languages
        // originating from europe such as English, French, German, Spanish etc..
		text->setLayout( osgText::Text::VERTICAL );

		text->setText( str );
    
	return text.release();

}



osg::Geometry*	Stock::_createFilledRectangle( const osg::Vec3& lowLeftPos, const osg::Vec3& delta, const osg::Vec4& color, const float& lineWidth)
{
	osg::ref_ptr< osg::Geometry> rectangle ( new osg::Geometry );

	float zValue = 0.0f;
        
	osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array(4) );

		(*vertices)[0].set(lowLeftPos.x(),				lowLeftPos.y(),				zValue);
        (*vertices)[1].set(lowLeftPos.x(),				lowLeftPos.y() + delta.y(),	zValue);
        (*vertices)[2].set(lowLeftPos.x() + delta.x(),	lowLeftPos.y() + delta.y(),	zValue);
        (*vertices)[3].set(lowLeftPos.x() + delta.x(),	lowLeftPos.y(),				zValue);

        // pass the created vertex array to the points geometry object.
        rectangle->setVertexArray( vertices.get() );
        
        // set the colors as before, plus using the aobve
		osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
		colors->push_back( color );
        rectangle->setColorArray( colors.get() );
        rectangle->setColorBinding( osg::Geometry::BIND_OVERALL );
        

        // set the normal in the same way color.
		osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
        normals->push_back( osg::Vec3(0.0f,0.0f,-1.0f) );
        rectangle->setNormalArray( normals.get() );
        rectangle->setNormalBinding( osg::Geometry::BIND_OVERALL );


        // This time we simply use primitive, and hardwire the number of coords to use 
        // since we know up front,
        rectangle->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS,0,4 ) );

		// Turn of lighting for line and set line width.
		rectangle->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
		
		osg::ref_ptr< osg::LineWidth > linewidth ( new osg::LineWidth );
		linewidth->setWidth( lineWidth );
		rectangle->getOrCreateStateSet()->setAttributeAndModes( linewidth.get(), osg::StateAttribute::ON );

	return rectangle.release();

}



// Draw points ( dots )
osg::Geometry*	Stock::_drawPoints( osg::Vec3Array* vertices, osg::Vec4Array* colors, const float& pSize )
{
	osg::ref_ptr < osg::Geometry > pointsGeom ( new osg::Geometry );

        // pass the created vertex array to the points geometry object.
        pointsGeom->setVertexArray( vertices );
               
        pointsGeom->setColorArray( colors );

		if ( colors->size() == 1 )
			pointsGeom->setColorBinding( osg::Geometry::BIND_OVERALL );   
		else
			pointsGeom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
        
        
		osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
        normals->push_back( osg::Vec3( 0.0f, 0.0f, -1.0f ) );
        
		pointsGeom->setNormalArray( normals.get() );
        pointsGeom->setNormalBinding( osg::Geometry::BIND_OVERALL );


        pointsGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );

		// Turn of lighting for points and set point size.
		pointsGeom->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF );

		osg::ref_ptr< osg::Point > point ( new osg::Point );
		point->setSize( pSize );

		pointsGeom->getOrCreateStateSet()->setAttributeAndModes( point.get(), osg::StateAttribute::ON );


	return pointsGeom.release();

}
