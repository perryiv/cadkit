<html>
<?php
  $page_title = "G-NURBS -- Inheritance";
  $content_title = "G-NURBS -- Inheritance";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    The inheritance tree is simply:
    <pre>
              Spline
              /    \
          Curve  Surface
    </pre>

    The base class, 
    <?php write_cpp ( "Spline" ); ?>,
    can be any number of independent variables.

    Inheriting from
    <?php write_cpp ( "Spline" ); ?>
    are
    <?php write_cpp ( "Curve" ); ?>
    and
    <?php write_cpp ( "Surface" ); ?>,
    which assume the number of independent variables are 1 and 2, respectively.
  </p>
</blockquote>
<?php require ( "end_page.php" ); ?>
</html>
