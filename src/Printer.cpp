#include "Printer.hpp"
#include "File.hpp"
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

	auto screen = ScreenInteractive::Fullscreen();
	auto mainComp = Renderer([&]() { return dom; });
	screen.Loop(mainComp);
}

} // namespace spsq
