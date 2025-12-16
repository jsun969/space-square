#include <fmt/ranges.h>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <vector>

void ftxUIDemo() {
	using namespace ftxui;

	Element document = hbox({
			text("left") | border,
			text("middle") | border | flex,
			text("right") | border,
	});

	auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
	Render(screen, document);
	screen.Print();
}

int main() {
	ftxUIDemo();
	fmt::print("Hello, Space Square {}\n", std::vector<int> { 1, 2, 3, 4, 5 });
	return 0;
}
