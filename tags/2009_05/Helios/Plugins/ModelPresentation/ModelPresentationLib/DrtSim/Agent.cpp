
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Wei Chen, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <ctime>

#include "Usul/Endian/Endian.h"
#include "Usul/File/Stats.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/File/Path.h"

#include "Agent.h"

#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
#include "osgDB/WriteFile"

#include <sstream>


#define debug 1



osg::Group*		Agent::buildAgentScene ( )
{
	_root =  new osg::Group ;

	// _root->addChild( _util.createTextAsNode( osg::Vec3( _areaSizeX, _areaSizeY+30.0f, 0.0f), osg::Vec4( 1.5f, 0.5f, 0.5f, 1.0f ), 25.0f, "Agent" ) ); 
  #if 1 // agent
  {

    std::string path = _workingDir;
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    std::string filename = Usul::Strings::format( path, "/agentText.ive" );
    group->addChild( _createTextAsNode( osg::Vec3( 0.0f, -30.0f, 0.0f), osg::Vec4( 1.0f, 0.5f, 0.5f, 1.0f ), 20.0f, " Agent " ) );
    
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
    _root->addChild( group.get() ); 
  }
#endif

	_root->addChild( createAgentSequence ( ) );

	return _root.get();
}



bool			Agent::setAgentFileName ( const std::string &fn )
{
	_filename = fn;

	if( _AgentDetailsLoader( ) )		return true;
	
	return false;

}


osg::Geode*		Agent::createAgentColorCodeBar ( )
{

	osg::ref_ptr< osg::Geode > geode ( new osg::Geode );


	float characterSize = 26.0f;
	float linewidth		= 5.0f;
	float delta			= 80.0f;
	float zValue		= 0.0f;

	float cf0			= 1.2f;
	float cf1			= 3.0f;
	float cf2			= 0.7f;

	unsigned int	numRectangles = 6;
	unsigned int	layer		  = 0;

	osg::Vec3	rectangleDelta ( delta, delta/2.0, zValue );


	std::vector< osg::Vec3 >	rectPos  ( numRectangles );
	std::vector< osg::Vec3 >	colorPos ( numRectangles );

	for( unsigned i = 0; i < numRectangles; ++i )
	{
		rectPos[i].set(  osg::Vec3( _winMargin, _winHeight - cf1*_winMargin - cf2*layer*delta, zValue ) );
		colorPos[i].set( osg::Vec3( _winMargin+cf0*delta, _winHeight - cf1*_winMargin - cf2*layer*delta + delta/4.0f, zValue ) );
		
		layer++;
	}

	// add Agent color code bar
	geode->addDrawable( _createFilledRectangle( rectPos[0], rectangleDelta, _suceptibleAgentColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[0], _suceptibleAgentColor, characterSize, "Suceptible Agent"));

	geode->addDrawable( _createFilledRectangle( rectPos[1], rectangleDelta, _exposedAgentColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[1], _exposedAgentColor, characterSize, "Exposed Agent"));

	geode->addDrawable( _createFilledRectangle( rectPos[2], rectangleDelta, _infectedAgentColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[2], _infectedAgentColor, characterSize, "Infected Agent"));

	geode->addDrawable( _createFilledRectangle( rectPos[3], rectangleDelta, _recoveredAgentColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[3], _recoveredAgentColor, characterSize, "Recovered Agent"));

	geode->addDrawable( _createFilledRectangle( rectPos[4], rectangleDelta, _vaccinatedAgentColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[4], _vaccinatedAgentColor, characterSize, "Vaccinated Agent"));

	geode->addDrawable( _createFilledRectangle( rectPos[5], rectangleDelta, _deadAgentColor, linewidth ) );
	geode->addDrawable( _createText( colorPos[5], _deadAgentColor, characterSize, "Dead Agent"));


	return geode.release();

}


