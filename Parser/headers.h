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


#ifdef DEBUG
constexpr bool debug = true;
#elif defined (NDEBUG)
constexpr bool debug = false;
#else
constexpr bool debug = false; 
#endif


#define DEBUG(stuff) \
	if(debug)\
	{\
		stuff \
	}

#define NDEBUG(stuff) stuff