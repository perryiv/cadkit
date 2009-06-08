/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <osg/ref_ptr>


#include "Usul/Endian/Endian.h"
#include "Usul/File/Stats.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Format.h"

#include "osgDB/WriteFile"

#include "Spore.h"


#define debug 1



bool		Spore::setSporeFileName( const std::string &fn )
{
	_filename = fn;

	if( _SporeDetailsLoader( ) )		return true;
	
	return false;

}



osg::Group*		Spore::buildSporeScene( )
{

	_root =  new osg::Group ;

	// _root->addChild( _createTextAsNode( osg::Vec3( 30.0f, _areaSizeY+30.0f, 0.0f), osg::Vec4( 1.5f, 0.5f, 0.5f, 1.0f ), 25.0f, "Spore" ) );
#if 1 // agent
  {
    std::string path = "C:/data/Santanam/output/text/";
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    std::string filename = Usul::Strings::format( path, "sporeText.osg" );
    group->addChild( _createTextAsNode( osg::Vec3( 400.0f, -30.0f, 0.0f), osg::Vec4( 1.0f, 0.5f, 0.5f, 1.0f ), 20.0f, " Spores " ) );
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
    _root->addChild( group.get() ); 
  }
#endif
  
	_root->addChild( createSporeSequence( ) );

	return _root.get();

}



osg::ShapeDrawable*		Spore::_createShapeOfCylinder( const osg::Vec3 &center, const float &radius, const osg::Vec4 &color )
{

	osg::ref_ptr< osg::ShapeDrawable >		sphere ( new osg::ShapeDrawable );
	// sphere->setUseDisplayList ( false );

	osg::ref_ptr< osg::TessellationHints >	hints ( new osg::TessellationHints );
	hints->setDetailRatio( 2.0f );
	
	sphere = new osg::ShapeDrawable( new osg::Sphere( center, radius ), hints.get() );
	sphere->setColor( color );

	return sphere.release();

}




osg::Switch*		Spore::createSporeSwitch( )
{

	_sporeSwitch = new osg::Switch;

	if( !_sporeDetails.empty( ) )
	{
		osg::ref_ptr< osg::Geode >	geode ( new osg::Geode );
		geode = _createSporeDetails( );

		for(unsigned int i = 0; i < _sporeDetails.size(); ++i )
		{
			if( i < _sporeStartingStep || i > _sporeEndingStep )
				_sporeSwitch->addChild( new osg::Geode );
			else
				_sporeSwitch->addChild( geode.get( ) );
		}

		_sporeSwitch->setSingleChildOn( 0 );
	
	}

	return _sporeSwitch.get( );

}



osg::Sequence*		Spore::createSporeSequence( )
{

	_sporeSequence = new osg::Sequence;

	_sporeSequence->addChild( new osg::Geode );
	_sporeSequence->setTime( _sporeSequence->getNumChildren()-1, _sTime );

	if( !_sporeDetails.empty( ) )
	{
		unsigned int count = 0;

		osg::ref_ptr< osg::Geode >	geode ( new osg::Geode );
		geode = _createSporeDetails( );

		for(unsigned int i = 0; i < _nSteps; ++i )
		{
			if ( i < _sporeStartingStep || i > _sporeEndingStep )
			{
				_sporeSequence->addChild ( new osg::Geode );
			}
			else	// ( _sporeStartingStep <= i <= _sporeEndingStep )
			{
				_sporeSequence->addChild ( geode.get( ) );
			}

			// if on transhipment step, wait longer
			if( ( count < _numTranSteps ) && ( i == (_transhipment[count]-1) ) )
			{
				_sporeSequence->setTime( _sporeSequence->getNumChildren()-1, _seqTime*_numTranMovements );
				count++;
			}
			else
			{
				_sporeSequence->setTime( _sporeSequence->getNumChildren()-1, _seqTime );
			}

		}

		_sporeSequence->setInterval( osg::Sequence::LOOP, 0, -1 );

		// real-time playback, repeat indefinitively
		_sporeSequence->setDuration( 1.0f, -1 );

		// must be started explicitly
		_sporeSequence->setMode( osg::Sequence::START );

	}


	return _sporeSequence.get( );

}



