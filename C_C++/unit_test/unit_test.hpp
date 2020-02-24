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
#include <cstring>

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
private:
  UnitTest(const char *fileName)
    :in_{fileName}, fileName_{fileName}
  {}


  void initTest(const char *testName)
  {
    // Print results of previous test.
    if (testName_) {
      printResults();
    }

    std::string stars(strlen(testName) + 4, '*');

    std::cerr << stars << std::endl;
    std::cerr << "* " << testName << " *" << std::endl;
    std::cerr << stars << std::endl;

    affirmsInTest_ = 0;
    failuresInTest_ = 0;
    alreadyMarkedFailure_ = false;
    ++totalTests_;
    testName_ = testName;
  }


  // Individual test statistics
  void printResults()
  {
    std::cerr << "Passed " << affirmsInTest_ - failuresInTest_
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

  // Per test
  size_t affirmsInTest_ = 0;
  size_t failuresInTest_ = 0;
  bool alreadyMarkedFailure_ = false;
  const char *testName_ = nullptr;

  // Per file
  size_t testsFailed_ = 0;
  size_t totalTests_ = 0;
  size_t currentLine_ = 0;

  std::ifstream in_;
  std::unordered_map<size_t, std::string> prevLines_;
  std::string fileName_;


public:
  static UnitTest createTester(const char *fileName)
  {
    return UnitTest(fileName);
  }


  ~UnitTest()
  {
    // Need print results of last test.
    printResults();
    summarize();
    delete[] testName_;
  }
  UnitTest(const UnitTest&) = delete;
  UnitTest& operator=(const UnitTest&) = delete;


  template<typename F>
  void assertThrows(const F& fn, const std::string& errMsg, size_t line, const char *testName)
  {
    try {
      fn();
      assertTrue(false, line, testName);
    } catch (std::exception& e) {
      assertTrue(std::string(e.what()) == errMsg, line, testName);
    }
  }


  template<typename T1, typename T2>
  void assertNotEqual(const T1& obj, const T2& actual, size_t line, const char *testName)
  {
    assertTrue(obj != actual, line, testName);
  }


  template<
    typename T1,
    typename T2,
    std::enable_if_t<is_printable_v<T1> && is_printable_v<T2>, int> = 0
  >
  void assertEquals(const T1& expected, const T2& actual, size_t line, const char *testName)
  {
    std::ostringstream os;
    os << "Expected " << expected << ", got " << actual;
    assertTrue(expected == actual, line, testName, os.str());
  }


  template<
    typename T1,
    typename T2,
    std::enable_if_t<!is_printable_v<T1> || !is_printable_v<T2>, int> = 0
  >
  void assertEquals(const T1& expected, const T2& actual, size_t line, const char *testName)
  {
    assertTrue(expected == actual, line, testName);
  }


  void assertFalse(bool statement, size_t line, const char *testName)
  {
    assertTrue(!statement, line, testName);
  }


  void assertTrue(bool statement, size_t line, const char *testName, std::string&& errMsg="")
  {
    if (testName != testName_) {
      initTest(testName);
    }

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
};

}

// Macros to grab relevant values from test file
#define assertFalse(statement) assertFalse(statement, __LINE__, __FUNCTION__)
#define assertTrue(statement) assertTrue(statement, __LINE__, __FUNCTION__)
#define assertEquals(expected, actual) assertEquals(expected, actual, __LINE__, __FUNCTION__)
#define assertNotEqual(obj, actual) assertNotEqual(obj, actual, __LINE__, __FUNCTION__)
#define assertThrows(fn, errMsg) assertThrows(fn, errMsg, __LINE__, __FUNCTION__)

#define createTester() createTester(__FILE__)

#endif
