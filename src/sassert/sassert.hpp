// https://github.com/jonathanpoelen/super-assert.git

#ifndef SASSERT_HPP
#define SASSERT_HPP

#ifdef NDEBUG

#define sassert_message(msg, expr)
#define sassert(expr)

#else

#include <string>
#include <iostream>
#include <type_traits>

#ifndef NO_BOOST
# include <boost/current_function.hpp>
#endif

namespace Super_Assert {

#if defined(SASSERT_COLOR) && !defined(SASSERT_NO_COLOR)
//TODO SASSERT_DYNAMIC_COLOR (read file configuration)
#  ifndef SASSERT_COLOR_CHARACTER
#  define SASSERT_COLOR_CHARACTER "\033[00;33m"
#  endif
#  ifndef SASSERT_COLOR_FLOATING
#    define SASSERT_COLOR_FLOATING "\033[00;32m"
#  endif
#  ifndef SASSERT_COLOR_INTEGRAL
#    define SASSERT_COLOR_INTEGRAL "\033[00;34m"
#  endif
#  ifndef SASSERT_COLOR_BOOLEAN
#    define SASSERT_COLOR_BOOLEAN "\033[00;36m"
#  endif
#  ifndef SASSERT_COLOR_STRING
#    define SASSERT_COLOR_STRING "\033[00;31m"
#  endif
#  ifndef SASSERT_COLOR_OTHER
#    define SASSERT_COLOR_OTHER "\033[00;35m"
#  endif
#  ifndef SASSERT_COLOR_UNKNOW
#    define SASSERT_COLOR_UNKNOW "\033[00;36m"
#  endif
#  ifndef SASSERT_COLOR_SYMBOLE
#    define SASSERT_COLOR_SYMBOLE "\033[00;35m"
#  endif
#  ifndef SASSERT_COLOR_EXPR
#    define SASSERT_COLOR_EXPR "\033[01;30m"
#  endif
#  ifndef SASSERT_COLOR_MSG
#    define SASSERT_COLOR_MSG "\033[01;31m"
#  endif
#  ifndef SASSERT_COLOR_FILE
#    define SASSERT_COLOR_FILE "\033[01;31m"
#  endif
#  ifndef SASSERT_COLOR_LINE
#    define SASSERT_COLOR_LINE "\033[00;33m"
#  endif
#  ifndef SASSERT_COLOR_FUNCTION
#    define SASSERT_COLOR_FUNCTION "\033[01;34m"
#  endif
#  ifndef SASSERT_COLOR_RESET
#    define SASSERT_COLOR_RESET "\033[00m"
#  endif
#else
#  define SASSERT_COLOR_CHARACTER ""
#  define SASSERT_COLOR_FLOATING ""
#  define SASSERT_COLOR_INTEGRAL ""
#  define SASSERT_COLOR_BOOLEAN ""
#  define SASSERT_COLOR_STRING ""
#  define SASSERT_COLOR_OTHER ""
#  define SASSERT_COLOR_UNKNOW ""
#  define SASSERT_COLOR_SYMBOLE ""
#  define SASSERT_COLOR_EXPR ""
#  define SASSERT_COLOR_MSG ""
#  define SASSERT_COLOR_FILE ""
#  define SASSERT_COLOR_LINE ""
#  define SASSERT_COLOR_FUNCTION ""
#  define SASSERT_COLOR_RESET ""
#endif


template<typename T> struct is_character : std::false_type {};
template<> struct is_character<signed char> : std::true_type {};
template<> struct is_character<unsigned char> : std::true_type {};
template<> struct is_character<char> : std::true_type {};
template<> struct is_character<wchar_t> : std::true_type {};
template<> struct is_character<char16_t> : std::true_type {};
template<> struct is_character<char32_t> : std::true_type {};

template<class BasicString> struct is_basic_string : std::false_type {};
template<class CharT, class Trait, class Allocator>
struct is_basic_string<std::basic_string<CharT, Trait, Allocator> >
: std::true_type {};

template<class T> struct is_string : is_basic_string<T> {};
template<class T> struct is_string<T*> : is_character<T> {};
template<class T, std::size_t N> struct is_string<T[N]> : is_character<T> {};

template<class T, class TrueClass, class FalseClass>
using eval_if_c_t = typename std::conditional<T::value, TrueClass, FalseClass>::type::type;


enum class PrinterType
{
  integral,
  floating_point,
  boolean,
  string,
  character,
  other
};

template<PrinterType val>
struct printer_type
{
  static constexpr PrinterType value = val;
  typedef printer_type type;
};

using std::to_string;

template<typename T, PrinterType =
eval_if_c_t<is_character<T>, printer_type<PrinterType::character>,
eval_if_c_t<std::is_same<T, bool>, printer_type<PrinterType::boolean>,
eval_if_c_t<std::is_integral<T>, printer_type<PrinterType::integral>,
eval_if_c_t<std::is_floating_point<T>, printer_type<PrinterType::floating_point>,
eval_if_c_t<is_string<T>, printer_type<PrinterType::string>,
printer_type<PrinterType::other>
>>>>>::value>
struct Printer
{
  static void print(const T& x)
  { dispatch_print(x, 0); }

