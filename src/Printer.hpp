#pragma once

#include "File.hpp"
#include <cstdint>

namespace spsq {

class Printer {
private:
	std::string humanizeSize(uintmax_t bytes);

public:
	void print(const File& root);
};

} // namespace spsq
