PROBLEM_FILES = 1sp-ctp.cpp main.cpp Utils.cpp

LIBRARIES = -lgecodedriver -lgecodeflatzinc -lgecodefloat -lgecodeint -lgecodekernel -lgecodeminimodel \
			-lgecodesearch -lgecodeset -lgecodesupport

so:
	g++ -fPIC $(PROBLEM_FILES) problem_wrapper.cpp -std=c++11 -shared $(LIBRARIES) -o ../out/libgecode.so -O3 \
		-ffast-math

compile:
	g++ -std=c++11 -F/Library/Frameworks -c -o main main.cpp
	
compilegc:
	g++ -std=c++11 -I ../../../gecode-release-6.2.0/gecode main.cpp -o launch 1sp-ctp.cpp -L ../../../gecode-release-6.2.0 -lgecodekernel -lgecodesearch -lgecodeint -lgecodesupport
	
compiletmp:
	g++ -std=c++11 -I ../../../gecode-release-6.2.0/gecode main.cpp -o launch Utilities.cpp 1sp-ctp.cpp -L ../../../gecode-release-6.2.0 -lgecodekernel -lgecodesearch -lgecodeint -lgecodedriver -lgecodeminimodel -lgecodesupport
