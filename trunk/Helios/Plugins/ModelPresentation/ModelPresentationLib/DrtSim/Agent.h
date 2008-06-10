
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Wei Chen, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AgentDetails loader
// Load information which contains the agent ID (0, 1, 2 ...), location of agents in each step for each agent,
//									the disease stage of the agents (people).

// SERV projects -- Santanam Version 2.0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_CLASS_AGENT_H_
#define	_CLASS_AGENT_H_

#include "../ModelPresentationLib.h"

#include <osg/Node>
#include <osg/Switch>
#include <osg/Sequence>

#include <osgText/Text>
#include <osg/Geometry>
#include <osg/Vec3>

#include <string>
#include <vector>



class Agent
{
public:

	bool			setAgentFileName ( const std::string &fn );
	void			setAreaSizeX ( const unsigned int &areaX )	{ _areaSizeX = areaX; }
	void			setAreaSizeY ( const unsigned int &areaY )	{ _areaSizeY = areaY; }
	void			setWindowHeight ( const unsigned int &wh )	{ _winHeight = wh;	  }
	void			setWindowWidth  ( const unsigned int &ww )  { _winWidth  = ww;    }
	void			setWindowMargin ( const unsigned int &wm )	{ _winMargin = wm;    }

	void			setNumofTranshipmentSteps( const unsigned int &ns )	{ _numTranSteps = ns; }
	void			setNumofTranshipmentMovements( const unsigned int &nm ) { _numTranMovements = nm; }
	void			setTranshipmentReserve( )					{ _transhipment.reserve( _numTranSteps ); }
	void			setTranshipmentSteps( const unsigned int &n )		{ _transhipment.push_back( n ); }

  void      setWorkingDir( const std::string &dir ){ _workingDir = dir; }
  void      setWriter( ModelPresentationLib* writer ){ _mpdWriter = writer; }

	unsigned int	getNumOfAgentSteps ( )	{ return _agentSteps.size(); }
  unsigned int  getNumTranshipmentSteps() { return _numTranSteps * _numTranMovements; }
  unsigned int getTotalSteps() { return ( _agentSteps.size() + ( _numTranSteps * _numTranMovements ) ); }

	void	hide ();
	void	show ();
	bool	shown () const;

	void	resume ();
	void    startOver ();


	osg::Geode*		createAgentColorCodeBar ( );
	

	osg::Group*		buildAgentScene ( );
	osg::Switch*	createAgentStepSwitch ( ); // ( const osg::Vec3 &pos, const osg::Vec4 &layoutColor, const float &characterSize );
	osg::Switch*	createAgentSwitch ( );

	osg::Sequence*	createAgentStepSequence ( ); // ( const osg::Vec3 &pos, const osg::Vec4 &layoutColor, const float &characterSize );
	osg::Sequence*	createAgentSequence ( );


	Agent(void) :
		_seqTime				( 1.0f ),
		_sTime					( 12.0f ),
		_winHeight				( 1024 ),
		_winWidth				( 1280 ),
		_winMargin				( 50 ),
		_filename				( "" ),
		_areaSizeX				( 0 ),
		_areaSizeY				( 0 ),
		_numTranSteps			( 0 ),
		_numTranMovements		( 0 ),
		_suceptibleAgentColor	( 204.0/255.0f, 153.0/255.0f, 0.0f, 1.0f ),
		_exposedAgentColor		( 204.0/255.0f, 0.0f, 0.0f, 1.0f ),
		_infectedAgentColor		( 204.0/255.0f, 0.0f, 1.0f, 1.0f ),
		_recoveredAgentColor	( 51.0/255.0f, 255.0/255.0f, 51.0/255.0f, 1.0f ),
		_vaccinatedAgentColor	( 0.0f, 102/255.0f, 153.0/255.0f, 1.0f ),
		_deadAgentColor			( 0.0f, 0.0f, 0.0f, 1.0f ),
		_agentDetails			( 0 ),
		_agentSteps				( 0 ),
		_root					( 0 ),
		_agentStepSwitch		( 0 ),
		_agentSwitch			( 0 ),
		_agentStepSequence		( 0 ),
		_agentSequence			( 0 )
	{	
	}
	
	~Agent(void)	
	{
		_agentDetails.clear ( );
		_agentSteps.clear ( );
	}



private:

	bool			_AgentDetailsLoader ( );
	osg::Geode*		_createAgentDetailsbyStep ( const unsigned int& step, unsigned int &tCount, unsigned int &counter );

	const std::string	_intToString( const int& i );
	osgText::Text*	_createText( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geode*		_createTextAsNode( const osg::Vec3& pos, const osg::Vec4& layoutColor, const float& characterSize, const std::string& str );
	osg::Geometry*	_createFilledRectangle( const osg::Vec3& lowLeftPos, const osg::Vec3& delta, const osg::Vec4& color, const float& lineWidth );
	osg::Geometry*	_drawPoints( osg::Vec3Array* vertices, osg::Vec4Array* colors, const float& pSize );


	struct	AgentDetail	{
		unsigned int		agentStep;
		unsigned int		agentID;
		unsigned int		agentX;
		unsigned int		agentY;
		char				diseaseStage;
	};

	float			_seqTime;
	float			_sTime;
	unsigned int	_winHeight;
	unsigned int	_winWidth;
	unsigned int    _winMargin;
	std::string		_filename;
	unsigned int	_areaSizeX;
	unsigned int	_areaSizeY;

	unsigned int    _numTranSteps;
	unsigned int    _numTranMovements;

	osg::Vec4		_suceptibleAgentColor;	
	osg::Vec4		_exposedAgentColor;		
	osg::Vec4		_infectedAgentColor;	
	osg::Vec4		_recoveredAgentColor;	
	osg::Vec4		_vaccinatedAgentColor;	
	osg::Vec4		_deadAgentColor;

	std::vector< AgentDetail >		_agentDetails;
	std::vector< unsigned int >		_agentSteps;
	std::vector< unsigned int >     _transhipment;

	osg::ref_ptr < osg::Group >     _root;

	osg::ref_ptr< osg::Switch >		_agentStepSwitch;
	osg::ref_ptr< osg::Switch >		_agentSwitch;
	osg::ref_ptr< osg::Sequence >	_agentStepSequence;
	osg::ref_ptr< osg::Sequence >	_agentSequence;

  std::string _workingDir;
  ModelPresentationLib::RefPtr _mpdWriter;


};

#endif	// _CLASS_AGENT_H_
