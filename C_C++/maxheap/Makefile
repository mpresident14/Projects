CXX = cl 
CXXFLAGS = /W4 /EHsc

all: test.exe

clean: 
	del *.obj
	del *.exe

test-maxheap.obj: test-maxheap.cpp maxheap.hpp maxheap-private.hpp testing-program.hpp
	$(CXX) $(CXXFLAGS) /c test-maxheap.cpp

testing-program.obj: testing-program.cpp testing-program.hpp 
	$(CXX) $(CXXFLAGS) /c testing-program.cpp

test.exe: test-maxheap.obj testing-program.obj
	link test-maxheap.obj testing-program.obj /out:test.exe


