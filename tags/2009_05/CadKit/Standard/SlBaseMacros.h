
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlBaseMacros.h: Macros used in classes that inherit from SlBase.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_BASE_MACROS_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_BASE_MACROS_CLASS_H_


///////////////////////////////////////////////////////////////////////////////
//
// Call this macro to obtain the static instance of the runtime
// class associated with the "class_name" you give it.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_CLASS_TYPE(class_name) (&class_name::type##class_name)


///////////////////////////////////////////////////////////////////////////////
//
// Call this macro to obtain the static instance of the class
// id associated with the "class_name" you give it.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_CLASS_ID(class_name) (class_name::typeId##class_name)


///////////////////////////////////////////////////////////////////////////////
//
// Put this macro at the bottom of your class definition. It will give your
// class a static instance of SlType and declare a function
// to obtain it (which you can use instead of the "SL_CLASS_TYPE"
// macro. The given 32 bit id should be unique.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_DECLARE_CLASS(class_name,id) \
public: \
  static  const SlType type##class_name; \
  enum { typeId##class_name = id }; \
  static  const SlType * getBaseClassType(); \
  static  const SlType * getClassType(); \
  virtual const SlType * getType() const; \
  static  SlUint32       getClassTypeId(); \
  virtual SlUint32       getTypeId() const


///////////////////////////////////////////////////////////////////////////////
//
// Put this macro at the bottom of your class definition. It will give your
// class everything that "SL_DECLARE_CLASS" does and also a static member
// function to declare an instance of your class. Your class must have a 
// default constructor (i.e., takes no parameters) to use this macro.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_DECLARE_DYNAMIC_CLASS(class_name,id) \
  SL_DECLARE_CLASS(class_name,id); \
  static SlBase *createObject()


///////////////////////////////////////////////////////////////////////////////
//
// This macro is used by the following two macros. It's the declaration
// of the static data member of SlType.
//
///////////////////////////////////////////////////////////////////////////////

#define _SL_IMPLEMENT_CLASS_TYPE_INFO(class_name, base_class_name, create_function) \
  const SlType class_name::type##class_name =   { #class_name, create_function, SL_CLASS_TYPE(base_class_name) }; \
  const SlType *class_name::getBaseClassType()  { return SL_CLASS_TYPE(base_class_name); } \
  const SlType *class_name::getClassType()      { return SL_CLASS_TYPE(class_name); } \
  const SlType *class_name::getType() const     { return SL_CLASS_TYPE(class_name); } \
  SlUint32 class_name::getClassTypeId()         { return SL_CLASS_ID(class_name); } \
  SlUint32 class_name::getTypeId() const        { return SL_CLASS_ID(class_name); }


///////////////////////////////////////////////////////////////////////////////
//
// Put this macro at the top of your implementation file. 
// It goes along with "SL_DECLARE_CLASS"
//
///////////////////////////////////////////////////////////////////////////////

#define SL_IMPLEMENT_CLASS(class_name, base_class_name) \
  _SL_IMPLEMENT_CLASS_TYPE_INFO(class_name, base_class_name, 0x0)


///////////////////////////////////////////////////////////////////////////////
//
// Put this macro at the top of your implementation file. 
// It goes along with "SL_DECLARE_DYNAMIC_CLASS"
//
///////////////////////////////////////////////////////////////////////////////

#define SL_IMPLEMENT_DYNAMIC_CLASS(class_name, base_class_name) \
  SlBase *class_name::createObject() { return new class_name; } \
  _SL_IMPLEMENT_CLASS_TYPE_INFO(class_name, base_class_name, class_name::createObject)


#endif // _CADKIT_STANDARD_LIBRARY_BASE_MACROS_CLASS_H_
