
<?php

///////////////////////////////////////////////////////////////////////////////
//
//  Given "dir/filename.ext" return "dir/filename".
//
///////////////////////////////////////////////////////////////////////////////

function drop_file_extension ( $path )
{
  // Get the extension.
  $ext = substr ( $path, strrpos ( $path, "." ) + 1 );

  // Find the position of the start of the extension.
  $pos = strpos ( $path, ".$ext" );

  // Return the substring that stops at the extension.
  return substr ( $path, 0, $pos );
}

?>
