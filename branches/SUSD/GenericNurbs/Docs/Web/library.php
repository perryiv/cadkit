<html>
<?php
  $page_title = "G-NURBS -- Library";
  $content_title = "G-NURBS -- Library";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    G-NURBS exists entirely within header files, there is no linking required.

    The three classes,
    <?php write_cpp ( "Spline" ); ?>,
    <?php write_cpp ( "Curve" ); ?>, and
    <?php write_cpp ( "Surface" ); ?>,
    have the minimum methods necessary for accessing the spline's internal data.
    
    Other operations, such as creating a curve as a circle or evaluating a surface, exist as template functions.
  </p>
</blockquote>
<?php require ( "end_page.php" ); ?>
</html>
