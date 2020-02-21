#ifndef UNIT_TEST_HPP_INCLUDED
#define UNIT_TEST_HPP_INCLUDED 1

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception>
#include <type_traits>

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
  void assertNotEqual(const T1& obj1, const T2& obj2, size_t line)
  {
    assertTrue(obj1 != obj2, line);
  }

  template<typename T1, typename T2>
  void assertEquals(const T1& obj1, const T2& obj2, size_t line)
  {
    assertTrue(obj1 == obj2, line);
  }

  void assertFalse(bool statement, size_t line)
  {
    assertTrue(!statement, line);
  }

  void assertTrue(bool statement, size_t line)
  {
    ++affirmsInTest_;

    std::string s;
    // If we need to move forward to reach the affirm statement
    if (currentLine_ < line) {
      // Advance ifstream to the affirm line
      for (size_t i = 0; i < line - currentLine_; ++i) {
        in_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
      // Grab the affirm statement
      getline(in_, s);
      currentLine_ = line + 1;
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
              << " / " << affirmsInTest_ << " affirmations." << "\n" << std::endl;
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
  size_t currentLine_ = 1;

  std::ifstream in_;
  std::unordered_map<size_t, std::string> prevLines_;
  std::string fileName_;
};

// Macros to grab relevant values from test file
#define assertFalse(statement) UnitTest::assertFalse(statement, __LINE__)
#define assertTrue(statement) UnitTest::assertTrue(statement, __LINE__)
#define assertEquals(obj1, obj2) UnitTest::assertEquals(obj1, obj2, __LINE__)
#define assertNotEqual(obj1, obj2) UnitTest::assertNotEqual(obj1, obj2, __LINE__)
#define assertThrows(fn, errMsg) UnitTest::assertThrows(fn, errMsg, __LINE__)

#define createTester() UnitTest::createTester(__FILE__)
#define initTest() initTest(__FUNCTION__)

#endif
