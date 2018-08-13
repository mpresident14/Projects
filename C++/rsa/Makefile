CXX = clang++
CXXFLAGS = -g -std=c++11 -Wall -Wextra -pedantic 

all: main

clean: 
	rm -rf *.o
	rm -rf *.exe

rsa_utils.o: rsa_utils.hpp rsa_utils.cpp
	$(CXX) $(CXXFLAGS) -c rsa_utils.cpp

rsa_encrypter.o: rsa_encrypter.hpp rsa_encrypter.cpp rsa_utils.hpp
	$(CXX) $(CXXFLAGS) -c rsa_encrypter.cpp

rsa_decrypter.o: rsa_decrypter.hpp rsa_decrypter.cpp rsa_encrypter.hpp
	$(CXX) $(CXXFLAGS) -c rsa_decrypter.cpp

server.o: server.hpp server.cpp rsa_decrypter.hpp
	$(CXX) $(CXXFLAGS) -c server.cpp

user.o: user.hpp user.cpp server.hpp
	$(CXX) $(CXXFLAGS) -c user.cpp	

main.o: main.cpp user.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

main: main.o rsa_encrypter.o rsa_decrypter.o rsa_utils.o server.o user.o
	$(CXX) $(CXXFLAGS) -pthread -o main $^