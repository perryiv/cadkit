/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
#include "osgDB/WriteFile"

#include <sstream>

#include "Usul/Endian/Endian.h"
#include "Usul/File/Stats.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Format.h"

#include "Transhipment.h"



#define debug 1



osg::Group*		Transhipment::buildTranshipmentScene( )
{

	_root = new osg::Group ;

#if 1 
  {
    std::string path = "C:/data/Santanam/output/text/";
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    std::string filename = Usul::Strings::format( path, "transText.osg" );
    group->addChild( _createTextAsNode( osg::Vec3( 200.0f, -30.0f, 0.0f), osg::Vec4( 1.0f, 0.5f, 0.5f, 1.0f ), 20.0f, " Transipment " ) );
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
    _root->addChild( group.get() ); 
  }
#endif
	_root->addChild( createTranshipmentSequence( ) );

	return _root.get( );


}



bool		Transhipment::setTranshipmentFileName( const std::string &fn )
{
	
	_filename = fn;

	if( _TranDetailsLoader( ) )		
	{
		_pickUpRandomColors( );
		_computeQuadraticBezier( );
		return true;
	}


	return false;

}



void			Transhipment::_pickUpRandomColors( )
{
	unsigned int n = 256;		// ( 0 to 255 )

	_tranColors = new osg::Vec4Array;

	srand( static_cast< unsigned int > ( time( NULL ) ) );

	if( !_tranDetails.empty( ) )
	{
		for( unsigned int i = 0; i < _tranDetails.size(); ++i )		// every transhipment assigned one color
		{
			unsigned int red	= static_cast< unsigned int > ( ( double ( rand( ) ) / RAND_MAX ) * n );
			unsigned int green	= static_cast< unsigned int > ( ( double ( rand( ) ) / RAND_MAX ) * n );
			unsigned int blue	= static_cast< unsigned int > ( ( double ( rand( ) ) / RAND_MAX ) * n );

			_tranColors->push_back( osg::Vec4( (double)red/255.0, (double)green/255.0, (double)blue/255.0, 1.0f ) );
		}

	}

}


unsigned int	Transhipment::getStepOfTranshipments( const unsigned int n )
{

	if ( n == 0 ) 
		return _tranDetails[n].tranStep;
	else
		return _tranDetails[_tranSteps[n-1]].tranStep;

}

osg::Switch*	Transhipment::createTranshipmentSwitch( )
{

	_tranSwitch = new osg::Switch;



	return _tranSwitch.get( );

}



osg::Sequence*	Transhipment::createTranshipmentSequence( )
{

	// numbers and curves
	_tranSequence = new osg::Sequence;
	
	_tranSequence->addChild( new osg::Geode );
	_tranSequence->setTime( _tranSequence->getNumChildren()-1, _sTime );		// step 0, waiting

  unsigned int counter = 0;
	if( !_tranDetails.empty() )
	{
		unsigned int sId ( 0 );
		unsigned int eId ( 0 );
		unsigned int count ( 0 );
		unsigned int preStep ( 0 );

		_tranSequence->addChild( new osg::Geode );

		for( unsigned int s = 0; s < _tranSteps.size(); ++s )
		{
			if( s > 0 )		sId += _tranSteps[s-1];
			eId = sId + _tranSteps[s];

			count = _tranDetails[sId].tranStep - preStep; // + s;			// after every transhipment there is an extra waiting time
			preStep = _tranDetails[sId].tranStep;

			_tranSequence->setTime( _tranSequence->getNumChildren()-1, _seqTime*(count-1) );
			
			for( unsigned int j = 0; j < _numBezierPoints; ++j )
			{
        osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
        geode = _createTranshipmentDetailsbyStep( j, sId, eId );
				_tranSequence->addChild( geode.get() );
        #if 1 // agent
        {
          std::string stepValue = "";
          if( counter < 10 )
            stepValue = Usul::Strings::format( "00", counter );
          if( counter < 100 && counter >= 10 )
            stepValue = Usul::Strings::format( "0", counter );
          if( counter >= 100 )
            stepValue = Usul::Strings::format( counter );

          std::string path = "C:/data/Santanam/output/trans/";
          osg::ref_ptr< osg::Group > group ( new osg::Group );
          std::string filename = Usul::Strings::format( path, "trans", stepValue, ".osg" );
          group->addChild( geode.get() );
          osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
          counter ++;
        }
        #endif
				_tranSequence->setTime( _tranSequence->getNumChildren()-1, _seqTime );
			}

			_tranSequence->addChild( new osg::Geode );
        #if 0 // agent
        {
          std::string path = "C:/data/Santanam/output/trans/";
          osg::ref_ptr< osg::Group > group ( new osg::Group );
          std::string filename = Usul::Strings::format( path, "trans", counter, ".osg" );
          group->addChild( new osg::Geode );
          osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
          counter ++;
        }
        #endif
			_tranSequence->setTime( _tranSequence->getNumChildren()-1, _seqTime );	// extra time

		}	

		_tranSequence->addChild( new osg::Geode );
 		_tranSequence->setTime( _tranSequence->getNumChildren()-1, _seqTime*(_numOfSteps - _tranDetails[_tranDetails.size()-1].tranStep + _tranSteps.size()) );

	}

	_tranSequence->setInterval( osg::Sequence::LOOP, 0, -1 );
	_tranSequence->setDuration( 1.0f, -1 );
	_tranSequence->setMode( osg::Sequence::START );


	return _tranSequence.get( );

}



