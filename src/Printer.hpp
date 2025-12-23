#pragma once

#include "File.hpp"
#include "Squares.hpp"
#include <cstdint>
#include <ftxui/dom/elements.hpp>

namespace spsq {

class Printer {
private:
	std::string humanizeSize(uintmax_t bytes);
	ftxui::Element __debugTmpMakeBox(int dimx, int dimy);
	ftxui::Element createSquaresDom(const Squares& squares);

public:
	void print(const File& root);
};

} // namespace spsq
