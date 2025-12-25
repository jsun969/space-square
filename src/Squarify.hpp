#pragma once

#include "File.hpp"
#include "Squares.hpp"

namespace spsq::squarify {

Squares squarify(const File& directory, int totalWidth, int totalHeight);

} // namespace spsq::squarify
