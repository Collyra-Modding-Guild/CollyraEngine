#pragma once

// Dll export configuration (It will never be import here)
#ifdef COLLYRAGAMESYSTEM_CONFIG
#define COLLYRAGAMESYSTEM_API __declspec(dllexport)
#else
#define COLLYRAGAMESYSTEM_API __declspec(dllimport)
#endif

// For exporting functions that will be extern (used by the engine)
#define COLLYRAGAMESYSTEM_FACTORY extern "C" COLLYRAGAMESYSTEM_API

//Same as above, but used as a flag to identify classConstructors
#define COLLYRAGAMESYSTEM_EXPORTS COLLYRAGAMESYSTEM_FACTORY


//Engine Communication--------------
#include "../../CollObject.h"
#include "../../GameObject.h"
#include <math.h>

//Reflection Handle --------------
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor.hpp>
#include <boost/type_traits.hpp>


#define REM(...) __VA_ARGS__
#define EAT(...)

#define DETAIL_TYPEOF_INT2(tuple) DETAIL_TYPEOF_HEAD tuple
#define DETAIL_TYPEOF_INT(...) DETAIL_TYPEOF_INT2((__VA_ARGS__))

// Retrieve the type
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
#define TYPEOF(x) DETAIL_TYPEOF_INT(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
// Strip off the type
#define STRIP(x) EAT x
// Show the type without parenthesis
#define PAIR(x) REM x

struct reflector
{
	//Get field_data at index N
	template<int N, class T>
	static typename T::template field_data<N, T> get_field_data(T& x)
	{
		return typename T::template field_data<N, T>(x);
	}

	// Get the number of fields
	template<class T>
	struct fields
	{
		static const int n = T::fields_n;
	};
};

struct serializer
{
	//Get field_data at index N
	template<int N, class T>
	static typename T::template fieldI_data<N, T> get_field_data(T& x)
	{
		return typename T::template fieldI_data<N, T>(x);
	}

	// Get the number of fields
	template<class T>
	struct fields
	{
		static const int n = T::fields_in;
	};
};

template<class M, class T>
struct make_const
{
	typedef T type;
};

template<class M, class T>
struct make_const<const M, T>
{
	typedef typename boost::add_const<T>::type type;
};


#define REFLECT_EACH(r, data, i, x) \
PAIR(x); \
template<class Self> \
struct field_data<i, Self> \
{ \
    Self & self; \
    Self * selfPtr; \
    field_data(Self & self) : self(self), selfPtr(&self) {} \
    \
    typename make_const<Self, TYPEOF(x)>::type & get() \
    { \
        return self.STRIP(x); \
    }\
    typename boost::add_const<TYPEOF(x)>::type & get() const \
    { \
        return self.STRIP(x); \
    }\
    const char * name() const \
    {\
        return BOOST_PP_STRINGIZE(STRIP(x)); \
    } \
    typename make_const<Self, TYPEOF(x)>::type *  getPtr() \
    { \
        return &selfPtr->STRIP(x); \
    }\
}; \

#define SERIALIZE_EACH(r, data, i, x) \
PAIR(x); \
template<class SelfI> \
struct fieldI_data<i, SelfI> \
{ \
    Self & self; \
    Self * selfPtr; \
    field_data(Self & self) : self(self), selfPtr(&self) {} \
    \
    typename make_const<Self, TYPEOF(x)>::type & get() \
    { \
        return self.STRIP(x); \
    }\
    typename boost::add_const<TYPEOF(x)>::type & get() const \
    { \
        return self.STRIP(x); \
    }\
    const char * name() const \
    {\
        return BOOST_PP_STRINGIZE(STRIP(x)); \
    } \
    typename make_const<Self, TYPEOF(x)>::type *  getPtr() \
    { \
        return &selfPtr->STRIP(x); \
    }\
}; \

#define REFLECTABLE(...) \
static const int fields_n = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
friend struct reflector; \
template<int N, class Self> \
struct field_data {}; \
BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\

#define SERIALIZABLE(...) \
static const int fields_in = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
friend struct serializer; \
template<int N, class SelfI> \
struct fieldI_data {}; \
BOOST_PP_SEQ_FOR_EACH_I(SERIALIZE_EACH, dataI, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

struct field_visitor
{
	template<class C, class Visitor, class I>
	void operator()(C& c, Visitor v, I)
	{
		v(reflector::get_field_data<I::value>(c));
	}
};

struct fieldI_visitor
{
	template<class C, class Visitor, class I>
	void operator()(C& c, Visitor v, I)
	{
		v(serializer::get_field_data<I::value>(c));
	}
};

template<class C, class Visitor>
void visit_each(C& c, Visitor v)
{
	typedef boost::mpl::range_c<int, 0, reflector::fields<C>::n> range;
	boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), v, _1));
}

template<class C, class Visitor>
void visitI_each(C& c, Visitor v)
{
	typedef boost::mpl::range_c<int, 0, serializer::fields<C>::n> range;
	boost::mpl::for_each<range>(boost::bind<void>(fieldI_visitor(), boost::ref(c), v, _1));
}