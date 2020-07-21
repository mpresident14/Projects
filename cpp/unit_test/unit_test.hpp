#ifndef UNIT_TEST_HPP_INCLUDED
#define UNIT_TEST_HPP_INCLUDED 1

#include <cstddef>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>

#include <experimental/source_location>

namespace prez {

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

template <typename T>
inline constexpr bool is_printable_v = is_printable<T>::value;

/***********************
 * Actual testing class
 ***********************/
class UnitTest {
private:
  UnitTest(const char* fileName) : in_{ fileName }, fileName_{ fileName } {}

  void initTest(const char* testName) {
    // Print results of previous test.
    if (testName_) {
      printResults();
    }

    std::string stars(strlen(testName) + 4, '*');

    std::cout << stars << std::endl;
    std::cout << "* " << testName << " *" << std::endl;
    std::cout << stars << std::endl;

    affirmsInTest_ = 0;
    failuresInTest_ = 0;
    alreadyMarkedFailure_ = false;
    ++totalTests_;
    testName_ = testName;
  }

  // Individual test statistics
  void printResults() {
    std::cout << "Passed " << affirmsInTest_ - failuresInTest_ << " / " << affirmsInTest_
              << " affirmations."
              << "\n\n"
              << std::endl;
  }

  // Entire file statistics
  void summarize() {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "SUMMARY: Passed " << totalTests_ - testsFailed_ << " / " << totalTests_
              << " tests." << std::endl;

    if (testsFailed_ == 0) {
      std::cout << "Congratulations! All tests passed!" << std::endl;
    }
    std::cout << "------------------------------------------------" << std::endl;
  }

  // Per test
  size_t affirmsInTest_ = 0;
  size_t failuresInTest_ = 0;
  bool alreadyMarkedFailure_ = false;
  const char* testName_ = nullptr;

  // Per file
  size_t testsFailed_ = 0;
  size_t totalTests_ = 0;
  size_t currentLine_ = 0;

  std::ifstream in_;
  std::unordered_map<size_t, std::string> prevLines_;
  std::string fileName_;

public:
  static UnitTest createTester(const std::experimental::source_location& location =
          std::experimental::source_location::current()) { return UnitTest(location.file_name()); }

  ~UnitTest() {
    // Need to print results of last test.
    printResults();
    summarize();
  }
  UnitTest(const UnitTest&) = delete;
  UnitTest& operator=(const UnitTest&) = delete;

  template <typename F>
  std::string assertThrows(
      const F& fn,
      const std::experimental::source_location& location =
          std::experimental::source_location::current()) {
    try {
      fn();
      assertTrue(false, location);
      return "";
    } catch (std::exception& e) {
      assertTrue(true, location);
      return e.what();
    }
  }

  template <typename F>
  void assertThrowsWithMsg(
      const F& fn,
      const std::string& errMsg,
      const std::experimental::source_location& location =
          std::experimental::source_location::current()) {
    try {
      fn();
      assertTrue(false, location);
    } catch (std::exception& e) {
      assertEquals(errMsg, std::string(e.what()), location);
    }
  }

  template <typename T1, typename T2>
  void assertNotEqual(
      const T1& obj,
      const T2& actual,
      const std::experimental::source_location& location =
          std::experimental::source_location::current()) {
    assertTrue(obj != actual, location);
  }

  template <
      typename T1,
      typename T2,
      std::enable_if_t<is_printable_v<T1> && is_printable_v<T2>, int> = 0>
  void assertEquals(
      const T1& expected,
      const T2& actual,
      const std::experimental::source_location& location =
          std::experimental::source_location::current()) {
    std::ostringstream err;
    bool b = expected == actual;
    if (!b) {
      err << "\t"
          << "EXPECTED: " << expected << '\n';
      err << "\t"
          << "GOT:      " << actual;
    }
    assertTrue(b, location, err.str());
  }

  template <
      typename T1,
      typename T2,
      std::enable_if_t<!is_printable_v<T1> || !is_printable_v<T2>, int> = 0>
  void assertEquals(
      const T1& expected,
      const T2& actual,
      const std::experimental::source_location& location =
          std::experimental::source_location::current()) {
    assertTrue(expected == actual, location);
  }

  void assertFalse(
      bool statement,
      const std::experimental::source_location& location =
          std::experimental::source_location::current()) {
    assertTrue(!statement, location);
  }

  void assertTrue(
      bool statement,
      const std::experimental::source_location& location =
          std::experimental::source_location::current(),
      std::string&& errMsg = "") {
    if (location.function_name() != testName_) {
      initTest(location.function_name());
    }

    ++affirmsInTest_;

    size_t line = location.line();
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
      std::cout << "FAILURE: " << fileName_ << ", line " << line << ": " << s << std::endl;
      if (!errMsg.empty()) {
        std::cout << errMsg << std::endl;
      }

      // Update the total number of failed tests
      if (!alreadyMarkedFailure_) {
        alreadyMarkedFailure_ = true;
        ++testsFailed_;
      }
    }
  }
};

}  // namespace prez

#endif
