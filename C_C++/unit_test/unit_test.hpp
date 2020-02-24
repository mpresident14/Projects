#ifndef UNIT_TEST_HPP_INCLUDED
#define UNIT_TEST_HPP_INCLUDED 1

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <exception>
#include <type_traits>

namespace mcprez
{

/*******************************************************
 * Check to see if a type can use the ostream operator
 ******************************************************/
template <typename T>
struct is_printable {
private:
  template <typename TT>
  static auto test(bool)
    -> decltype(std::declval<std::ostream&>() << std::declval<TT>(), std::true_type());

  template <typename>
  static auto test(int) -> std::false_type;

public:
  // Passing in true tries the bool parameter overload first.
  static constexpr bool value = decltype(is_printable::test<T>(true))::value;
};

template<typename T>
inline constexpr bool is_printable_v = is_printable<T>::value;


/***********************
 * Actual testing class
 ***********************/
class UnitTest {
public:
  static UnitTest createTester(std::string fileName)
  {
    return UnitTest(std::move(fileName));
  }


  void initTest(std::string testName)
  {
    std::string stars(testName.length() + 4, '*');

    std::cerr << stars << std::endl;
    std::cerr << "* " << testName << " *" << std::endl;
    std::cerr << stars << std::endl;

    affirmsInTest_ = 0;
    failuresInTest_ = 0;
    alreadyMarkedFailure_ = false;
    ++totalTests_;
  }


  template<typename F>
  void assertThrows(const F& fn, const std::string& errMsg, size_t line)
  {
    try {
      fn();
      assertTrue(false, line);
    } catch (std::exception& e) {
      assertTrue(std::string(e.what()) == errMsg, line);
    }
  }


  template<typename T1, typename T2>
  void assertNotEqual(const T1& obj, const T2& actual, size_t line)
  {
    assertTrue(obj != actual, line);
  }


  template<
    typename T1,
    typename T2,
    std::enable_if_t<is_printable_v<T1> && is_printable_v<T2>, int> = 0
  >
  void assertEquals(const T1& expected, const T2& actual, size_t line)
  {
    std::ostringstream os;
    os << "Expected " << expected << ", got " << actual;
    assertTrue(expected == actual, line, os.str());
  }


  template<
    typename T1,
    typename T2,
    std::enable_if_t<!is_printable_v<T1> || !is_printable_v<T2>, int> = 0
  >
  void assertEquals(const T1& expected, const T2& actual, size_t line)
  {
    assertTrue(expected == actual, line);
  }


  void assertFalse(bool statement, size_t line)
  {
    assertTrue(!statement, line);
  }


  void assertTrue(bool statement, size_t line, std::string&& errMsg="")
  {
    ++affirmsInTest_;

    std::string s;
    // If we need to move forward to reach the affirm statement
    if (currentLine_ < line) {
      // Advance ifstream to the affirm line
      for (size_t i = 0; i < line - currentLine_ - 1; ++i) {
        in_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
      // Grab the affirm statement
      getline(in_, s);
      currentLine_ = line;
      prevLines_[line] = s;
    }
    // If we have already gotten this line (e.g. affirm() in a loop)
    else {
      s = prevLines_[line];
    }

    if (!statement) {
      ++failuresInTest_;
      std::cerr << "FAILURE: " << fileName_ << ", line " << line << ": " << s
                << std::endl;
      if (!errMsg.empty()) {
        std::cerr << '\t' << errMsg << std::endl;
      }

      // Update the total number of failed tests
      if (!alreadyMarkedFailure_) {
        alreadyMarkedFailure_ = true;
        ++testsFailed_;
      }
    }
  }


  // Individual test statistics
  void printResults()
  {
    std::cerr << "Result: Passed " << affirmsInTest_ - failuresInTest_
              << " / " << affirmsInTest_ << " affirmations." << "\n\n" << std::endl;
  }


  // Entire file statistics
  void summarize()
  {
    std::cerr << "------------------------------------------------" << std::endl;
    std::cerr << "SUMMARY: Passed " << totalTests_ - testsFailed_
              << " / " << totalTests_ << " tests." << std::endl;

    if (testsFailed_ == 0) {
      std::cerr << "Congratulations! All tests passed!" << std::endl;
    }
    std::cerr << "------------------------------------------------" << std::endl;
  }


private:
  UnitTest(std::string fileName)
    :in_{fileName}, fileName_{fileName}
  {}

  // Per test
  size_t affirmsInTest_ = 0;
  size_t failuresInTest_ = 0;
  bool alreadyMarkedFailure_ = false;

  // Per file
  size_t testsFailed_ = 0;
  size_t totalTests_ = 0;
  size_t currentLine_ = 0;

  std::ifstream in_;
  std::unordered_map<size_t, std::string> prevLines_;
  std::string fileName_;
};

}

// Macros to grab relevant values from test file
#define assertFalse(statement) assertFalse(statement, __LINE__)
#define assertTrue(statement) assertTrue(statement, __LINE__)
#define assertEquals(expected, actual) assertEquals(expected, actual, __LINE__)
#define assertNotEqual(obj, actual) assertNotEqual(obj, actual, __LINE__)
#define assertThrows(fn, errMsg) assertThrows(fn, errMsg, __LINE__)

#define createTester() createTester(__FILE__)
#define initTest() initTest(__FUNCTION__)

#endif
