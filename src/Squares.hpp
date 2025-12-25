#pragma once

#include "File.hpp"
#include <memory>
#include <vector>

namespace spsq {

enum class Direction {
	Horizontal,
	Vertical
};

struct Square {
	int width;
	int height;
	const File& file;
};

struct Squares {
	/*
		Horizontal:
		S | S | S

		Vertical:
		S
		-
		S
		-
		S
	*/
	Direction direction;
	// TODO: std::vector<std::variant<Squares, Square>> children; Type checking is so cooked
	std::vector<std::unique_ptr<Squares>> squareGroups;
	std::vector<Square> squares;
};

} // namespace spsq
