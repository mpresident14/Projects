# Use "nmake"

CXX = cl 
CXXFLAGS = /W4 /EHsc

all: cribbage.exe

clean: 
	del *.obj
	del *.exe

card.obj: card.cpp card.hpp
	$(CXX) $(CXXFLAGS) /c card.cpp

player.obj: player.cpp player.hpp card.hpp
	$(CXX) $(CXXFLAGS) /c player.cpp

pegger.obj: pegger.cpp pegger.hpp card.hpp
	$(CXX) $(CXXFLAGS) /c pegger.cpp

cribbage.obj: cribbage.cpp pegger.hpp player.hpp card.hpp
	$(CXX) $(CXXFLAGS) /c cribbage.cpp

player.exe: player.obj card.obj
	link player.obj card.obj /out:player.exe

pegger.exe: pegger.obj card.obj
	link pegger.obj card.obj /out:pegger.exe

cribbage.exe: cribbage.obj player.obj pegger.obj card.obj
	link cribbage.obj pegger.obj player.obj card.obj /out:cribbage.exe