#pragma once
#include "File.hpp"
#include <string>

namespace spsq {

class Scanner {
public:
	File scan(std::string rootPath, int depth);
};

} // namespace spsq
