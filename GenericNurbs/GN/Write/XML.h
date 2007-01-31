
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  XML writer.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_WRITE_XML_H_
#define _GENERIC_NURBS_LIBRARY_WRITE_XML_H_

#include "GN/Config/BaseClass.h"


namespace GN {
namespace Write {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Structs to write the known base classes.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct BaseSplineClass
{
  template < class OutType, class StringType >
  static void write ( const SplineType &, const StringType &, OutType & )
  {
  }
};
template <> struct BaseSplineClass < GN::Config::Base::StringData >
{
  typedef GN::Config::Base::StringData SplineType;
  template < class OutType, class StringType >
  static void write ( const SplineType &s, const StringType &indent, OutType &out )
  {
    out << indent << "<string>" << s.getStringData().c_str() << "</string>\n";
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Structs to write the class type.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineClass > struct ClassType;
template <> struct ClassType < GN::Traits::Spline >
{
  template < class OutType, class StringType >
  static void write ( const StringType &indent, OutType &out )
  {
    out << indent << "<type>Spline</type>\n";
  }
};
template <> struct ClassType < GN::Traits::Curve >
{
  template < class OutType, class StringType >
  static void write ( const StringType &indent, OutType &out )
  {
    out << indent << "<class_type>Curve</class_type>\n";
  }
};
template <> struct ClassType < GN::Traits::Surface >
{
  template < class OutType, class StringType >
  static void write ( const StringType &indent, OutType &out )
  {
    out << indent << "<type>Surface</type>\n";
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Struct to write a spline to xml.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct XML
{
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineClass::TypeTag TypeTag;
  typedef typename SplineClass::BaseClass BaseClass;
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotType KnotType;
  typedef typename SplineType::ControlPointType ControlPointType;

  template < class OutType, class StringType >
  static void write ( const SplineClass &s, const StringType &indent, OutType &out )
  {
    // Start the spline.
    out << indent << "<spline>\n";

    // Needed below.
    UIntType numIndepVars ( s.numIndepVars() );
    UIntType numDepVars ( s.numDepVars() );
    UIntType dimension ( s.dimension() );
    UIntType totalCtrPts ( s.totalNumControlPoints() );
    UIntType totalKnots ( s.totalNumKnots() );
    StringType indent2 ( indent + indent );
    StringType indent3 ( indent + indent2 );
    StringType indent4 ( indent + indent3 );

    // Write the string data.
    Detail::BaseSplineClass<BaseClass>::write ( s, indent2, out );

    // Write the type.
    Detail::ClassType<TypeTag>::write ( indent2, out );

    // Write the integer data.
    out << indent2 << "<address>" << &s << "</address>\n";
    out << indent2 << "<independents>" << numIndepVars << "</independents>\n";
    out << indent2 << "<dependents>" << numDepVars << "</dependents>\n";
    out << indent2 << "<dimension>" << dimension << "</dimension>\n";
    out << indent2 << "<rational>" << ( ( s.rational() ) ? "true" : "false" ) << "</rational>\n";
    out << indent3 << "<control_points>\n";
    out << indent4 << "<num>" << totalCtrPts << "</num>\n";
    out << indent4 << "<bytes>" << ( sizeof ( ControlPointType ) ) << "</bytes>\n";
    out << indent3 << "</control_points>\n";
    out << indent3 << "<knots>\n";
    out << indent4 << "<num>" << totalKnots << "</num>\n";
    out << indent4 << "<bytes>" << ( sizeof ( KnotType ) ) << "</bytes>\n";
    out << indent3 << "</knots>\n";

    // For each independent variable...
    {
      for ( UIntType i = 0; i < numIndepVars; ++i )
      {
        // Data for this iteration.
        UIntType numKnots ( s.numKnots ( i ) );

        out << indent2 << "<independent which=\"" << i << "\">\n";
        out << indent3 << "<order>" << s.order ( i ) << "</order>\n";
        out << indent3 << "<control_points>" << s.numControlPoints ( i ) << "</control_points>\n";
        out << indent3 << "<knots count=\"" << numKnots << "\">\n";

        // Write all the knots.
        for ( UIntType j = 0; j < numKnots; ++j )
        {
          out << indent4 << "<knot i=\"" << j << "\">" << s.knot(i,j) << "</knot>\n";
        }

        out << indent3 << "</knots>\n";
        out << indent2 << "</independent>\n";
      }
    }

    // Write the control points...
    {
      out << indent2 << "<control_points>\n";
      char buffer[1024];

      // For each control point...
      for ( UIntType j = 0; j < totalCtrPts; ++j )
      {
        out << indent3 << "<point i=\"" << j << "\">";

        // Write all of the dependent variables.
        for ( UIntType i = 0; i < numDepVars; ++i )
        {
          // Note: Linux and Cygwin cannot print long double.
          ::sprintf ( buffer, "%20.15f", double ( s.controlPoint(i,j) ) );
          out << buffer;
        }

        out << "</point>\n";
      }

      out << indent2 << "</control_points>\n";
    }

    // End of the spline.
    out << indent << "</spline>\n";
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

};


///////////////////////////////////////////////////////////////////////////////
//
//  Write the spline to an XML stream.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType, class StringType, class OutType > 
inline void xml ( const SplineType &s, const StringType &indent, OutType &out )
{
  Detail::XML<SplineType>::write ( s, indent, out );
}


}; // namespace Write
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_WRITE_XML_H_
