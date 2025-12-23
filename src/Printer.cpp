#include "Printer.hpp"
#include "File.hpp"
#include "Squares.hpp"
#include "fmt/format.h"
#include <fmt/base.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <vector>

namespace spsq {

std::string Printer::humanizeSize(uintmax_t bytes) {
	constexpr int SUFFIX_CNT = 6, BASE = 1024;
	constexpr std::array<std::string, SUFFIX_CNT> SUFFIXES { "KB", "MB", "GB", "TB", "PB", "EB" };
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

ftxui::Element Printer::__debugTmpMakeBox(int dimx, int dimy) {
	using namespace ftxui;

	auto title = fmt::format("{}x{}", dimx, dimy);
	return window(text(title) | bold,
								text("FILE") | dim | center) |
				 size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
}

ftxui::Element Printer::createSquaresDom(const Squares& squares) {
	using namespace ftxui;

	std::vector<ftxui::Element> children;
	if (!squares.squares.empty()) {
		for (const auto& square : squares.squares) {
			children.push_back(__debugTmpMakeBox(square.width, square.height));
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

void Printer::print(const File& root) {
	using namespace ftxui;
	auto FileComp = [](const std::string& title) {
		return border(vcenter(paragraphAlignCenter(title)));
	};
	auto FolderComp = [](const std::string& title, const Element& children) {
		return window(text(title), children);
	};

	// TODO: Use squarified treemap
	std::vector<Element> ch;
	for (const auto& child : root.children) {
		auto size = humanizeSize(child.size_bytes);
		if (child.type == FileType::Directory) {
			auto title = fmt::format("{} ({})", child.name, size);
			ch.push_back(FolderComp(title, hbox({})));
		} else if (child.type == FileType::File) {
			auto title = fmt::format("{}\n{}", child.name, size);
			ch.push_back(FileComp(title));
		}
	}
	auto dom = FolderComp(root.name, hbox(ch));

	// DEBUG
	auto leaf1 = std::make_unique<Squares>(Squares {
			.direction = Direction::Vertical,
			.squares = { Square { 30, 20 }, Square { 30, 20 } },
	});
	auto inner1 = std::make_unique<Squares>(Squares {
			.direction = Direction::Horizontal,
			.squares = { Square { 12, 16 }, Square { 12, 16 }, Square { 6, 16 } },
	});
	auto inner2 = std::make_unique<Squares>(Squares {
			.direction = Direction::Horizontal,
			.squares = { Square { 17, 24 }, Square { 13, 24 } },
	});
	std::vector<std::unique_ptr<Squares>> innerGroups;
	innerGroups.push_back(std::move(inner1));
	innerGroups.push_back(std::move(inner2));
	auto leaf2 = std::make_unique<Squares>(Squares {
			.direction = Direction::Vertical,
			.squareGroups = std::move(innerGroups),
	});
	std::vector<std::unique_ptr<Squares>> rootGroups;
	rootGroups.push_back(std::move(leaf1));
	rootGroups.push_back(std::move(leaf2));
	auto mockSquares = Squares {
		.direction = Direction::Horizontal,
		.squareGroups = std::move(rootGroups),
	};
	auto mockSquaresDom = createSquaresDom(mockSquares);
	// DEBUG

	auto screen = ScreenInteractive::Fullscreen();
	auto mainComp = Renderer([&]() { return mockSquaresDom; });
	screen.Loop(mainComp);
}

} // namespace spsq
