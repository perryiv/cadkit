<html>
<?php
  $page_title = "G-NURBS -- Home";
  $content_title = "G-NURBS -- Home";
  require ( "start_page.php" );
?>
<blockquote>
  <p>
    G-NURBS is a C++ template library of <i>Non-Uniform Rational B-Spline</i> (NURBS) objects and operations.

    The objects are configured using a policy class.
  </p>
  <p>
    Several policy classes are provided which can be used as-is.
    
    They also serve as an example of how to customize your own policy.
  </p>
  <p>
    For example, the following code will declare a NURBS-curve type that uses 
    <?php write_cpp ( "double" ); ?>
    as the scalar data type, and 
    <?php write_cpp ( "std::vector" ); ?>
    as the container of the scalar values.

    <pre>
      #include "GN/Config/UsulConfig.h"
      #include "GN/Splines/Curve.h"

      typedef GN::Config::Usul::Vector&lt;double&gt Config;
      typedef GN::Splines::Curve&lt;Config&gt       Curve;
    </pre>

    The above configuration uses the
    <?php write_cpp ( "Usul" ); ?>
    library to provide the 2D, 3D, and 4D vector types, as well as the 4x4 matrix type.
  </p>
  <p>
    Alternatively, one could use the 
    <?php write_cpp_as_external_link ( "gmtl", "http://ggt.sourceforge.net" ); ?>
    library, and
    <?php write_cpp ( "float" ); ?>
    as the data type.

    <pre>
      #include "GN/Config/GmtlConfig.h"
      #include "GN/Splines/Curve.h"

      typedef GN::Config::gmtl::Vector&lt;float&gt Config;
      typedef GN::Splines::Curve&lt;Config&gt      Curve;
    </pre>

    Use the navigation bar in the upper left to see how to create, modify, and evaluate a spline.
  </p>
</blockquote>
<?php require ( "end_page.php" ); ?>
</html>
