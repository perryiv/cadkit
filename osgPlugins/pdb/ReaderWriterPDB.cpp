
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterPDB.h"

#include "osg/Geometry"
#include "osg/LineWidth"

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
#include <sys/stat.h>
#include <cassert>

#ifdef _WIN32
#define STAT _stat
#else
#define STAT stat
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterPDB::ReaderWriterPDB() :
  _atoms(),
  _bonds(),
  _materialChooser(),
  _maxDistanceFactor ( 30 ),
  _lastRangeMax ( std::numeric_limits<float>::max() )
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
  _bonds.clear();
  _materialChooser.clear();
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

  struct STAT filebuf;

  int result = _stat ( file.c_str(), &filebuf );
  assert ( 0 == result );

  // Parse all the file and build internal data.
  this->_parse ( in, filebuf.st_size );

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

    if(i->getId() != -1) 
    {
      // make the geometry for this point.
      osg::ref_ptr<osg::LOD> lod ( this->_makeAtom ( atom ) );

      // Add the lod to the root.
      root->addChild ( lod.get() );
    }
  }

  for (Bonds::const_iterator i = _bonds.begin(); i != _bonds.end(); ++i)
  {
    const Bond &bond = *i;
    osg::ref_ptr<osg::LOD> lod (this->_makeBond( bond ) );
    root->addChild ( lod.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an bond.
//  TODO, make several branches in the lod. 
//  Use bounding-box info to set lod-ranges.
//
///////////////////////////////////////////////////////////////////////////////

osg::LOD *ReaderWriterPDB::_makeBond (const Bond &bond ) const
{
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

  const osg::Vec3 center (bond.getX(), bond.getY(), bond.getZ());
  const float height = bond.getH();
  const float radius = bond.getR();
  //std::cout << height << std::endl;
  /*
  //draw a cylinder
  osg::ref_ptr<osg::Shape> cylinder ( new osg::Cylinder (center, radius, height));
  osg::ref_ptr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable(cylinder.get()));

  
  // Not so many triangles.
  osg::ref_ptr<osg::TessellationHints> hints ( new osg::TessellationHints() );
  hints->setDetailRatio ( 0.25f ); // TODO, remove hard-coded value.
  drawable->setTessellationHints ( hints.get() );
  */  
  // Add the cylinder to a geode.
  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  //geode->addDrawable ( drawable.get() );
  
  
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  osg::ref_ptr<osg::Vec3Array> p ( new osg::Vec3Array );

  p->resize ( 2 );
  (*p)[0] = bond.getPoint1();
  (*p)[1] = bond.getPoint2();

  geom->setVertexArray ( p.get() );
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, 2 ) );

  osg::ref_ptr<osg::Vec4Array> c ( new osg::Vec4Array );
  c->resize ( 1 );
  (*c)[0] = osg::Vec4(1.0, 1.0, 1.0, 1.0);

  geom->setColorArray ( c.get() );
  geom->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE_SET );

  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( 5 );

  osg::ref_ptr<osg::StateSet> state = geom->getOrCreateStateSet();
  state->setAttribute ( lw.get() );
  state->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );

  geode->addDrawable ( geom.get() );

  // Name the geode with the line from the file.
  geode->setName ( bond.toString() );

  // Add the geode to the lod.
  lod->addChild ( geode.get() );

  // Set the range.
  lod->setRange ( 0, 0, std::numeric_limits<float>::max() ); // TODO, use bounding-box to set ranges.

  return lod.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Make an atom.
//
///////////////////////////////////////////////////////////////////////////////

