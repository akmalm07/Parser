#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <type_traits>
#include <memory>
#include <variant>
#include <functional>
#include <ranges> 
#include <algorithm> 



#define DEBUG(stuff) \
	if(false)\
	{\
		stuff \
	}

#define NDEBUG(stuff) stuff