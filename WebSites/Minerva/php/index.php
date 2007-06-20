<!--
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
-->

<html>
<?php
  $page_title = "Minerva -- Home";
  $content_title = "Minerva -- Home";
  require ( "start_page.php" );
?>
<p>
  
  Minerva is an end-user application that unifies several existing GIS tools with 
  high-performance, planet-wide 3D visualization. At the core, Minerva is 
  a Windows desktop program that follows the document-view paradigm – with 
  the ability to open multiple documents at once – and uses a plugin architecture 
  for extensible design. Minerva also has multi-screen, virtual reality display 
  capability, which can be thought of as another view to the document.

</p>
<p>

  Using the Windows desktop application, the user queries for information from 
  a variety of sources, including: WMS servers, postGIS databases, and the file 
  system. Minerva keeps track of these "layers" using a collection of abstract 
  base classes. Communication with the multi-screen display occurs via XML-based 
  serialization and deserialization of these layer objects through a specialized 
  table in a designated postGIS database. Minerva's primary strength is the ability 
  to display raster and vector data together from multiple sources with the benefit 
  of high-performance computer graphics (including animating through temporal data 
  sets). By using robust open source toolkits like 
  <a target="_blank" href="http://www.ossim.org/OSSIM/ossimPlanet.html">OSSIMPlanet</a>.
  we are able to manage gigabytes worth of terrain and image layers.

</p>
<p>

  Minerva is an open-source project under active development at 
  <a target="_blank" href="http://www.asu.edu">Arizona State University</a>'s 
  <a target="_blank" href="http://dt.asu.edu">Decision Theater</a>, 
  and is used in our production facility to support policy decision-making meetings 
  for our customers. Projects completed with Minerva vary from 
  <a target="_blank" href="http://picasaweb.google.com/perryiv/ScottsdaleUnifiedSchoolDistrict">school enrollment</a>
  to 
  <a target="_blank" href="http://picasaweb.google.com/perryiv/WestNileVirus">disease propagation</a>
  studies.

</p>
<p>

  The program is written with a combination of platform independent C++ and Microsoft managed C++ and C#.
  
  Source code is available via
  <a href="subversion.php">Subversion</a> courtesy of
  <a target="_blank" href="https://sourceforge.net/svn/?group_id=54156">SourceForge.net</a>.

</p>
<?php require ( "end_page.php" ); ?>
</html>