// Create Agent Steps Switch (  to display the current ( agent ) step )
//	Following information is supposed to be displayed in HUDtext
osg::Switch*	Agent::createAgentStepSwitch ( )
{
	float zValue = 0.0f;
	float characterSize = 26.0f;

	osg::Vec3 pos ( _winMargin, _winHeight - _winMargin, zValue );
	osg::Vec4 layoutColor ( 0.8f, 0.6f, 0.4f, 1.0f );

	_agentStepSwitch = new osg::Switch;

	if ( !_agentDetails.empty() )
	{
		for( unsigned int i = 0; i < _agentSteps.size(); ++i )
		{
			std::string		str = " Current Step = " + _intToString( i ) + " of " + _intToString(_agentSteps.size());

			_agentStepSwitch->addChild( _createTextAsNode( pos, layoutColor, characterSize, str ) );

		}
	}

	_agentStepSwitch->setSingleChildOn ( 0 );


	return _agentStepSwitch.get();

}



// Create Agent Switch ( dots every step )
osg::Switch*	Agent::createAgentSwitch ( )
{
	//
	//_agentSwitch = new osg::Switch;

	//if( !_agentDetails.empty() )
	//{ 
 //   unsigned int count = 0;
 //   unsigned int counter = 0;
	//	for( unsigned int i = 0; i < _agentSteps.size(); ++i )
 //   {
	//		_agentSwitch->addChild( _createAgentDetailsbyStep( i, count, counter ) );
 //     counter ++;
 //   }

	//	_agentSwitch->setSingleChildOn( 0 );

	//}


	return _agentSwitch.get();

}



