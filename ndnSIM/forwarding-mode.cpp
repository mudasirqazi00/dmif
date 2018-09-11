#ifndef FORWARDING_MODE_CPP
#define FORWARDING_MODE_CPP

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class ForwardingMode {
public:
	enum {
		Flooding = 1,
		Directive = 2
	};
};
#endif
