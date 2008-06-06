<html>
<?php
  $page_title = "G-NURBS -- Coordinate Transformations";
  $content_title = "G-NURBS -- Coordinate Transformations";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    After a spline has been 
    <a href="create.php">created</a>
    it can be manipulated with a coordinate transformation.
    
    All of the coordinate transformation methods are template functions in 
    <?php write_cpp ( "namespace GN::Transform" ); ?>.
  </p>
  <p>
    <?php require "example_common.php" ?>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Matrix</h3>
  <p>
    To transform a curve using a 4x4 matrix.
    <pre>
      Curve c;
      Curve::Matrix44 m;
      m.setTranslation ( Curve::Vec3 ( 1, 2, 3 ) );
      GN::Transform::matrix ( c, m );
    </pre>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Translate</h3>
  <p>
    To translate a spline.
    <pre>
      Spline s;
      Spline::Vec3 v ( 1, 2, 3 );
      GN::Transform::translate ( s, v );
    </pre>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Scale</h3>
  <p>
    To scale a curve.
    <pre>
      Curve c;
      Spline::Vec3 s ( 1, 2, 3 );
      GN::Transform::scale ( c, s );
    </pre>
  </p>
</blockquote>
<?php require ( "end_page.php" ); ?>
</html>
