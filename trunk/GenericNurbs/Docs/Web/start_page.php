<head>
  <?php 
    require ( "html_meta_tags.php" );
    require ( "function_drop_file_extension.php" );
    require ( "function_navigation_bar_entry.php" );
    require ( "function_write_cpp_code.php" );
    require ( "global_variables.php" );
  ?>
  <style>A{text-decoration:none}</style>
  <title><?php echo "$page_title" ?></title>
</head>
<!-- 
Attributes "leftmargin" and "topmargin" are for IE and are ignored by Netscape.
Attributes "marginheight" and "marginwidth" are for Netscape and are ignored by IE.
-->
<?php
  echo "<body ";
  echo "leftmargin=\"0\" ";
  echo "topmargin=\"0\" ";
  echo "marginheight=\"0\" ";
  echo "marginwidth=\"0\" ";
  echo "link=\"$link_color\" ";
  echo "alink=\"$active_link_color\" ";
  echo "vlink=\"$visited_link_color\" ";
  echo "bgcolor=\"$page_background_color\" ";
  echo "text=\"$page_text_color\" ";
  echo ">\n";
?>
  <table height="100%" width="97%" cellpadding="0" cellspacing="0" border="0" align="left">
    <tr>
      <?php
        echo "<td height=\"100%\" width=\"$nav_bar_width\" valign=\"top\">\n";
        require ( "nav_bar.php" );
        echo "</td>\n";
      ?>
      <td height="100%" width="2%">
      </td>
      <td height="100%" width="78%" valign="top">
        <br />
        <center>
          <font size="+2">
            <?php echo "$content_title" ?>
          </font>
        </center>
        <br /><?php require "horizontal_rule.php" ?><br />

<!-- Main body of page follows -->