// Agent Sequence
// Actual HUD text creation
osg::Sequence*		Agent::createAgentStepSequence ()
{

	float zValue = 0.0f;
	float characterSize = 26.0f;

	osg::Vec3 pos ( _winMargin, _winHeight-_winMargin, zValue );
	osg::Vec4 layoutColor ( 0.8f, 0.6f, 0.4f, 1.0f );
	
	_agentStepSequence = new osg::Sequence;

	_agentStepSequence->addChild( new osg::Geode );
	_agentStepSequence->setTime( _agentStepSequence->getNumChildren()-1, _sTime );
  
	if ( !_agentDetails.empty() )
	{
		unsigned int count = 0;
    unsigned int counter = 0;
    unsigned int index = 0;
		for( unsigned int i = 0; i < _agentSteps.size(); ++i )
		{
			std::string	str = "Current Step = " + _intToString( i + 1 );
      osg::ref_ptr< osg::Group > group ( new osg::Group );
      group->addChild( _createTextAsNode( pos, layoutColor, characterSize, str ) );
			_agentStepSequence->addChild( group.get() );
			
			if( ( count < _numTranSteps ) && ( i == (_transhipment[count]-1) ) )
			{
        
				_agentStepSequence->setTime( _agentStepSequence->getNumChildren()-1, _seqTime*_numTranMovements );
				count++;
			}
			else
			{
				_agentStepSequence->setTime( _agentStepSequence->getNumChildren()-1, _seqTime );
			}
      // dump osg file
      std::string path = _workingDir;
#if 1
      {
        osg::ref_ptr< osg::Camera > camera ( new osg::Camera );

        camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        camera->setProjectionMatrixAsOrtho2D( 0, 1280, 0, 1024 );
        camera->setViewMatrix( osg::Matrix::identity( ) );
        camera->setClearMask( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
        camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
        camera->addChild( group.get() );
        if( index < _transhipment.size() && i == ( _transhipment[ index ] - 1 ) )
        {
          for( unsigned int x = 0; x < _numTranMovements; x ++ )
          {
            
            std::string stepValue = "";
            if( counter < 10 )
              stepValue = Usul::Strings::format( "00", counter );
            if( counter < 100 && counter >= 10 )
              stepValue = Usul::Strings::format( "0", counter );
            if( counter >= 100 )
              stepValue = Usul::Strings::format( counter );

            std::string filename = Usul::Strings::format( "step", stepValue );
            std::string fullpath = Usul::Strings::format( path, "/", filename, ".ive" ); 
            osgDB::writeNodeFile( *( camera.get() ), fullpath.c_str() );
            _mpdWriter->addModel( filename, fullpath );
            std::vector< std::string > modelList;
            modelList.push_back( filename );
            _mpdWriter->addModelsToTimeSet( modelList, "Steps", counter, counter + 1 );
            counter ++;
          }
          index = count;
        }
        else
        {
          std::string stepValue = "";
            if( counter < 10 )
              stepValue = Usul::Strings::format( "00", counter );
            if( counter < 100 && counter >= 10 )
              stepValue = Usul::Strings::format( "0", counter );
            if( counter >= 100 )
              stepValue = Usul::Strings::format( counter );
        
            std::string filename = Usul::Strings::format( "step", stepValue );
            std::string fullpath = Usul::Strings::format( path, "/", filename, ".ive" ); 
            osgDB::writeNodeFile( *( camera.get() ), fullpath.c_str() );
            _mpdWriter->addModel( filename, fullpath );
            std::vector< std::string > modelList;
            modelList.push_back( filename );
            _mpdWriter->addModelsToTimeSet( modelList, "Steps", counter, counter + 1 );
            counter ++;
        }
      }
#endif
      
		}

		// loop through all children
		_agentStepSequence->setInterval( osg::Sequence::LOOP, 0, -1 );

		// real-time playback, repeat indefinitively
		_agentStepSequence->setDuration( 1.0f, -1 );

		// must be started explicitly
		_agentStepSequence->setMode( osg::Sequence::START );

	}


	return _agentStepSequence.get();

}



osg::Sequence*		Agent::createAgentSequence ( )
{
	
	_agentSequence = new osg::Sequence;
	
	_agentSequence->addChild( new osg::Geode );
	_agentSequence->setTime( _agentSequence->getNumChildren()-1, _sTime );


	if( !_agentDetails.empty() )
	{
		unsigned int count = 0;
    unsigned int counter = 0;
		for( unsigned int i = 0; i < _agentSteps.size(); ++i )
		{
			_agentSequence->addChild( _createAgentDetailsbyStep( i, count, counter ) );

			if( ( count < _numTranSteps ) && ( i == (_transhipment[count]-1) ) )
			{
				_agentSequence->setTime( _agentSequence->getNumChildren()-1, _seqTime*_numTranMovements );
				count++;
			}
			else
			{
				_agentSequence->setTime( _agentSequence->getNumChildren()-1, _seqTime );
			}
      //counter ++;
		}

		// loop through all children
		_agentSequence->setInterval( osg::Sequence::LOOP, 0, -1 );

		// real-time playback, repeat indefinitively
		_agentSequence->setDuration( 1.0f, -1 );

		// must be started explicitly
		_agentSequence->setMode( osg::Sequence::START );

	}


	return _agentSequence.get();

}





// Create Agent information by each step ( to display all dots )
osg::Geode*		Agent::_createAgentDetailsbyStep( const unsigned int &step, unsigned int &tCount, unsigned int &counter )
{

	osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

	if( _agentDetails.empty() || _agentSteps.empty() )		return geode.release();

	unsigned int	startIndex = 0;
	for(unsigned int i = 0; i < step; ++i)		startIndex += _agentSteps[i];

	osg::ref_ptr< osg::Vec3Array >	vertices ( new osg::Vec3Array );
	osg::ref_ptr< osg::Vec4Array >	colors ( new osg::Vec4Array );

	// if want add Agent ID as text, put code here
	// .......................

	const float zValue  = 5.0f;		// different layers for different display ( agent + stock + transhipment + spores )
	const float pSize	= 2.0f;

	for( unsigned int n = startIndex; n < startIndex + _agentSteps[step]; ++n )
	{
		// trick here is, as requested, the top-left is set to origin ( 0,0 )
		vertices->push_back( osg::Vec3( _agentDetails[n].agentX, _areaSizeY - _agentDetails[n].agentY, zValue ) );

		switch( _agentDetails[n].diseaseStage )
		{
			case 'S':
			case 's':
				colors->push_back ( _suceptibleAgentColor );
				break;

			case 'E':
			case 'e':
				colors->push_back ( _exposedAgentColor );
				break;

			case 'I':
			case 'i':
				colors->push_back ( _infectedAgentColor );
				break;

			case 'R':
			case 'r':
				colors->push_back ( _recoveredAgentColor );
				break;

			case 'V':
			case 'v':
				colors->push_back ( _vaccinatedAgentColor );
				break;

			case 'D':
			case 'd':
				colors->push_back ( _deadAgentColor );
				break;

			default:
				break;
		}

	}

	geode->addDrawable( _drawPoints( vertices.get(), colors.get(), pSize ) );

  // export the agent scene for this step
  
#if 0 // agent
  {
    std::string stepValue = "";
    if( step < 10 )
      stepValue = Usul::Strings::format( "00", step );
    if( step < 100 && step >= 10 )
      stepValue = Usul::Strings::format( "0", step );
    if( step >= 100 )
      stepValue = Usul::Strings::format( step );

    std::string path = "C:/data/Santanam/output/agent/";
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    std::string filename = Usul::Strings::format( path, "agent", stepValue, ".ive" );
    group->addChild( geode.get() );
    osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
  }
#endif
#if 1
   {
    std::string path = _workingDir;
    if( tCount < _transhipment.size() && step == ( _transhipment[tCount]-1 ) )
    {
      for( unsigned int x = 0; x < _numTranMovements; x ++ )
      {
        
        std::string stepValue = "";
        if( counter < 10 )
          stepValue = Usul::Strings::format( "00", counter );
        if( counter < 100 && counter >= 10 )
          stepValue = Usul::Strings::format( "0", counter );
        if( counter >= 100 )
          stepValue = Usul::Strings::format( counter );

        std::string filename = Usul::Strings::format( "agent", stepValue );
        std::string fullpath = Usul::Strings::format( path, "/", filename, ".ive" ); 
        _mpdWriter->addModel( filename, fullpath );
        std::vector< std::string > modelList;
        modelList.push_back( filename );
        _mpdWriter->addModelsToTimeSet( modelList, "Agents", counter, counter + 1 );
    
        osg::ref_ptr< osg::Group > group ( new osg::Group );
        group->addChild( geode.get() );
        osgDB::writeNodeFile( *( group.get() ), fullpath.c_str() );
        ++counter;
      }
    }
    else
    {
      std::string stepValue = "";
        if( counter < 10 )
          stepValue = Usul::Strings::format( "00", counter );
        if( counter < 100 && counter >= 10 )
          stepValue = Usul::Strings::format( "0", counter );
        if( counter >= 100 )
          stepValue = Usul::Strings::format( counter );
    
        std::string filename = Usul::Strings::format( "agent", stepValue );
        std::string fullpath = Usul::Strings::format( path, "/", filename, ".ive" ); 
        _mpdWriter->addModel( filename, fullpath );
        std::vector< std::string > modelList;
        modelList.push_back( filename );
        _mpdWriter->addModelsToTimeSet( modelList, "Agents", counter, counter + 1 );
        osg::ref_ptr< osg::Group > group ( new osg::Group );
        group->addChild( geode.get() );
        osgDB::writeNodeFile( *( group.get() ), fullpath.c_str() );
        ++counter;
    }
  }
#endif
	return geode.release();

}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// loader agent details data from input file
// Format for AgentDetail.dat ( Java binary file, big Endian ):
//		Step ( int ), Agent ID ( int ), Agent X-coord ( int ), Agent Y-coord ( int ), Agents's disease stage ( char )
//		... ... ( Number of agents )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Agent::_AgentDetailsLoader( )
{

	const unsigned int zoom = 3;

  Usul::Scope::CurrentDirectory cwd ( _workingDir );
	const Usul::Types::Uint64 fileSize ( Usul::File::size ( _filename ) );
	std::ifstream infile( _filename.c_str(), std::ios::in | std::ios::binary );

	if ( !infile )
	{
    std::cerr << "Error: unable to open AgentDetails file!" << std::endl;
		return false;
	}

	// in Java binary, 1 size of char == 2 size of char in C / C++
	const Usul::Types::Uint64 rowBytes ( 4 * sizeof ( int ) + 2*sizeof ( char ) );
	const Usul::Types::Uint64 numAgents ( fileSize / rowBytes );
	USUL_ASSERT ( fileSize == numAgents * rowBytes );
	
	int		tValue ( 0 );
	char	stage = ' ';

	int		numSteps	= 0;
	int		count		= 0;		// used to count how many agents in one step

#if 0
	clock_t		startTime		= ::clock();
	clock_t		updateDuration	= 1000;		// 1 second
#endif

	_agentDetails.reserve ( numAgents );

	AgentDetail tmp;

	for( Usul::Types::Uint64 i = 0; i < numAgents; ++i)
	{

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.agentStep = static_cast< unsigned int > ( ::abs ( tValue ) );   // in case we have negative value
		
		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.agentID = static_cast< unsigned int > ( ::abs ( tValue ) );

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.agentX = zoom * static_cast< unsigned int > ( ::abs ( tValue ) );

		infile.read( ( char * ) &tValue, sizeof( int ) );
		Usul::Endian::FromBigToSystem::convert ( tValue );
		tmp.agentY = zoom * static_cast< unsigned int > ( ::abs ( tValue ) );

		infile.read( ( char * ) &stage, sizeof( char ) );			// == infile.get(stage);
		infile.read( ( char * ) &stage, sizeof( char ) );
		tmp.diseaseStage = stage;

		_agentDetails.push_back(tmp);


		if( static_cast<int> ( tmp.agentStep ) == numSteps )				// steps of agent are continuous
		{
			++count;
		}
		else
		{
			_agentSteps.push_back(count);

			++numSteps;
			count = 1;
		}
	}

	_agentSteps.push_back(count);		// the last step

	infile.close();
	return true;
}


void Agent::hide ()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0x0 );
}

