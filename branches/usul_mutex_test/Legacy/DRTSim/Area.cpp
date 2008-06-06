/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "Usul/Endian/Endian.h"
#include "Usul/File/Stats.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Format.h"

#include "Area.h"

#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/ShapeDrawable>
#include "osgDB/WriteFile"

#include <sstream>


#define debug 1



// openscenegraph stuff
osg::Geode*	Area::buildAreaScene ( )
{
	
	osg::ref_ptr< osg::Geode > geode ( new osg::Geode );


	if ( !_areaDetails.empty( ) )
	{

		float zValue = 0.0f;		// divided into different layers
		float delta  = 5.0f;

		
	/*	// add text ( the origin (0,0) )
		{
			osg::Vec4 originColor ( 0.8f, 0.5f, 0.5f, 1.0f );
			float textSize		= 20.0f;
			std::string text	= "(0,0)";

			// add text ( the origin (0,0) )
			geode->addDrawable ( _util.createText( osg::Vec3( -2.0*delta, _areaSizeY+delta, zValue ), originColor, textSize, text ) );
	
		}	*/


		// add text ( hospital grids )
		{
			const std::string str = "G";
			zValue = 2.0f;

			const float xUnit = _areaSizeX / _areaCols;
			const float yUnit = _areaSizeY / _areaRows;

			const float characterSize = 20.0f;
			osg::Vec4 color ( 155.0/255.0f, 155.0/255.0f, 155.0/255.0f, 1.0f );

			for( unsigned int i = 0; i < _areaRows; ++i )
				for( unsigned int j = 0; j< _areaCols; ++j )
				{
					std::string text = str + std::string( _intToString ( i * _areaCols + j ) );
					geode->addDrawable ( _createText( osg::Vec3( j*xUnit+delta, _areaSizeY-(yUnit+i*yUnit-delta), zValue ), color, characterSize, text ) );
				}

		}


		// add text ( hospital ID )
		{
			zValue = 3.0f;
			const std::string str = "H";
			const float cf = 2.5f;	// coefficient

			const float characterSize = 10.0f;
			const osg::Vec4 hColor ( 1.0f, 1.0f, 204.0/255.0f, 1.0f );

			for( unsigned int i = 0; i < _areaDetails.size(); ++i )
			{
				std::string text = str + std::string( _intToString( _areaDetails[i].hospitalID ) );
				osg::Vec3 pos ( _areaDetails[i].hospitalX-delta, _areaSizeY-_areaDetails[i].hospitalY-cf*delta, zValue );
				geode->addDrawable ( _createText( pos, hColor, characterSize, text ) );
			}

		}

		// draw area frame
		{
			const float zValue		= 0.0f;
			const float width		= 5.0f;

			osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
				vertices->push_back( osg::Vec3( 0.0,		0.0,		zValue ) );
				vertices->push_back( osg::Vec3( _areaSizeX,	0.0,		zValue ) );
				vertices->push_back( osg::Vec3( _areaSizeX,	_areaSizeY, zValue ) );
				vertices->push_back( osg::Vec3( 0.0,		_areaSizeY,	zValue ) );

	        
			osg::ref_ptr< osg::Vec4Array> colors ( new osg::Vec4Array );
				colors->push_back( osg::Vec4( 0.35f, 0.35f, 0.35f, 1.0f ) );

			geode->addDrawable( _drawLines( vertices.get(), colors.get(), osg::PrimitiveSet::QUADS, width ) );

			osg::ref_ptr< osg::Vec4Array> ecolors ( new osg::Vec4Array );
				ecolors->push_back( osg::Vec4( 0.5f, 0.5f, 0.5f, 1.0f ) );

			geode->addDrawable( _drawLines( vertices.get(), ecolors.get(), osg::PrimitiveSet::LINE_LOOP, width ) );

		}


		// draw area lines
		{

			osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

			float zValue = 0.5;
			float width  = 3.0f;
        
			for(unsigned int i = 0; i < (_areaRows - 1); ++i)
			{
				vertices->push_back( osg::Vec3( 0.0,		1.0*(i+1)*_areaSizeY/_areaRows, zValue ) );
				vertices->push_back( osg::Vec3( _areaSizeX, 1.0*(i+1)*_areaSizeY/_areaRows, zValue ) );
			}

			for(unsigned int j = 0; j < (_areaCols - 1); ++j)
			{
				vertices->push_back( osg::Vec3( 1.0*(j+1)*_areaSizeX/_areaCols, 0.0,		zValue ) );
				vertices->push_back( osg::Vec3( 1.0*(j+1)*_areaSizeX/_areaCols, _areaSizeY, zValue ) );
			}

		
			// set the colors as before, plus using the aobve
			osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
				colors->push_back( osg::Vec4( 0.45f, 0.45f, 0.45f, 1.0f ) );

			geode->addDrawable( _drawLines( vertices.get(), colors.get(), osg::PrimitiveSet::LINES, width ) );

		}


		// draw hospital dots ( points )
		{
			float zValue = 3.0f;
			float pSize  = 6.0f;

			osg::Vec4	color( 0.0f, 102.0/255.0f, 1.0f, 1.0f );
			float radius ( 3.0f );

			for(unsigned int n = 0; n < _areaDetails.size(); ++n)
			{
				osg::Vec3	center( _areaDetails[n].hospitalX, _areaSizeY - _areaDetails[n].hospitalY, zValue );
				geode->addDrawable( _createShapeOfSphere( center, radius, color ) );

			}
        
	/*		// display hospitals by GL_POINTS
			osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

				for(unsigned int n = 0; n < _areaDetails.size(); ++n)
					vertices->push_back( osg::Vec3( _areaDetails[n].hospitalX, _areaSizeY - _areaDetails[n].hospitalY, zValue ) );  // Top-left is the origin
					geode->addDrawable( new osg::Sphere( osg::Vec3( _areaDetails[n].hospitalX, _areaSizeY - _areaDetails[n].hospitalY, zValue ), 1.0f );
		
			osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
				colors->push_back( osg::Vec4( 0.7f, 0.7f, 0.7f, 1.0f ) );

			geode->addDrawable( _util.drawPoints( vertices.get(), colors.get(), pSize ) );
	*/	
		}


	}
#if 1 // agent
  {
    std::string path = "C:/data/Santanam/output/area/";
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    std::string filename = Usul::Strings::format( "area.osg" );
    group->addChild( geode.get() );
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
  }
#endif
	return geode.release();

}