osg::LOD *ReaderWriterPDB::_makeAtom ( const Atom &atom ) const
{
  // The lod holding the various representations.
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

   // Set the lod's material. This will effect all the children.
  osg::ref_ptr<osg::StateSet> ss ( lod->getOrCreateStateSet() );
  const std::string type ( atom.getName() );
  osg::ref_ptr<osg::Material> m ( _materialChooser.getMaterial ( type ) );
  ss->setAttribute ( m.get() );

  // Get the atom's numbers.
  const osg::Vec3 center ( atom.getX(), atom.getY(), atom.getZ() );
  const float radius ( atom.getR() );

  // Name the lod with the data from the atom.
  lod->setName ( atom.toString() );

  // Add several spheres.
  unsigned int numChildren ( 5 );
  for ( unsigned int i = 0; i < numChildren - 1; ++i )
  {
    float detail ( 1.0f - (float) i / ( numChildren - 1 ) );
    lod->addChild ( this->_makeSphere ( center, radius, detail ) );
  }

  // Last child is a cube.
  //lod->addChild ( this->_makeCube ( center, radius * 1.5 ) );

  // Set the centers and ranges.
  this->_setCentersAndRanges ( lod.get() );

  // Return the lod.
  return lod.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere. TODO, use osg::Geometry for vertex arrays. 
//  Perhaps make soccer ball.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode *ReaderWriterPDB::_makeSphere ( const osg::Vec3 &center, float radius, float detail ) const
{
  // Make the sphere.
  osg::ref_ptr<osg::Sphere> sphere ( new osg::Sphere ( center, radius ) );
  osg::ref_ptr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable ( sphere.get() ) );

  // Adjust the number of triangles.
  osg::ref_ptr<osg::TessellationHints> hints ( new osg::TessellationHints() );
  hints->setDetailRatio ( detail );
  drawable->setTessellationHints ( hints.get() );

  // TODO, make this an option. Display lists crash with really big files.
  drawable->setUseDisplayList ( false );

  // Add the sphere to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( drawable.get() );

  // Return the geode.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a cube. TODO, use osg::Geometry for vertex arrays. 
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode *ReaderWriterPDB::_makeCube ( const osg::Vec3 &center, float size ) const
{
  // Make the sphere.
  osg::ref_ptr<osg::Box> cube ( new osg::Box ( center, size ) );
  osg::ref_ptr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable ( cube.get() ) );

  // Add the cube to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( drawable.get() );

  // Return the geode.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_parse ( std::ifstream &in, int filesize )
{
  // The buffer that holds the lines. Plenty of padding just to be safe.
  const unsigned int size ( 512 );
  char buf[size];
  int numAtoms = 0;

  _atoms.resize (filesize / 80);

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
    _atoms.at( atom.getId() ) = atom;
    }
	  else if( type == "HETATM") 
    {
		  /*numAtoms++;
		  Atom atom(in, type);
		  //std::cout << numAtoms << " = " << atom.getId() << std::endl;
		  //if(atom.getId() != numAtoms) {
		  //	std::cerr << "Atoms are not sequential\n";
		  //}
		  _atoms.push_back( atom );
		  */
	  }
	  else if( type == "CONECT")
    {
		  int id, connect, len, columnStart = 6;
      const int columnLength = 5;
      char num[6];
      len = strlen(buf);
      memset(num, 0, 6 * sizeof(char));
      strncpy(num, buf + columnStart, columnLength);
      id = atoi (num);
      columnStart += columnLength;
      while(columnStart < len - 5)
      {
        memset(num,0, 6*sizeof(char));
        strncpy(num, buf + columnStart, columnLength);
        connect = atoi (num);
        if ( _atoms.at(id).getId() != -1 && _atoms.at(connect).getId() != -1 )
          _bonds.push_back( Bond (_atoms.at(id), _atoms.at(connect), _bonds.size() + 1));
        columnStart += columnLength;
      }
	  }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod center and ranges.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterPDB::_setCentersAndRanges ( osg::LOD *lod ) const
{
  // If there are no children then just return.
  if ( 0 == lod->getNumChildren() )
    return;

  // Get the first child.
  osg::Node *child = lod->getChild ( 0 );

  // Get the bounding sphere for the first child.
  const osg::BoundingSphere &boundingSphere = child->getBound();
  if ( boundingSphere.radius() <= 0 )
    throw ( std::runtime_error ( "Error 1614663463, bounding sphere has zero radius" ) );

  // The maximum distance for the lod ranges.
  float maxDist ( _maxDistanceFactor * boundingSphere.radius() );

  // Get the center of the bounding sphere.
  const osg::Vec3 &center = boundingSphere.center();

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  // The minimum of the range we set.
  float rangeMin ( 0 );

  // Loop through all of the children except the last one.
  // Note: Unlike previous versions of OSG, there is one LOD "range" for 
  // each LOD "child". Each "range" has a min and max value.
  unsigned int numChildren ( lod->getNumChildren() );
  for ( unsigned int i = 0; i < numChildren - 1; ++i )
  {
    // Set the range.
    float rangeMax = ( ( (float) i + 1 ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    lod->setRange ( i, rangeMin, rangeMax );
    rangeMin = rangeMax;
  }

  // Set the range for the last child.
  lod->setRange ( numChildren - 1, rangeMin, _lastRangeMax );
}


///////////////////////////////////////////////////////////////////////////////
//
//  This global instance gets instantiated when the plugin is loaded.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterPDB> g_ReaderWriter_PDB_proxy;
