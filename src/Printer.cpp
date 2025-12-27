#include "Printer.hpp"
#include "File.hpp"
#include "Squares.hpp"
#include "Squarify.hpp"
#include "fmt/format.h"
#include <fmt/base.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace spsq::printer {
namespace {

std::string humanizeSize(uintmax_t bytes) {
	constexpr int SUFFIX_CNT = 6, BASE = 1024;
	constexpr std::array<std::string_view, SUFFIX_CNT> SUFFIXES { "KB", "MB", "GB", "TB", "PB", "EB" };
	if (bytes < BASE) {
		return fmt::format("{} B", bytes);
	}
	double size = static_cast<double>(bytes) / BASE;
	int unit = 0;
	for (; size >= BASE && unit < SUFFIX_CNT - 1; ++unit) {
		size /= BASE;
	}
	return fmt::format("{:.1f} {}", size, SUFFIXES[unit]);
}

// Forward declaration for mutual recursion
ftxui::Element createSquaresDom(const Squares& squares);

ftxui::Element createFileSquareDom(const Square& square) {
	using namespace ftxui;

	// TODO: Improve UI
	auto FileComp = [](const std::string& title) {
		return border(vcenter(paragraphAlignCenter(title)));
	};
	auto FolderComp = [](const std::string& title, const Element& children) {
		return window(text(title), children);
	};

	auto filesize = humanizeSize(square.file.sizeBytes);
	if (square.file.type == FileType::Directory) {
		auto title = fmt::format("{} ({})", square.file.name, filesize);
		// Recursively squarify and render children inside the folder
		int innerWidth = square.width - 2;
		int innerHeight = square.height - 2;
		Element childrenDom = hbox({});
		if (innerWidth > 0 && innerHeight > 0 && !square.file.children.empty()) {
			// Create a temporary parent file to squarify children
			File tempParent = square.file;
			auto childSquares = spsq::squarify::squarify(tempParent, innerWidth, innerHeight);
			childrenDom = createSquaresDom(childSquares);
		}
		return FolderComp(title, childrenDom) | size(WIDTH, EQUAL, square.width) | size(HEIGHT, EQUAL, square.height);
	} else {
		auto title = fmt::format("{}\n{}", square.file.name, filesize);
		return FileComp(title) | size(WIDTH, EQUAL, square.width) | size(HEIGHT, EQUAL, square.height);
	}
}

ftxui::Element createSquaresDom(const Squares& squares) {
	using namespace ftxui;

	std::vector<ftxui::Element> children;
	if (!squares.squares.empty()) {
		for (const auto& square : squares.squares) {
			children.push_back(createFileSquareDom(square));
		}
	} else {
		for (const auto& group : squares.squareGroups) {
			children.push_back(createSquaresDom(*group));
		}
	}
	if (squares.direction == Direction::Horizontal) {
		return hbox(children);
	} else {
		return vbox(children);
	}
}

} // namespace

void print(const File& root) {
	using namespace ftxui;

	auto terminalSize = Terminal::Size();
	auto squares = spsq::squarify::squarify(root, terminalSize.dimx, terminalSize.dimy);
	// DEBUG
	// 	std::function<void(const Squares&, int)> printSquares = [&](const Squares& sq, int indent) {
	// 		std::string pad(indent * 2, ' ');
	// 		auto dirStr = sq.direction == Direction::Horizontal ? "Horizontal" : "Vertical";
	// 		fmt::println("{}Group ({})", pad, dirStr);
	//
	// 		if (!sq.squares.empty()) {
	// 			for (const auto& s : sq.squares) {
	// 				fmt::println("{}  Square: {}x{} ({})", pad, s.width, s.height, s.file.name);
	// 			}
	// 		} else {
	// 			for (const auto& group : sq.squareGroups) {
	// 				printSquares(*group, indent + 1);
	// 			}
	// 		}
	// 	};
	// 	printSquares(squares, 0);
	// DEBUG
	auto dom = createSquaresDom(squares);

	auto screen = ScreenInteractive::Fullscreen();
	auto mainComp = Renderer([&]() { return dom; });
	screen.Loop(mainComp);
}

} // namespace spsq::printer
