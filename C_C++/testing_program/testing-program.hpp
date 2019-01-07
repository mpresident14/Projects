#ifndef TESTING_PROGRAM_HPP_INCLUDED
#define TESTING_PROGRAM_HPP_INCLUDED 1

#include <iostream> 
#include <fstream>
#include <string>
#include <cstddef>
#include <unordered_map>

class TestingProgram{
    public:
        TestingProgram(std::string name);
        TestingProgram() = delete;
        // Main checking function
        static void check(bool statement, size_t line, std::string file)
        {
            // Increment the total number of checks in the current test
            ++totalChecks_;

            // Need to initialize the ifstream data member with the file name the
            // first time we hit an affirm in the test file
            if (!streamInitialized){
                std::ifstream temp{file};
                in = std::move(temp);
                streamInitialized = true;
            }

            std::string s;
            // If we need to move forward to reach the affirm statement
            if (line >= currentLine){
                // Advance ifstream to the affirm line
                for (size_t i = 0; i < line - currentLine; ++i){
                    getline(in,s);
                }
                // Grab the affirm statement and save in hashmap in case it is needed later
                getline(in, s);
                prevLines[line] = s;
                currentLine = line + 1;
            }
            // If we have already gotten this line, find it in the hashmap
            else{
                s = prevLines[line];
            }

            // If the affirm statement fails
            if (!statement){
                // Increment Individual test fails
                ++fails_;                
                // Print details
                std::cerr << "FAILURE: " << file << ", line " <<  line 
                        << ": " << s << std::endl;

                // Update the total number of failed tests
                if (!alreadyMarkedFailure_){
                    alreadyMarkedFailure_ = true;
                    ++testsFailed_;
                }
            }
        }

        /// Print statistics about the individual test
        static void printResults()
        {
            std::cerr << "Result: Failed " << fails_ << " / " 
                << totalChecks_ << " affirmations." << "\n" << std::endl;
        }

        // Print statistics about the entire test program
        static void summarize()
        {
            std::cerr << "------------------------------------------------" << std::endl;
            std::cerr << "SUMMARY: Failed " << testsFailed_ << " / "
                << totalTests_ << " tests." << std::endl;

            if (testsFailed_ == 0){
                std::cerr << "Congratulations! All tests passed!" << std::endl;
            }  
            std::cerr << "------------------------------------------------" << std::endl;
        }
        
    private:
        std::string testName_;
        static size_t fails_;
        static size_t totalChecks_;
        static bool streamInitialized;
        static std::ifstream in;
        static size_t currentLine;
        static std::unordered_map <int, std::string> prevLines;
        static size_t testsFailed_;
        static size_t totalTests_;
        static bool alreadyMarkedFailure_;
};

// Retrieves the filename and line from the test file and passes
// the info to the main check function
#define affirm(statement) \
    TestingProgram::check(statement, __LINE__, __FILE__)
    
#endif