// set area file name
bool	Area::setAreaFileName( const std::string &fn )
{
	_filename = fn;

	if( _AreaDetailsLoader( ) )		return true;	
	
	return false;

}



osg::ShapeDrawable*		Area::_createShapeOfSphere( const osg::Vec3 &center, const float &radius, const osg::Vec4 &color )
{

	osg::ref_ptr< osg::ShapeDrawable >		sphere ( new osg::ShapeDrawable );
	// sphere->setUseDisplayList ( false );

	osg::ref_ptr< osg::TessellationHints >	hints ( new osg::TessellationHints );
	hints->setDetailRatio( 2.0f );
	
	sphere = new osg::ShapeDrawable( new osg::Sphere( center, radius ), hints.get() );
	sphere->setColor( color );


	return sphere.release();

}


osg::MatrixTransform*		Area::_transformTheShapeDrawable( const osg::Vec3 &pos, osg::Geode *geode )
{
	osg::ref_ptr< osg::MatrixTransform >	transform ( new osg::MatrixTransform );

		// osg::Matrix		S = osg::Matrix::scale( scale );
		osg::Matrix		T = osg::Matrix::translate( pos );
		transform->setDataVariance ( osg::Object::STATIC );
		transform->setMatrix( T );	// ( S * T );

		transform->addChild ( geode );


	return transform.release();

}


const unsigned int		Area::getHospitalCoordinateX ( const unsigned int id )
{
	if( ( id >= 0 ) && ( id < _numHospitals ) )
		return _areaDetails[id].hospitalX;		

	return 0;

}


const unsigned int		Area::getHospitalCoordinateY ( const unsigned int id )
{
	if( ( id >= 0 ) && ( id < _numHospitals ) )
		return _areaDetails[id].hospitalY;

	return 0;

}



