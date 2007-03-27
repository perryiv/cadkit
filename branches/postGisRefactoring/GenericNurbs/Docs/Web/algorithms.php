<html>
<?php
  $page_title = "G-NURBS -- Algorithms";
  $content_title = "G-NURBS -- Algorithms";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    G-NURBS has several algorithms that operate on the splines, or on their components (e.g., a knot vector).
    All of the algorithms are template functions in 
    <?php write_cpp ( "namespace GN::Algorithms" ); ?>.
  </p>
  <p>
    <?php require "example_common.php" ?>
    Also, where not specified, the examples also assume that the splines have been properly constructed.
  </p>
</blockquote>
  <br /><?php require "horizontal_rule.php" ?><br />
<blockquote>
  <h3>Find Knot Span</h3>
  <p>
    To find the knot-span of parameter <i>u</i> in the knot vector of curve <i>c</i>:
    <pre>
      Curve c;
      myCurveCreate ( c ); // Construct curve in here.
      Curve::UIntType span = GN::Algorithms::findKnotSpan ( c, u );
    </pre>
    To find the knot-spans of the parameters <i>u</i> and <i>v</i> in the corresponding knot vectors of surface <i>s</i>:
    <pre>
      Surface s;
      mySurfaceCreate ( s ); // Construct surface in here.
      Surface::UIntType spanU = GN::Algorithms::findKnotSpan ( s, 0, u );
      Surface::UIntType spanV = GN::Algorithms::findKnotSpan ( s, 1, v );
    </pre>
  </p>
</blockquote>
<?php require ( "end_page.php" ); ?>
</html>
