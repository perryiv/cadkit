
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterPDB.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cassert>

#include "Atom.h"
#include "Bond.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::ReaderWriterPDB() :
  _molecules(),
  _materialChooser(),
  _currentMolecule(NULL),
  _sphereFactory ( new SphereFactory ),
  _cylinderFactory ( new CylinderFactory ),
  _periodicTable()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::~ReaderWriterPDB()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the "class name".
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterPDB::className()
{
  return "PDB Reader";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if we read files with the given extension.
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterPDB::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "pdb" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the atoms.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::Result ReaderWriterPDB::readNode ( const std::string &file, const osgDB::ReaderWriter::Options *options )
{
#define USE_EXCEPTIONS
#ifdef USE_EXCEPTIONS

  // Safely...
  try
  {

#endif

    return this->_read ( file, options );

#ifdef USE_EXCEPTIONS

  }

  // Catch known exceptions.
  catch ( const ReaderWriterPDB::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterPDB::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterPDB::Result ( "Unknown exception caught" );
  }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the internal data members.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_init()
{
  _molecules.clear();
  _materialChooser.clear();
  _currentMolecule = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the atoms.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::Result ReaderWriterPDB::_read ( const std::string &file, const osgDB::ReaderWriter::Options * )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Make sure we handle files with this extension.
  if ( !acceptsExtension ( osgDB::getFileExtension ( file ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Open the file.
  std::ifstream in ( file.c_str() );

  // Make sure it opened.
  if ( !in.is_open() )
    return ReadResult::ERROR_IN_READING_FILE;

  //create ifstream for psf file
  std::string psfPath = _getPsfPath( file );
  std::ifstream psf( psfPath.c_str());
  
  // Parse all the file and build internal data.
  this->_parse ( in, psf );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( _build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *ReaderWriterPDB::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  
  //Loop through the molecules
  for (Molecules::const_iterator i = _molecules.begin(); i != _molecules.end(); ++i)
  {
    const MoleculePtr &molecule = *i;
    osg::ref_ptr<osg::MatrixTransform> mt ((osg::MatrixTransform *) (molecule.get())->build() );
    root->addChild ( mt.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_parse ( std::ifstream &in, std::ifstream &psf )
{
  // The buffer that holds the lines. Plenty of padding just to be safe.
  const unsigned int size ( 512 );
  char buf[size];
  Molecule * molecule = NULL;

  

  // Loop until we reach the end of the file.
  while ( !in.eof() )
  {
    // Read the line.
    in.getline ( buf, size - 1 );

    // For convenience, read it into a string-stream.
    std::istringstream in ( buf );

    // Get the first word.
    std::string type;
    in >> type;

    // Make sure it is all capital letters.
    std::transform ( type.begin(), type.end(), type.begin(), ::toupper );

    // If we are at the end the break out of the loop. This added step is 
    // another layer of safety. (I've seen situations where the EOF character 
    // is never reached, resulting in an infinite loop.)
    if ( "END" == type )
      break;
    if("MODEL" == type) 
      molecule = this->_getCurrentMolecule();
    else if("ENDMDL" == type)
      _currentMolecule = NULL;
    // See if it is an atom.
    else if ( "ATOM" == type )
    {
      molecule = this->_getCurrentMolecule();
		  Atom atom(buf, type, _periodicTable);
      molecule->addAtom(atom);
    }
	  else if( type == "HETATM") 
    {
      molecule = this->_getCurrentMolecule();
		  Atom atom(buf, type, _periodicTable);
      molecule->addAtom(atom);
	  }
    /* Turn off CONECT for now
	  else if( type == "CONECT")
    {
      molecule = this->_getCurrentMolecule();
		  int id, connect, len, columnStart = 6;
      const int columnLength = 5;
      char num[6];
      len = strlen(buf);
      //get the id of the atom to connect bonds from
      memset(num, 0, 6 * sizeof(char));
      strncpy(num, buf + columnStart, columnLength);
      id = atoi (num);
      columnStart += columnLength;
      //loop through the connecting atoms and add bonds
      while(columnStart < len - 5)
      {
        memset(num,0, 6*sizeof(char));
        strncpy(num, buf + columnStart, columnLength);
        connect = atoi (num);
        molecule->addBond((Atom::ID) id, (Atom::ID) connect);
        columnStart += columnLength;
      }
	  }*/
  }
  if(psf.is_open())
    _parsePsf( psf );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse the PSF file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_parsePsf( std::ifstream &in )
{
  Molecule *molecule = this->_getCurrentMolecule();
  // The buffer that holds the lines. Plenty of padding just to be safe.
  const unsigned int size ( 512 );
  char buf[size];

  while(!in.eof())
  {
     // Read the line.
    in.getline ( buf, size - 1 );

    // For convenience, read it into a string-stream.
    std::istringstream line ( buf );

    int num;
    std::string type;

    line >> num >> type;

    //start reading bonds
    if(type == "!NBOND:"){
      in.getline( buf, size - 1 );
      std::istringstream bondLine( buf );
      for(int i = 0; i < num; i ++){
        Atom::ID atom1, atom2;
        //bonds are 4 per line
        bondLine >> atom1 >> atom2;
        molecule->addBond(atom1, atom2);
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns point to current molecule.  If none exists, it creates a new one.
//
///////////////////////////////////////////////////////////////////////////////

Molecule* ReaderWriterPDB::_getCurrentMolecule()
{
  if(_currentMolecule == NULL)
  {
    _currentMolecule = new Molecule ( &_materialChooser, _sphereFactory.get(), _cylinderFactory.get() );
    osg::ref_ptr< Molecule > r = _currentMolecule;
    _molecules.push_back(r);
  }
  return _currentMolecule;
}

std::string ReaderWriterPDB::_getPsfPath( const std::string &file)
{
    std::string psf = osgDB::getStrippedName(file);
  std::string path = osgDB::getFilePath(file);
  psf += ".psf";
  if(!path.empty())
  {
#if defined (WIN32)
    psf = path + "\\" + psf;
#else
  psf = path + "/" + psf;
#endif
  }
  return psf;
}


///////////////////////////////////////////////////////////////////////////////
//
//  This global instance gets instantiated when the plugin is loaded.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterPDB> g_ReaderWriter_PDB_proxy;
