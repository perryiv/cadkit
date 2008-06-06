
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This is a class for IMD communication.  It is similar in functionality as
//  the IMD routines for VMD.  http://www.ks.uiuc.edu/Research/vmd/
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMD_H__
#define __IMD_H__

#include "Usul/System/Socket.h"
#include "Usul/Types/Types.h"

namespace OsgFox {
namespace Plugins {
namespace IMD {

typedef Usul::Types::Int32 int32;

struct Energies
{
  //Energies are in Kcal/mol
  int32 timestep;
  float temperature;
  float total;
  float potential;
  float vanderwaals;
  float electrostatic;
  float bond;
  float angle;
  float dihedral;
  float improper;
};

class IMD
{
public:
  IMD( const std::string& host, unsigned int port );
  
  enum IMDType //Message types
  {
    DISCONNECT,   // close IMD connection, leaving sim running 
    ENERGIES,     // energy data block 
    FCOORDS,      // atom coordinates  ( no forces )
    GO,           // start the simulation 
    HANDSHAKE,    // endianism and version check message      
    KILL,         // kill the simulation job, shutdown IMD     
    MDCOMM,       // MDComm style force data ( not currently used )                
    PAUSE,        // pause the running simulation              
    TRATE,        // set IMD update transmission rate          
    IOERROR       // indicate an I/O error                     
  };

  enum Endianism
  {
    SAME,
    OPPOSITE,
    UNKNOWN
  };


  // control messages
  bool disconnect();   
  bool pause();        
  bool kill();         
  bool handshake();    
  bool trate(int32); 


  //forces units are Kcal/mol/angstrom

  bool sendMDcomm   ( int32, const int32 *, const float *);
  bool sendEnergies ( const Energies *);
  bool sendCoords   ( int32, const float *);


  bool    receiveHandshake();
  IMDType receiveHeader( int32 &);
 
  bool receiveMDcomm   ( int32, int32 *, float *);
  bool receiveEnergies ( Energies *) const;
  bool receiveCoords   ( int32, float *) const;

  Endianism endianism() const { return _endianism; }
  bool need2Flip() const { return _endianism == OPPOSITE; }

private:
  bool _writeHeader( Usul::System::Buffer &buf );
  bool _go();
  IMDType _recv_header_nolengthswap(int32 &length);

  const unsigned int _headerSize;
  const unsigned int _version;

  Usul::System::Socket _socket;


  Endianism _endianism;

};

}
}
}

#endif

