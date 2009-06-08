
#include <iostream>
#include <limits>
#include <vector>
#include <list>
#include <map>


template < class T > struct A
{
  A() : _t(){}
  T _t;
};


template < class T > class B : A < T >
{
  B() : A < T >(){}
};


template < class T > struct C
{
  C() : _t(){}
  virtual ~C(){}
  T _t;
};


template < class T > struct D : C < T >
{
  D() : C < T >(){}
  virtual ~D(){}
};


template < class T > struct SizeInfo
{
  static void print()
  {
    std::cout << "T ...................... " << sizeof ( T )    << std::endl;
    std::cout << "A<T> ................... " << sizeof ( A<T> ) << std::endl;
    std::cout << "B<T> ................... " << sizeof ( B<T> ) << std::endl;
    std::cout << "C<T> ................... " << sizeof ( C<T> ) << std::endl;
    std::cout << "D<T> ................... " << sizeof ( D<T> ) << std::endl;

    std::cout << "std::vector of T ....... " << sizeof ( std::vector < T > )    << std::endl;
    std::cout << "std::vector of A<T> .... " << sizeof ( std::vector < A<T> > ) << std::endl;
    std::cout << "std::vector of B<T> .... " << sizeof ( std::vector < B<T> > ) << std::endl;
    std::cout << "std::vector of C<T> .... " << sizeof ( std::vector < C<T> > ) << std::endl;
    std::cout << "std::vector of D<T> .... " << sizeof ( std::vector < D<T> > ) << std::endl;

    std::cout << "std::list of T ......... " << sizeof ( std::list < T > )    << std::endl;
    std::cout << "std::list of A<T> ...... " << sizeof ( std::list < A<T> > ) << std::endl;
    std::cout << "std::list of B<T> ...... " << sizeof ( std::list < B<T> > ) << std::endl;
    std::cout << "std::list of C<T> ...... " << sizeof ( std::list < C<T> > ) << std::endl;
    std::cout << "std::list of D<T> ...... " << sizeof ( std::list < D<T> > ) << std::endl;

    std::cout << "std::map of T .......... " << sizeof ( std::map < T, T > )    << std::endl;
    std::cout << "std::map of A<T> ....... " << sizeof ( std::map < T, A<T> > ) << std::endl;
    std::cout << "std::map of B<T> ....... " << sizeof ( std::map < T, B<T> > ) << std::endl;
    std::cout << "std::map of C<T> ....... " << sizeof ( std::map < T, C<T> > ) << std::endl;
    std::cout << "std::map of D<T> ....... " << sizeof ( std::map < T, D<T> > ) << std::endl;

    std::cout << std::endl;
  }
};


int main ( int argc, char ** argv )
{
  std::cout << "unsigned char"      << std::endl; SizeInfo < unsigned char      >::print();
  std::cout << "unsigned short"     << std::endl; SizeInfo < unsigned short     >::print();
  std::cout << "unsigned int"       << std::endl; SizeInfo < unsigned int       >::print();
  std::cout << "unsigned long"      << std::endl; SizeInfo < unsigned long      >::print();
  std::cout << "unsigned long int"  << std::endl; SizeInfo < unsigned long int  >::print();
  std::cout << "unsigned long long" << std::endl; SizeInfo < unsigned long long >::print();

  std::cout << "char"      << std::endl; SizeInfo < char      >::print();
  std::cout << "short"     << std::endl; SizeInfo < short     >::print();
  std::cout << "int"       << std::endl; SizeInfo < int       >::print();
  std::cout << "long"      << std::endl; SizeInfo < long      >::print();
  std::cout << "long int"  << std::endl; SizeInfo < long int  >::print();
  std::cout << "long long" << std::endl; SizeInfo < long long >::print();

  std::cout << "float"       << std::endl; SizeInfo < float       >::print();
  std::cout << "double"      << std::endl; SizeInfo < double      >::print();
  std::cout << "long double" << std::endl; SizeInfo < long double >::print();

  //std::cin.get();

  return 0;
}