  template<typename U>
  static auto dispatch_print(const U& x, int)
  -> decltype(std::cerr << x)
  { return std::cerr << SASSERT_COLOR_OTHER "{ " << x << " }" SASSERT_COLOR_RESET; }

  template<typename U>
  static void dispatch_print(const U& x, unsigned)
  { dispatch_with_to_string(x, 0); }

  template<typename U>
  static auto dispatch_with_to_string(const U& x, int)
  -> decltype(std::cerr << to_string(x))
  { return std::cerr << SASSERT_COLOR_STRING "\"" << to_string(x) << "\"" SASSERT_COLOR_RESET; }

  template<typename U>
  static void dispatch_with_to_string(const U&, unsigned)
  { std::cerr << SASSERT_COLOR_UNKNOW "<unknow>" SASSERT_COLOR_RESET; }
};

template<typename T>
struct Printer<T, PrinterType::integral>
{
  static void print(const T& x)
  { std::cerr << "" SASSERT_COLOR_INTEGRAL << x << "" SASSERT_COLOR_RESET; }
};

template<typename T>
struct Printer<T, PrinterType::floating_point>
{
  static void print(const T& x)
  { std::cerr << "" SASSERT_COLOR_FLOATING << x << "" SASSERT_COLOR_RESET; }
};

template<typename T>
struct Printer<T, PrinterType::string>
{
  static void print(const T& x)
  { dispatch_print(x, 0); }

  static auto dispatch_print(const T& x, int)
  -> decltype(std::cerr << x)
  { return std::cerr << SASSERT_COLOR_STRING "\"" << x << "\"" SASSERT_COLOR_RESET; }

  static void dispatch_print(const T& x, unsigned)
  { std::wcerr << SASSERT_COLOR_STRING "\"" << x << "\"" SASSERT_COLOR_RESET; }
};

template<typename T>
struct Printer<T, PrinterType::character>
{
  static void print(char x)
  {
    if (x == std::char_traits<char>::to_char_type('\'')) {
      std::cerr << SASSERT_COLOR_CHARACTER"'\''" SASSERT_COLOR_RESET;
    }
    else {
      std::cerr << SASSERT_COLOR_CHARACTER "'" << x << "'" SASSERT_COLOR_RESET;
    }
  }

  template<typename U>
  static void print(const U& x)
  {
    if (x == std::char_traits<T>::to_char_type('\'')) {
      std::wcerr << SASSERT_COLOR_CHARACTER"'\''" SASSERT_COLOR_RESET;
    }
    else {
      std::wcerr << SASSERT_COLOR_CHARACTER "'" << x << "'" SASSERT_COLOR_RESET;
    }
  }
};

template<typename T>
struct Printer<T, PrinterType::boolean>
{
  static void print(const T& x)
  {
    if (x) {
      std::cerr << SASSERT_COLOR_BOOLEAN "true" SASSERT_COLOR_RESET;
    }
    else {
      std::cerr << SASSERT_COLOR_BOOLEAN "false" SASSERT_COLOR_RESET;
    }
  }
};


template<typename T = void, typename U = void>
struct SAssert
{
  T l;
  const char * op;
  U r;

