# UnitTest
A program to unit test your code.

Instructions for using the UnitTest class:

1. Declare a global UnitTest object with `createTest`.

2. Write each of your tests in separate functions. Begin each test by calling `initTest`. Use `affirm` to check statements that should be true. End the test with a call to the function `printResults` to get statistics about the individual test.

3. In your main function, call each of your tests. End with a call to `summarize` to get statistics about your entire test program.

See example.cpp for a sample use of the UnitTest class.
