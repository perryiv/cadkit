
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterPDB.h"
#include "Enum.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "XmlDom/Policy.h"
#include "XmlDom/Reader.h"
#include "XmlDom/File.h"
#include "XmlDom/BuildTree.h"
#include "XmlDom/Node.h"
#include "XmlDom/Convert.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Predicates/FileExists.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cassert>

#include <ctime>
#include <iostream>

#include "Atom.h"
#include "Bond.h"

#ifdef _WIN32
# define STAT _stat
#else
# define STAT stat
#endif

//#define USE_EXCEPTIONS


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::ReaderWriterPDB() :
  _molecules( new MoleculeList ),
  _materialFactory ( new MaterialFactory ),
  _currentMolecule(NULL),
  _shapeFactory ( new ShapeFactory ),
  _periodicTable(),
  _flags ( PDB::SHOW_ATOMS | PDB::SHOW_BONDS | PDB::LOAD_ATOMS | PDB::LOAD_BONDS )
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
  _molecules->clear();
  _materialFactory->clear();
  _currentMolecule = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the atoms.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::Result ReaderWriterPDB::_read ( const std::string &file, const osgDB::ReaderWriter::Options *options )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Configure this instance based on the options.
  this->_configure ( options );

  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( file ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Open the file.
  std::ifstream in ( file.c_str() );

  // Make sure it opened.
  if ( !in.is_open() )
    return ReadResult::ERROR_IN_READING_FILE;

  // Create ifstream for psf file.
  //std::string psfPath = _getPsfPath( file );
  //std::ifstream psf( psfPath.c_str());

  //Get the file size
  struct STAT filebuf;
  int result ( _stat ( file.c_str(), &filebuf ) );
  
  // Parse all the file and build internal data.
  this->_parse ( in, filebuf.st_size );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( _build() );

  // Initialized again to free accumulated data.
  //this->_init();

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
  root->setUserData( _molecules.get() );

  Molecules molecules ( _molecules->molecules() );

  //Loop through the molecules
  for (Molecules::const_iterator i = molecules.begin(); i != molecules.end(); ++i)
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

void ReaderWriterPDB::_parse ( std::ifstream &in, unsigned int filesize )
{
  clock_t start, finish; // used by clock()
	double total_second;

  start = clock(); // start to record the time

  // The buffer that holds the lines. Plenty of padding just to be safe.
  const unsigned int size ( 512 );
  char buf[size];
  Molecule * molecule = NULL;

  unsigned int estimatedNumAtoms ( filesize / 80 );

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
    if ( "MODEL" == type ) 
      molecule = this->_getCurrentMolecule( estimatedNumAtoms );
    else if ( "ENDMDL" == type )
      _currentMolecule = NULL;

    // See if it is an atom.
    else if ( "ATOM" == type && this->hasFlags ( PDB::LOAD_ATOMS ) )
    {
      molecule = this->_getCurrentMolecule( estimatedNumAtoms );
      molecule->addAtom( new Atom( buf, type, _periodicTable ) );
    }
	  else if ( "HETATM" == type && this->hasFlags ( PDB::LOAD_ATOMS ) )
    {
      molecule = this->_getCurrentMolecule( estimatedNumAtoms );
      molecule->addAtom( new Atom( buf, type, _periodicTable ) );
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

//  if ( psf.is_open() && this->hasFlags ( PDB::LOAD_BONDS ) )
//    this->_parsePsf ( psf );

  
	finish = clock();   // end of the computation; record the time
	total_second = (double)(finish - start) / CLOCKS_PER_SEC;

  std::cout << "Time to parse: " << total_second << std::endl;
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
//  Returns pointer to current molecule. If none exists, it creates a new one.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::Molecule* ReaderWriterPDB::_getCurrentMolecule( unsigned int numAtoms )
{
  if(_currentMolecule == NULL)
  {
    _currentMolecule = new Molecule ( _materialFactory.get(), _shapeFactory.get(), _flags, numAtoms );
    osg::ref_ptr< Molecule > r ( _currentMolecule );
    _molecules->push_back(r);
  }
  return _currentMolecule;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gets path to PSF file given a PDB path.
//
///////////////////////////////////////////////////////////////////////////////

std::string ReaderWriterPDB::_getPsfPath( const std::string &file)
{
  std::string psf  ( osgDB::getStrippedName(file) );
  std::string path ( osgDB::getFilePath(file) );
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
//  Configure this instance by parsing the options string.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_configure ( const Options *options )
{
  // For convenience.
  typedef XML::Config::Policy<>                 Policy;
  typedef XML::Node < Policy >                  Node;
  typedef XML::Callback::BuildTree < Node >     BuildCB;
  typedef XML::Reader < Policy, BuildCB >       Reader;
  typedef XML::File < Policy >                  File;

  // Handle trivial case.
  if ( !options )
    return;

  // Get the string.
  std::string chunk ( options->getOptionString() );

  // If the chunk is a filename then get the contents.
  if ( Usul::Predicates::FileExists() ( chunk ) )
    this->_getFileContents ( chunk, chunk );

  // Construct the node tree and get the root.
  Reader reader ( chunk.begin(), chunk.end() );
  Node *root = reader.callback().root();

  // Needed to convert.
  XML::Convert<bool> toBool;

  // Set our members.
  this->setFlags ( PDB::SHOW_ATOMS, toBool ( root->child ( "atoms/show", '/' )->value() ) );
  this->setFlags ( PDB::SHOW_BONDS, toBool ( root->child ( "bonds/show", '/' )->value() ) );
  this->setFlags ( PDB::LOAD_ATOMS, toBool ( root->child ( "atoms/load", '/' )->value() ) );
  this->setFlags ( PDB::LOAD_BONDS, toBool ( root->child ( "bonds/load", '/' )->value() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contents of the file as a string.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_getFileContents ( const std::string &filename, std::string &contents ) const
{
  struct STAT buf;
  int result = STAT ( filename.c_str(), &buf );
  if ( result )
  {
    // Size the string that will hold the entire file.
    contents.resize ( buf.st_size );

    // Open the file.
    std::ifstream in ( filename.c_str() );
    if ( in.is_open() )
    {
      // Read the file into the string.
      for ( std::string::size_type i = 0; i < contents.size(); ++i )
        contents[i] = in.get();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::setFlags ( unsigned int flags, bool state )
{
  if ( state )
    this->addFlags ( flags );
  else
    this->removeFlags ( flags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the flags.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::addFlags ( unsigned int flags )
{
  _flags = Usul::Bits::add ( _flags, flags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the flags.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::removeFlags ( unsigned int flags )
{
  _flags = Usul::Bits::remove ( _flags, flags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if we have the flags.
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterPDB::hasFlags ( unsigned int flags ) const
{
  return Usul::Bits::has ( _flags, flags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  This global instance gets instantiated when the plugin is loaded.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterPDB> g_ReaderWriter_PDB_proxy;
