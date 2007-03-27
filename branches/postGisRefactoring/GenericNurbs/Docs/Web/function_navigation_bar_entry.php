
<?php


///////////////////////////////////////////////////////////////////////////////
//
//  Make the entry in the navigation bar.
//
//  $filename = The file we will link to.
//  $url =      The link just clicked on. It will be the current page.
//
///////////////////////////////////////////////////////////////////////////////

function is_current_navigation ( $filename, $url )
{
  // Drop the extension.
  $filename = drop_file_extension ( $filename );

  // See if the url contains the filename at the very beginning.
  $pos = strpos ( $url, $filename );

  // Is it's the integer zero then it's a match.
  return ( is_integer ( $pos ) && 0 == $pos );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the entry in the main (vertical) navigation bar.
//
//  $filename =   The file we will link to.
//  $text =       What the link will say.
//  $space =      Vertical space between entries.
//  $cell_color = Color of the cell if it is the current page.
//  $text_color = Color of the text if it is the current page.
//  $url =        The link just clicked on. It will be the current page.
//
///////////////////////////////////////////////////////////////////////////////

function main_navigation_bar_entry ( $filename, $text, $space, $cell_color, $text_color, $url )
{
  // The row for vertical space and the start of the next row.
  echo "<tr><td height=\"$space\"></td></tr>\n";
  echo "<tr>\n";

  // The left margin for the entries (now that we are left justified).
  $left_margin = "25";

  // If the given file is the current one...
  if ( is_current_navigation ( $filename, $url ) )
  {
    // Some space on the left (now that we have it left justified).
    // Note: you need to specify white space or else Netscape doesn't 
    // color the cell.
    echo "<td bgcolor=\"$cell_color\" width=\"$left_margin\">&nbsp;&nbsp;</td>\n";

    // Start the cell, set its color.
    echo "<td bgcolor=\"$cell_color\" align=\"left\">\n";

    // Change the font color.
    echo "<font color=\"$text_color\">\n";

    // The text is not a link.
    echo "<b>$text</b>\n";

    // End the font.
    echo "</font>\n";
  }

  // Otherwise...
  else
  {
    // Some space on the left (now that we have it left justified).
    echo "<td width=\"$left_margin\"></td>\n";

    // Start the cell, do not set the color.
    echo "<td align=\"left\">\n";

    // The text is a link.
    echo "<a href=\"$filename\"><b>$text</b></a>\n";
  }

  // End the cell and row.
  echo "</td>\n</tr>\n";
}

///////////////////////////////////////////////////////////////////////////////
//
//  Make the entry in the horizontal navigation bar.
//
//  $filename =   The file we will link to.
//  $text =       What the link will say.
//  $url =        The link just clicked on. It will be the current page.
//
///////////////////////////////////////////////////////////////////////////////

function horizontal_navigation_bar_entry ( $filename, $text, $url )
{
  // If the given file is the current one...
  // Note, don't use is_current_navigation().
  if ( $filename == $url )
  {
    // Not a link.
    echo "$text";
  }

  // Otherwise...
  else
  {
    // A link.
    echo "<a href=\"$filename\">$text</a>";
  }
}

?>
