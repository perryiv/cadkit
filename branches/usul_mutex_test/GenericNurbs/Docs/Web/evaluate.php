<html>
<?php
  $page_title = "G-NURBS -- Evaluate";
  $content_title = "G-NURBS -- Evaluate";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    Once a curve, surface, or an arbitrary spline with <i>n</i>-number of independent variables
    has been <a href="create.php">created</a>, 
    evaluation of the dependent variables (i.e., points) is straightforward.

    All of the evaluation methods are template functions in 
    <?php write_cpp ( "namespace GN::Evaluate" ); ?>.
  </p>
  <p>
    <?php require "example_common.php" ?>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Curve Point</h3>
  <p>
    To evaluate a single curve point:
    <pre>
      Curve c;
      myCurveCreate ( c );                   // Construct curve in here.
      const Curve::IndependentType u = 0.5;  // Initialize the independent variable.
      Curve::Vector pt ( c.dimension() );    // Declare and size the dependent variable.
      GN::Evaluate::point ( c, u, pt );      // Call point-evaluation algorithm.
    </pre>
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Surface Point</h3>
  <p>
    To evaluate a single surface point:
    <pre>
      Surface s;
      mySurfaceCreate ( s );                 // Construct surface in here.
      const Curve::IndependentType u = 0.5;  // Initialize the first  independent variable.
      const Curve::IndependentType v = 0.5;  // Initialize the second independent variable.
      Surface::Vector pt ( s.dimension() );  // Declare and size the dependent variable.
      GN::Evaluate::point ( s, u, v, pt );   // Call point-evaluation algorithm.
    </pre>
  </p>
</blockquote>
<?php require ( "end_page.php" ); ?>
</html>
