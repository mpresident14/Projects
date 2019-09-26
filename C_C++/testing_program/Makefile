CXX = cl 
CXXFLAGS = /W4 /EHsc /c

all: example.exe

clean: 
	del *.obj
	del *.exe

example.obj: example.cpp testing-program.hpp
	$(CXX) $(CXXFLAGS) example.cpp

testing-program.obj: testing-program.cpp testing-program.hpp 
	$(CXX) $(CXXFLAGS) testing-program.cpp

example.exe: example.obj testing-program.obj
	link example.obj testing-program.obj /out:example.exe

