
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterPDB.h"

#include "osg/Geode"
#include "osg/LOD"
#include "osg/Shape"
#include "osg/ShapeDrawable"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include <fstream>
#include <sstream>
#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  We're packing a-y-x and radius into a 4D vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Index
{
  const unsigned int X ( 0 );
  const unsigned int Y ( 1 );
  const unsigned int Z ( 2 );
  const unsigned int R ( 3 );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::ReaderWriterPDB() :
  _atoms(),
  _bbox()
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
  return "PDB Reader/Writer";
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
  // Safely...
  try
  {
    return this->_read ( file, options );
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the internal data members.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_init()
{
  _atoms.clear();
  osg::BoundingBox bbox;
  _bbox = bbox;
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

  // Parse all the file and build internal data.
  this->_parse ( in );

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

  // Loop through all the atoms.
  for ( Atoms::const_iterator i = _atoms.begin(); i != _atoms.end(); ++i )
  {
    // Get the atom.
    const Atom &atom = *i;

    // make the geometry for this point.
    osg::ref_ptr<osg::LOD> lod ( this->_makeAtom ( atom ) );

    // Add the lod to the root.
    root->addChild ( lod.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an atom.
//  TODO, make several branches in the lod. 
//  Use bounding-box info to set lod-ranges.
//
///////////////////////////////////////////////////////////////////////////////

osg::LOD *ReaderWriterPDB::_makeAtom (const Atom &atom ) const
{
  // The lod holding the various representations.
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

  // Get the data.
  const std::string line = atom.toString();
  const osg::Vec3 center (atom.getX(), atom.getY(), atom.getZ());
  const float radius = atom.getR();

  osg::StateSet *ss = new osg::StateSet();
  const std::string s = atom.getName();
  osg::ref_ptr< osg::Material > m = materialChooser.getMaterial(s);
  ss->setAttribute(m.get());

  // Make a sphere at this point.
  osg::ref_ptr<osg::Sphere> sphere ( new osg::Sphere ( center, radius ) ); // TODO, remove hard-coded sphere.
  osg::ref_ptr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable ( sphere.get() ) ); // TODO, use osg::Geometry for vertex arrays.
  drawable->setStateSet(ss);

  // Not so many triangles.
  osg::ref_ptr<osg::TessellationHints> hints ( new osg::TessellationHints() );
  hints->setDetailRatio ( 0.25f ); // TODO, remove hard-coded value.
  drawable->setTessellationHints ( hints.get() );

  // Add the sphere to a geode.
  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  geode->addDrawable ( drawable.get() );

  // Name the geode with the line from the file.
  geode->setName ( line );

  // Add the geode to the lod.
  lod->addChild ( geode.get() );

  // Set the range.
  lod->setRange ( 0, 0, std::numeric_limits<float>::max() ); // TODO, use bounding-box to set ranges.

  // Return the lod.
  return lod.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_parse ( std::ifstream &in )
{
  // The buffer that holds the lines. Plenty of padding just to be safe.
  const unsigned int size ( 512 );
  char buf[size];
  int numAtoms = 0;

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

    // See if it is an atom.
    if ( "ATOM" == type )
    {
		numAtoms++;
		Atom atom(buf, type);
		//std::cout << numAtoms << " = " << atom.getId() << std::endl;
		//if(atom.getId() != numAtoms) {
		//	std::cerr << "Atoms are not sequential\n";
		//}
		_atoms.push_back( atom );
    }
	else if( type == "HETATM") {
		/*numAtoms++;
		Atom atom(in, type);
		//std::cout << numAtoms << " = " << atom.getId() << std::endl;
		//if(atom.getId() != numAtoms) {
		//	std::cerr << "Atoms are not sequential\n";
		//}
		_atoms.push_back( atom );
		*/
	}
	else if( type == "CONECT"){
		/*int id;
		in >> id;
		
		Atoms::const_iterator i = _atoms.begin();
		i = i + id;
		Atom &atom = *i;
	*/
		//TODO find atoms in _atoms and connect with bonds
	}
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  This global instance gets instantiated when the plugin is loaded.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterPDB> g_ReaderWriter_PDB_proxy;









#if 0


///////////////////////////////////////////////////////////////////////////////
//
//  Read the Protein Data Bank (PDB) file and return the scene.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::ReaderWriter::ReadResult ReaderWriterPDB::readNode(const std::string& file, const osgDB::ReaderWriter::Options *options)
{
  if( !acceptsExtension(osgDB::getFileExtension(file) ))
        return ReadResult::FILE_NOT_HANDLED;

  ifstream fd(file.c_str());
  if(!fd.is_open())
    return ReadResult::ERROR_IN_READING_FILE;

  osg::ref_ptr<osg::Group> root = new osg::Group();

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();

  const unsigned int size ( 512 );
  char buf[size];
  while(!fd.eof())
  {
    fd.getline(buf, size-1);
    if(strncmp(buf, "ATOM  ", 6) == 0)
    {
      float x,y,z,r;
      r = 1.0f;
      char num[9];
      memset(num, 0, 9 * sizeof(char));
      strncpy(num, buf+30, 8);
      x = atof(num);
      memset(num, 0, 9 * sizeof(char));
      strncpy(num, buf+38, 8);
      y = atof(num);
      memset(num, 0, 9 * sizeof(char));
      strncpy(num, buf+46, 8);
      z = atof(num);
      osg::ref_ptr<osg::Sphere> s = new osg::Sphere(osg::Vec3(x,y,z), r);
      osg::ref_ptr<osg::ShapeDrawable> sdraw = new osg::ShapeDrawable(s.get());
      geode->addDrawable(sdraw.get());
    }
    
  }
  root->addChild(geode.get());

  return root.release();

}


#endif
