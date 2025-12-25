#include "Squarify.hpp"
#include "File.hpp"
#include "Squares.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace spsq::squarify {

Squares squarify(const File& directory, int totalWidth, int totalHeight) {
	// Expected square aspect ratio (width / height)
	// 2 because typical terminal characters are about twice as tall as they are wide
	constexpr double RATIO = 2;

	const auto& files = directory.children;
	const double totalSize = static_cast<double>(directory.sizeBytes);
	const double totalArea = static_cast<double>(totalWidth) * static_cast<double>(totalHeight);
	auto getAreaFromSize = [&](uintmax_t sizeBytes) {
		double sizeRatio = static_cast<double>(sizeBytes) / totalSize;
		return sizeRatio * totalArea;
	};

	Squares squaresResult;

	Squares *outerSquare = nullptr, *nextOuterSquare = &squaresResult;
	double width = static_cast<double>(totalWidth), height = static_cast<double>(totalHeight);
	size_t i = 0;
	while (i < files.size()) {
		outerSquare = nextOuterSquare;

		Direction outerDirection;
		double fixedSide;
		if (width < height) {
			outerDirection = Direction::Vertical;
			fixedSide = width;
		} else {
			outerDirection = Direction::Horizontal;
			fixedSide = height;
		}

		uintmax_t curSize = 0;
		std::vector<const File*> curFileRow;
		size_t j;
		double prevAspectRatio = 0, dynSide = 0;
		for (j = i; j < files.size(); ++j) {
			auto newSize = curSize + files[j].sizeBytes;
			auto newArea = getAreaFromSize(newSize);
			auto newDynSide = newArea / fixedSide;
			// width / height
			double aspectRatio = outerDirection == Direction::Vertical ? fixedSide / newDynSide : newDynSide / fixedSide;
			if (j > i) {
				// Check if aspect ratio is getting worse
				if (std::abs(aspectRatio - RATIO) > std::abs(prevAspectRatio - RATIO)) {
					break;
				}
			}
			curSize = newSize;
			dynSide = newDynSide;
			curFileRow.push_back(&files[j]);
			prevAspectRatio = aspectRatio;
		}
		// Split squares
		auto splittedSquare = std::make_unique<Squares>();
		auto setSplittedSquares = [&](std::function<double(double)> getWidth, std::function<double(double)> getHeight) {
			for (const auto& file : curFileRow) {
				auto area = getAreaFromSize(file->sizeBytes);
				// TODO: Count file with 0 area and print ignored squares
				splittedSquare->squares.push_back({ .width = static_cast<int>(getWidth(area)),
																						.height = static_cast<int>(getHeight(area)),
																						.file = *file });
			}
		};
		if (outerDirection == Direction::Horizontal) {
			if (fixedSide < dynSide) {
				splittedSquare->direction = Direction::Horizontal;
				setSplittedSquares(
						[&](double area) { return area / fixedSide; }, [&](double area) { return fixedSide; });
			} else {
				splittedSquare->direction = Direction::Vertical;
				setSplittedSquares(
						[&](double area) { return dynSide; }, [&](double area) { return area / dynSide; });
			}
			width -= dynSide;
		} else {
			if (fixedSide > dynSide) {
				splittedSquare->direction = Direction::Horizontal;
				setSplittedSquares(
						[&](double area) { return area / dynSide; }, [&](double area) { return dynSide; });
			} else {
				splittedSquare->direction = Direction::Vertical;
				setSplittedSquares(
						[&](double area) { return fixedSide; }, [&](double area) { return area / fixedSide; });
			}
			height -= dynSide;
		}

		/*
		 *        Outer
		 *        /    \
		 *   Splitted  Blank
		 *    /  |  \
		 *  File Squares
		 *
		 * Then blank will be outer for the next iteration
		 */

		// Split outer square with splitted square and blank square
		outerSquare->direction = outerDirection;
		auto blankSquare = std::make_unique<Squares>();
		outerSquare->squareGroups.push_back(std::move(splittedSquare));
		nextOuterSquare = blankSquare.get();
		outerSquare->squareGroups.push_back(std::move(blankSquare));

		i = j;
	}

	// For the last outer square, remove the blank square, set it to be splitted square
	if (outerSquare && !outerSquare->squareGroups.empty()) {
		std::unique_ptr<Squares> splittedSquare = std::move(outerSquare->squareGroups.front());
		outerSquare->direction = splittedSquare->direction;
		outerSquare->squares = std::move(splittedSquare->squares);
		outerSquare->squareGroups.clear();
	}

	return squaresResult;
}

} // namespace spsq::squarify