  void print() const
  {
    print(l);
    std::cerr << " " SASSERT_COLOR_SYMBOLE << op << SASSERT_COLOR_RESET " ";
    print(r);
  }

  template<typename P>
  void print(const P& p) const
  { Printer<P>::print(p); }

  template<typename TT, typename UU>
  void print(const SAssert<TT,UU>& a) const
  { a.print(); }

  template<typename R>
  SAssert<const SAssert&, const R&> operator=(const R& x)
  { return {*this, "=", x}; }
};

template<>
struct SAssert<void,void>
{};

template<typename T>
struct SAssert<T,void>
{
  T l;

  void print() const
  { Printer<T>::print(l); }

  template<typename U>
  SAssert<T, const U&> operator=(const U& x)
  { return {l, "=", x}; }
};

template<typename T>
SAssert<const T&>
operator*(const SAssert<>&, const T& x)
{ return {x}; }

constexpr SAssert<> B;

#define MOP(op)\
template<typename T, typename U>\
SAssert<const T&, const U&>\
operator op(const SAssert<T>& a, const U& x)\
{ return {a.l, #op, x}; }\
\
template<typename T, typename U, typename R>\
SAssert<const SAssert<T,U>&, const R&>\
operator op(const SAssert<T,U>& a, const R& x)\
{ return {a, #op, x}; }

MOP(&)
MOP(^)
MOP(|)
MOP(||)
MOP(&&)
MOP(*)
MOP(+)
MOP(-)
MOP(/)
MOP(%)
MOP(==)
MOP(!=)
MOP(<)
MOP(<=)
MOP(>)
MOP(>=)

#undef MOP

struct null_pointer { void ** p = nullptr; };
}

//BOOST_PP_CAT
#define PP_CAT_I(e) #e
#define PP_CAT(e) PP_CAT_I(e)

#ifndef NO_BOOST
#  define SASSERT_FUNCTION BOOST_CURRENT_FUNCTION
#elif defined __GNU && defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#  define SASSERT_FUNCTION __PRETTY_FUNCTION__
#else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#    define SASSERT_FUNCTION __func__
#  else
#    define SASSERT_FUNCTION ""
#  endif
# endif

#define sassert_fail_(expr)                                                         \
    std::cerr << SASSERT_COLOR_FILE __FILE__ SASSERT_COLOR_RESET                    \
    ":" SASSERT_COLOR_LINE PP_CAT(__LINE__) SASSERT_COLOR_RESET                     \
    ":\n" SASSERT_COLOR_FUNCTION << SASSERT_FUNCTION << SASSERT_COLOR_RESET         \
    ":\nAssertion `" SASSERT_COLOR_EXPR PP_CAT(expr) SASSERT_COLOR_RESET "` failed" \
     "\n           ";                                                               \
    (Super_Assert::B * expr).print();                                               \
    std::cerr << std::endl;                                                         \
    *Super_Assert::null_pointer().p = 0;

#define sassert_message(msg, expr) do {                                   \
  if ((expr) ? 0 : 1) {                                                   \
    std::ios_base::sync_with_stdio(1);                                    \
    std::cerr << SASSERT_COLOR_MSG << (msg) << SASSERT_COLOR_RESET ":\n"; \
    sassert_fail_(expr)                                                   \
  }                                                                       \
} while(0)

#define sassert(expr) do {             \
  if ((expr) ? 0 : 1) {                \
    std::ios_base::sync_with_stdio(1); \
    sassert_fail_(expr)                \
  }                                    \
} while(0)

#endif

#endif