osg::Geode*		Transhipment::_createTranshipmentDetailsbyStep( unsigned int step, unsigned int sId, unsigned int eId )
{

	float delta = 1.0f;
	float characterSize = 10.0f;
	float linewidth = 2.0f;
	osg::Vec3 v;

	
	osg::ref_ptr< osg::Geode >	geode ( new osg::Geode );

	for( unsigned int i = sId; i < eId; ++i )		// for every transhipment
	{
		osg::ref_ptr< osg::Vec3Array >  vertices ( new osg::Vec3Array );
		osg::ref_ptr< osg::Vec4Array >  colors ( new osg::Vec4Array );

		osg::Vec4	color = _tranColors->at( i );

		colors->push_back( _tranColors->at( i ) );
		
		for( unsigned int j = 0; j <= step; ++j )
		{
			v = _BezierPoints.at( i * _numBezierPoints + j );
			vertices->push_back( osg::Vec3( v.x(), _areaSizeY - v.y(), v.z() ) );
		}

		// add vertices (lines)
		geode->addDrawable( _drawLines( vertices.release(), colors.release(), osg::PrimitiveSet::LINE_STRIP, linewidth ) );
		// add numbers (count)
		geode->addDrawable( _createText( osg::Vec3( v.x(), _areaSizeY - v.y(), v.z() ), color, characterSize, _tranDetails[i].stockType + _intToString( _tranDetails[i].quantityST ) ) );

		if( _fromCenter )
		{
			float radius = 5.0f;
			float zValue = 200.0f;
			osg::Vec3	center( _areaSizeX/2.0, _areaSizeY/2.0, zValue );
			osg::Vec4	color( 0.40f, 0.40f, 0.40f, 1.0f );

			geode->addDrawable( _createShapeOfSphere( center, radius, color ) );
		}
	
	}

	return geode.release();

}



osg::Vec3		Transhipment::_pointsOnQuadraticBezier( osg::Vec3 *cp, const float t )
{
	// B(t) = (1-t)*(1-t)*P0 + 2*t*(1-t)*P1 + t*t*P2
	
	float	Cx, Cy, Cz;
	float	tSquared = t * t;

	Cx = 1 - 2*t + tSquared;
	Cy = 2*t - 2*tSquared;
	Cz = tSquared;

	return ( cp[0] * Cx + cp[1] * Cy + cp[2] * Cz );

}


osg::Vec3		Transhipment::_findMiddleQuadraticBezierCP( const osg::Vec3 &v1, const osg::Vec3 &v2 )
{

	float t = 0.5;
	osg::Vec3 vc ( v1*t + v2*(1-t) );		// center

	osg::Vec3 v3 ( v2 - v1 );
	v3.normalize( );

	osg::Vec3 v4 ( -v3.y(),	v3.x(),  v3.z() );		// perpendicular angle
	osg::Vec3 v5 ( v3.y(),	-v3.x(), v3.z() );

	vc.set( vc.x(), vc.y(), 1.0f );

	// float length = ( osg::Vec3( v2 - v1 ) ).length( );    // different length
	float length = 100.0;		// fixed length

	osg::Vec3 v6 ( vc + v4 * length );		// may need change the length

    return osg::Vec3( v6.x(), v6.y(), length );		// length put to the height ( slope )

	// return osg::Vec3( vc.x(), vc.y(), length );		// length put to the height ( straight )

}



