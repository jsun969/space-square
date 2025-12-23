#pragma once

#include "File.hpp"
#include <string>

namespace spsq::scanner {

File scan(std::string rootPath, int depth);

} // namespace spsq::scanner
