
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterPDB.h"

const char* ReaderWriterPDB::className()
{
  return "PDB Reader/Writer";
}

bool ReaderWriterPDB::acceptsExtension(const string &ext)
{
  return osgDB::equalCaseInsensitive(ext, "pdb");
}

osgDB::ReaderWriter::ReadResult ReaderWriterPDB::readNode(const string& file, const osgDB::ReaderWriter::Options *options)
{
  if( !acceptsExtension(osgDB::getFileExtension(file) ))
        return ReadResult::FILE_NOT_HANDLED;

  ifstream fd(file.c_str());
  if(!fd.is_open())
    return ReadResult::ERROR_IN_READING_FILE;

  osg::Group *root = new osg::Group();

  osg::Geode *geode = new osg::Geode();
 
  char buf[82];
  while(!fd.eof()) {
    fd.getline(buf, 82);
    if(strncmp(buf, "ATOM  ", 6) == 0){
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
	  osg::Sphere *s = new osg::Sphere(osg::Vec3(x,y,z), r);
      osg::ShapeDrawable *sdraw = new osg::ShapeDrawable(s);
      geode->addDrawable(sdraw);
    }
    
  }
  root->addChild(geode);

  return root;

}

osgDB::RegisterReaderWriterProxy<ReaderWriterPDB> g_ReaderWriter_PDB_proxy;