void Agent::show()
{
	if ( _root.valid () )
		_root->setNodeMask ( 0xff );
}

bool Agent::shown () const
{
	return _root.valid () ? 0x0 != _root->getNodeMask() : false;
}


void Agent::resume ()
{
	
	osg::Sequence::SequenceMode mode = _agentSequence->getMode();

    if ( mode == osg::Sequence::STOP ) 
	{
        mode = osg::Sequence::START;
        std::cerr << "Start" << std::endl;
    }
    else if ( mode == osg::Sequence::PAUSE ) 
	{
        mode = osg::Sequence::RESUME;
        std::cerr << "Resume" << std::endl;
    }
    else {
        mode = osg::Sequence::PAUSE;
        std::cerr << "Pause" << std::endl;
    }
    
	_agentSequence->setMode( mode );
	_agentStepSequence->setMode( mode );

}


void Agent::startOver ()
{
	osg::Sequence::SequenceMode mode = osg::Sequence::START;

	_agentSequence->setMode( mode );
	_agentStepSequence->setMode( mode );
}


////////////// Utility

const std::string	Agent::_intToString(const int &num)
{
	std::stringstream ss;

	ss << num;
	
	return ss.str();
}


osgText::Text*		Agent::_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str)
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



osg::Geode*		Agent::_createTextAsNode( const osg::Vec3 &pos, const osg::Vec4 &layoutColor, const float &characterSize, const std::string &str )
{
	osg::ref_ptr< osg::Geode > geode (new osg::Geode );

	geode->addDrawable ( _createText( pos, layoutColor, characterSize, str ) );

	return geode.release();

}


osg::Geometry*	Agent::_createFilledRectangle( const osg::Vec3& lowLeftPos, const osg::Vec3& delta, const osg::Vec4& color, const float& lineWidth)
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
osg::Geometry*	Agent::_drawPoints( osg::Vec3Array* vertices, osg::Vec4Array* colors, const float& pSize )
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