osg::Geode*		Spore::_createSporeDetails( )
{

	osg::ref_ptr< osg::Geode >		geode ( new osg::Geode );

	float zValue = 5.0f;
	float radius = 4.0f;
	unsigned int n = 256;		// ( 0 to 255 )

	srand( static_cast< unsigned int > ( time( NULL ) ) );

	if( !_sporeDetails.empty( ) )
	{
		osg::ref_ptr< osg::Vec3Array >	vertices ( new osg::Vec3Array );
		osg::ref_ptr< osg::Vec4Array >	colors ( new osg::Vec4Array );
		float	deltaX = 2.0f;
		float   deltaY = 4.0f;
		float   linewidth = 2.0f;

		for( unsigned int i = 0; i < _sporeDetails.size(); ++i )
		{
			// osg::Vec3	center( _sporeDetails[i].sporeX, _areaSizeY - _sporeDetails[i].sporeY, zValue );
			unsigned int red	= static_cast< unsigned int> ( ( double ( rand( ) ) / RAND_MAX ) * n );
			unsigned int green	= static_cast< unsigned int> ( ( double ( rand( ) ) / RAND_MAX ) * n );
			unsigned int blue	= static_cast< unsigned int> ( ( double ( rand( ) ) / RAND_MAX ) * n );
			// osg::Vec4	color( (double)red/255.0, (double)green/255.0, (double)blue/255.0, 1.0f );

			// geode->addDrawable( _createShapeOfSphere( center, radius, color ) );		// use sphere == spores

			vertices->push_back( osg::Vec3( _sporeDetails[i].sporeX,			_areaSizeY - ( _sporeDetails[i].sporeY + deltaY ),	zValue ) );
			vertices->push_back( osg::Vec3( _sporeDetails[i].sporeX - deltaX,	_areaSizeY - ( _sporeDetails[i].sporeY ),			zValue ) );
			vertices->push_back( osg::Vec3( _sporeDetails[i].sporeX,			_areaSizeY - ( _sporeDetails[i].sporeY - deltaY ),	zValue ) );
			vertices->push_back( osg::Vec3( _sporeDetails[i].sporeX + deltaX,	_areaSizeY - ( _sporeDetails[i].sporeY ),			zValue ) );

			colors->push_back( osg::Vec4( (double)red/255.0, (double)green/255.0, (double)blue/255.0, 1.0f ) );
		
		}

		geode->addDrawable( _drawLines( vertices.release(), colors.release(), osg::PrimitiveSet::QUADS, linewidth ) );

	}
			

	return geode.release( );

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// loader spore details data from input file
// Format for SporeDetails.dat ( Java binary file, big Endian ):
//		Spore starting step ( int ), Spore ending step ( int ), Spore X-coordinate ( int ), Spore Y-coordinate( int )
//		... ... ( Number of hospitals )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool	Spore::_SporeDetailsLoader( )
{
	
	const unsigned int zoom = 3;

	std::cout << "Spore filename: " << _filename << std::endl;
	
	const Usul::Types::Uint64 fileSize ( Usul::File::size ( _filename ) );

	std::ifstream infile( _filename.c_str(), std::ios::in | std::ios::binary );
	
	if ( !infile )
	{
		std::cerr << "Error: unable to open SporeDetails file!\n";
		return false;
	}
	
	
	infile.read( ( char * ) &_sporeStartingStep, sizeof( int ) );		Usul::Endian::FromBigToSystem::convert ( _sporeStartingStep );
	infile.read( ( char * ) &_sporeEndingStep,	 sizeof( int ) );		Usul::Endian::FromBigToSystem::convert ( _sporeEndingStep );
	
	
#if debug
	char c = ' ';
	std::cout << _sporeStartingStep << c << _sporeEndingStep << c << std::endl;
#endif	
	
	
	const Usul::Types::Uint64 remaining ( fileSize - 2 * sizeof ( int ) );
	const Usul::Types::Uint64 rowBytes  ( 2 * sizeof ( int ) );
	const Usul::Types::Uint64 numSpores ( remaining / rowBytes );
	//USUL_ASSERT ( remaining == numSpores * rowBytes );

	int tValue ( 0 );
	SporeDetail tmp;

	_sporeDetails.reserve( static_cast< unsigned int > ( numSpores ) );

	
	for(Usul::Types::Uint64 i = 0; i < numSpores; ++i )
	{
		
		infile.read( ( char * ) &tValue, sizeof( int ) );		
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.sporeX = zoom * static_cast<unsigned int> ( ::abs ( tValue ) );	// in case we have a negative tValue
		
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.sporeY = zoom * static_cast<unsigned int> ( ::abs ( tValue ) );


		_sporeDetails.push_back(tmp);

#if debug
		// std::cout << "spore " << i << ": " << tmp.sporeX << c << tmp.sporeY << std::endl;
#endif

	}

	infile.close();
	
	return true;

}



void Spore::hide ()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0x0 );
}

void Spore::show()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0xff );
}

bool Spore::shown () const
{
	return _root.valid () ? 0x0 != _root->getNodeMask() : false;
}


void Spore::resume ()
{
	
	osg::Sequence::SequenceMode mode = _sporeSequence->getMode();

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
    
	_sporeSequence->setMode( mode );

}


void Spore::startOver ()
{
	osg::Sequence::SequenceMode mode = osg::Sequence::START;

	_sporeSequence->setMode ( mode );

}


////////////// Utility

osgText::Text*		Spore::_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str)
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



osg::Geode*		Spore::_createTextAsNode( const osg::Vec3 &pos, const osg::Vec4 &layoutColor, const float &characterSize, const std::string &str )
{
	osg::ref_ptr< osg::Geode > geode (new osg::Geode );

	geode->addDrawable ( _createText( pos, layoutColor, characterSize, str ) );

	return geode.release();

}


// Draw lines
osg::Geometry*	Spore::_drawLines( osg::Vec3Array* vertices, osg::Vec4Array* colors, const osg::PrimitiveSet::Mode& mode, const float& width )
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


