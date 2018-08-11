# Use "nmake"

CXX = cl 
CXXFLAGS = /W4 /EHsc /O2

all: main.exe

clean: 
	del *.obj
	del *.exe

rsa_utils.obj: rsa_utils.hpp rsa_utils.cpp
	$(CXX) $(CXXFLAGS) /c rsa_utils.cpp

rsa_encrypter.obj: rsa_encrypter.hpp rsa_encrypter.cpp rsa_utils.hpp
	$(CXX) $(CXXFLAGS) /c rsa_encrypter.cpp

rsa_decrypter.obj: rsa_decrypter.hpp rsa_decrypter.cpp rsa_encrypter.hpp
	$(CXX) $(CXXFLAGS) /c rsa_decrypter.cpp

server.obj: server.hpp server.cpp rsa_decrypter.hpp
	$(CXX) $(CXXFLAGS) /c server.cpp

user.obj: user.hpp user.cpp server.hpp
	$(CXX) $(CXXFLAGS) /c user.cpp	

main.obj: main.cpp user.hpp
	$(CXX) $(CXXFLAGS) /c main.cpp

main.exe: main.obj rsa_encrypter.obj rsa_decrypter.obj rsa_utils.obj server.obj user.obj
	link main.obj rsa_encrypter.obj rsa_decrypter.obj rsa_utils.obj server.obj user.obj /out:main.exe