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
  $page_title = "Minerva -- Build";
  $content_title = "Minerva -- Build";
  require ( "start_page.php" );
?>
<p>Dependencies:</p>
<ul>
	<li><a href="www.openscenegraph.org">OpenSceneGraph</a></li>
	<li><a href="www.ossim.org">OSSIM</a></li>
	<li><a href="www.boost.org">Boost</a></li>
	<li><a href="http://xerces.apache.org/xerces-c/">Xerces-c</a></li>
	<li><a href="http://pqxx.org/development/libpqxx/">pqxx</a></li>
</ul>
<p>Desktop Minerva requires <a href="www.trolltech.com">Qt</a></p>
<p>VR Minerva requires <a href="www.vrjuggler.org">VR Juggler</a></p>
<p>Optional dependencies:</p>
<ul>
	<li><a href="www.gdal.org">GDAL</a></li>
</ul>
<p>Minerva uses Microsoft Visual Studio 2005 for Windows builds and CMake for all other platforms.</p>
<?php require ( "end_page.php" ); ?>
</html>
