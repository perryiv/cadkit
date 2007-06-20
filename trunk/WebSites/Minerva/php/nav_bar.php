<!--
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
-->

<?php 

echo "<table align=\"center\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">\n";
echo "<tr><td height=\"$nav_bar_top_margin\"></td></tr>\n";

$url = basename ( $PHP_SELF );

main_navigation_bar_entry ( "index.php",        "Home",       $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "build.php",        "Build",      $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "subversion.php",   "Subversion", $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "documentation.php","Docs",       $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "license.php",      "License",    $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );

echo "</table>";

?>