void			Transhipment::_computeQuadraticBezier( )
{

	// if( !_tranDetails.empty() )
	_numBezierPoints = 20;
	_BezierPoints.reserve( _numBezierPoints * _tranDetails.size() );		// total number of Bezier points

	float	dt = 1.0 / ( _numBezierPoints - 1 );
	float	zValue = 200.0f;
	float   deltaX = 50.0f;


	for( unsigned int n = 0; n < _tranDetails.size(); ++n )
	{
		osg::Vec3	cp[3];
		
		int		fromHos = _tranDetails[n].fromHospID;
		int		toHos   = _tranDetails[n].toHospID;

		if( fromHos != -1 ) 
		{
			cp[0]	=  _hospitals[fromHos];
			cp[1]	=  _findMiddleQuadraticBezierCP( _hospitals[fromHos], _hospitals[toHos] );
			cp[2]	=  _hospitals[toHos];

			for( unsigned int i = 0; i < _numBezierPoints; ++i )
				_BezierPoints.push_back( _pointsOnQuadraticBezier( cp, i*dt ) );

		}
		else	// fromHos == -1		// stock from somewhere		// may need different way to calculate cp[1]
		{
			_fromCenter = true;

			cp[0]	=  osg::Vec3( _areaSizeX/2.0, _areaSizeY/2.0, zValue );   // from somewhere if "fromHos = -1"
			cp[2]	=  _hospitals[toHos];
			cp[1]   = ( cp[0] + cp[2] ) / 2.0;

			if( cp[0].x() >= _areaSizeX/2.0 )	cp[1].x() -= deltaX;		// curve
			else								cp[1].x() += deltaX;

			for( unsigned int i = 0; i < _numBezierPoints; ++i )
				_BezierPoints.push_back( _pointsOnQuadraticBezier( cp, i*dt ) );

		}


	}// end for

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// loader transhipments data from input file
// Format for Transhipment.dat ( Java binary file, big Endian ):
//		Step ( int ), Stock type ( char ), From hospital ID ( int ), To hospital ID ( int ), 
//		Quantity of stock transfered ( int )
//		... ... ( Number of transhipments )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool		Transhipment::_TranDetailsLoader ( )
{

	std::cout << "Transhipment filename: " << _filename << std::endl;

	const Usul::Types::Uint64 fileSize ( Usul::File::size ( _filename ) );

	std::ifstream infile( _filename.c_str(), std::ios::in | std::ios::binary );
	
	if ( !infile )
	{
		std::cerr << "Error: unable to open Transhipments file!\n";
		return false;
	}

	// in Java binary, 1 size of "char" == 2 * size of "char" in C / C++
	const Usul::Types::Uint64 rowBytes ( 4 * sizeof ( int ) + 2*sizeof ( char ) );
	const Usul::Types::Uint64 numTranshipments ( fileSize / rowBytes );
	USUL_ASSERT ( fileSize == numTranshipments * rowBytes );

	int				tValue ( 0 );
	char			type ( ' ' );

	unsigned int	numSteps ( 0 );
	unsigned int	count ( 0 );

	// _tranDetails.reserve( numTranshipments );		// here we donot know the exact numTranshipments ( zero stocks )
	TranDetail tmp;

	for( Usul::Types::Uint64 i = 0; i < numTranshipments; ++i )
	{

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.tranStep = static_cast< unsigned int > ( ::abs ( tValue ) );
		
		infile.read( ( char * ) &type, sizeof( char ) );		// infile.get(type);
		infile.read( ( char * ) &type, sizeof( char ) );
		tmp.stockType = type;

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.fromHospID = static_cast< unsigned int > ( ::abs ( tValue ) );

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.toHospID = static_cast< unsigned int > ( ::abs ( tValue ) );

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.quantityST = static_cast< unsigned int > ( ::abs ( tValue ) );

		if( tmp.quantityST > 0 )		// if "0", do not need draw transhipments
		{

			std::cout << "_tranDetails size: " << _tranDetails.size() << std::endl;

			// put number of transhipmentSteps, a slightly different since the number of transhipments not continuous
			if ( count == 0 )
			{
				++count;
			}
			else if ( tmp.tranStep == _tranDetails[_tranDetails.size() - 1].tranStep )
			{
				++count;
			}
			else
			{
				_tranSteps.push_back( count );

				++numSteps;
				count = 1;
			}
#if debug
			std::cout << "transhipment: " << tmp.tranStep << " " << tmp.stockType << " " << tmp.fromHospID << " " << tmp.toHospID << " " << tmp.quantityST << std::endl;
#endif
			_tranDetails.push_back(tmp);

		}

	}

	_tranSteps.push_back( count );

#if debug
	std::cout << "transhipment total steps: " << _tranSteps.size() << std::endl;
		for( unsigned int w = 0; w < _tranSteps.size(); ++w )
			std::cout << "transhipment steps: " << w << " " << _tranSteps[w] << std::endl;
#endif

	infile.close();

	return true;


	
}


void Transhipment::hide ()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0x0 );
}