osg::Vec3		Area::getHospitalCoordinates( const unsigned int id )
{
	float zValue	= 1.0f;

	osg::Vec3	vertex;
	
	if( ( id >= 0 ) && ( id < _numHospitals ) )
		vertex.set( _areaDetails[id].hospitalX, _areaDetails[id].hospitalY, zValue );

	return vertex;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// loader area details data from input file
// Format for AreaDetail.dat ( Java binary file, big Endian ):
//		Area Xsize ( int ), Area Ysize ( int ), Number of rows ( int ), Number of columns ( int )
//		Hospital ID ( int ), Hospital's Grid ( int ), Hospital's X-coordinate ( int ), Hospital's Y-coordinate ( int )
//		... ... ( Number of hospitals )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool	Area::_AreaDetailsLoader ( )
{
	
	const unsigned int zoom = 3;
	
	std::cout << "Area filename: " << _filename << std::endl;

	const Usul::Types::Uint64 fileSize ( Usul::File::size ( _filename ) );
	std::cout << "Area file to load is " << _filename << std::endl;

	std::ifstream infile( _filename.c_str(), std::ios::in | std::ios::binary );
        
	
	if ( false == infile.is_open() )
	{
		std::cerr << "Error: unable to open AreaDetails file!\n";
		return false;
	}
	
	
	infile.read( ( char * ) &_areaSizeX, sizeof( int ) );		Usul::Endian::FromBigToSystem::convert ( _areaSizeX );
	infile.read( ( char * ) &_areaSizeY, sizeof( int ) );		Usul::Endian::FromBigToSystem::convert ( _areaSizeY );
	infile.read( ( char * ) &_areaRows,	 sizeof( int ) );		Usul::Endian::FromBigToSystem::convert ( _areaRows );
	infile.read( ( char * ) &_areaCols,	 sizeof( int ) );		Usul::Endian::FromBigToSystem::convert ( _areaCols );

	_areaSizeX = zoom * _areaSizeX;
	_areaSizeY = zoom * _areaSizeY;

#if debug
	char c = ' ';
	std::cout << _areaSizeX << c << _areaSizeY << c << _areaRows << c << _areaCols << std::endl;
#endif	
	
	
	const Usul::Types::Uint64 remaining ( fileSize - 4 * sizeof ( int ) );
	const Usul::Types::Uint64 rowBytes ( 4 * sizeof ( int ) );
	const Usul::Types::Uint64 numHospitals ( remaining / rowBytes );
	USUL_ASSERT ( remaining == numHospitals * rowBytes );

	_numHospitals = static_cast<unsigned int> ( numHospitals );

	int tValue ( 0 );
	AreaDetail tmp;

	_areaDetails.reserve( static_cast< unsigned int > ( numHospitals ) );

	
	for(Usul::Types::Uint64 i = 0; i < numHospitals; ++i)
	{
		
		infile.read( ( char * ) &tValue, sizeof( int ) );		
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.hospitalID = static_cast<unsigned int> ( ::abs ( tValue ) );	// in case we have a negative tValue
		
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.hospitalGrid = static_cast<unsigned int> ( ::abs ( tValue ) );

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.hospitalX = zoom * static_cast<unsigned int> ( ::abs ( tValue ) );

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.hospitalY = zoom * static_cast<unsigned int> ( ::abs ( tValue ) );

		_areaDetails.push_back(tmp);

#if debug
		std::cout << tmp.hospitalID << c << tmp.hospitalGrid << c << tmp.hospitalX << c << tmp.hospitalY << std::endl;
#endif

	}

	infile.close();
	
	return true;

}




////////////////////////////////

const std::string	Area::_intToString(const int &num)
{
	std::stringstream ss;

	ss << num;
	
	return ss.str();
}


osgText::Text*		Area::_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str)
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



// Draw lines
osg::Geometry*	Area::_drawLines( osg::Vec3Array* vertices, osg::Vec4Array* colors, const osg::PrimitiveSet::Mode& mode, const float& width )
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

