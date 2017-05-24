%.o : %.cpp
	${CXX} ${CPPFLAGS} --std=c++11 ${CXXFLAGS} -c $< -o $@

imgdiff : imgdiff.o
	${CXX} ${LDFLAGS} $^ -lpng -o $@ 

