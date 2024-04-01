PROBLEM_FILES = 1sp-ctp.cpp

LIBRARIES = -lgecodedriver -lgecodeflatzinc -lgecodefloat -lgecodeint -lgecodekernel -lgecodeminimodel \
			-lgecodesearch -lgecodeset -lgecodesupport

so:
	g++ -fPIC $(PROBLEM_FILES) problem_wrapper.cpp -std=c++11 -shared $(LIBRARIES) -o ../out/libgecode.so -O3 \
		-ffast-math

compile:
	g++ -std=c++11 -F/Library/Frameworks -framework gecode -o ../out/Main $(PROBLEM_FILES) Main.cpp