void Transhipment::show()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0xff );
}

bool Transhipment::shown () const
{
	return _root.valid () ? 0x0 != _root->getNodeMask() : false;
}


void Transhipment::resume ()
{
	
	osg::Sequence::SequenceMode mode = _tranSequence->getMode();

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
    
	_tranSequence->setMode( mode );

}


void Transhipment::startOver ()
{
	osg::Sequence::SequenceMode mode = osg::Sequence::START;

	_tranSequence->setMode( mode );

}

////////////// Utility


osg::ShapeDrawable*		Transhipment::_createShapeOfSphere( const osg::Vec3 &center, const float &radius, const osg::Vec4 &color )
{

	osg::ref_ptr< osg::ShapeDrawable >		sphere ( new osg::ShapeDrawable );
	// sphere->setUseDisplayList ( false );

	osg::ref_ptr< osg::TessellationHints >	hints ( new osg::TessellationHints );
	hints->setDetailRatio( 2.0f );
	
	sphere = new osg::ShapeDrawable( new osg::Sphere( center, radius ), hints.get() );
	sphere->setColor( color );


	return sphere.release();

}


const std::string	Transhipment::_intToString(const int &num)
{
	std::stringstream ss;

	ss << num;
	
	return ss.str();
}


osgText::Text*		Transhipment::_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str)
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



osg::Geode*		Transhipment::_createTextAsNode( const osg::Vec3 &pos, const osg::Vec4 &layoutColor, const float &characterSize, const std::string &str )
{
	osg::ref_ptr< osg::Geode > geode (new osg::Geode );

	geode->addDrawable ( _createText( pos, layoutColor, characterSize, str ) );

	return geode.release();

}




// Draw lines
osg::Geometry*	Transhipment::_drawLines( osg::Vec3Array* vertices, osg::Vec4Array* colors, const osg::PrimitiveSet::Mode& mode, const float& width )
{
	osg::ref_ptr< osg::Geometry > linesGeom ( new osg::Geometry );


		// pass the created vertex array to the points geometry object.
		linesGeom->setVertexArray( vertices );
        		
		linesGeom->setColorArray( colors );
		
		if ( colors->size() == 1 )		// only one color
			linesGeom->setColorBinding( osg::Geometry::BIND_OVERALL );
		else
			linesGeom->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );	// BIND_PER_VERTEX || BIND_PER_PRIMITIVE_SET
        

		// set the normal in the same way color.
		osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
		normals->push_back( osg::Vec3( 0.0f, 0.0f, -1.0f ) );
		linesGeom->setNormalArray( normals.get() );
		linesGeom->setNormalBinding( osg::Geometry::BIND_OVERALL );


		linesGeom->addPrimitiveSet( new osg::DrawArrays( mode,0,vertices->size() ) );

		// Turn of lighting for line and set line width.
		linesGeom->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		
		osg::ref_ptr< osg::LineWidth > linewidth ( new osg::LineWidth() );
		linewidth->setWidth( width );
		
		linesGeom->getOrCreateStateSet()->setAttributeAndModes( linewidth.get(), osg::StateAttribute::ON );


	return linesGeom.release();

}
