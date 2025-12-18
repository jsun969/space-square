#pragma once
#include "File.hpp"
#include <cstdint>
#include <string>

namespace spsq {

class Scanner {
private:
	std::uintmax_t calcDirSize(const std::string& path);
	void propagateDirSizes(File& file);

public:
	File scan(std::string rootPath, int depth);
};

} // namespace spsq
