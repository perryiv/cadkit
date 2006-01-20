<?php 

echo "<table align=\"center\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">\n";
echo "<tr><td height=\"$nav_bar_top_margin\"></td></tr>\n";

$url = basename ( $PHP_SELF );

main_navigation_bar_entry ( "index.php",         "Home",        $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "algorithms.php",    "Algorithms",  $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "create.php",        "Create",      $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "download.php",      "Download",    $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "doxygen.php",       "Doxygen",     $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "evaluate.php",      "Evaluate",    $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "inheritance.php",   "Inheritance", $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "library.php",       "Library",     $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "license.php",       "License",     $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );
main_navigation_bar_entry ( "transform.php",     "Transform",   $nav_bar_vertical_space, $nav_bar_current_cell_color, $nav_bar_current_text_color, $url );

echo "</table>";

?>
