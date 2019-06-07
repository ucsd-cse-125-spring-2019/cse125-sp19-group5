#include "../Draw.h"
#include "Gui.h"
#include <locale>

std::vector<GuiElement*> Gui::rootElements;
std::vector<GuiElement*> Gui::allElements;

static std::string unicodeToUTF8(unsigned int codepoint)
{
    std::string out;

    if (codepoint <= 0x7f)
        out.append(1, static_cast<char>(codepoint));
    else if (codepoint <= 0x7ff)
    {
        out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }
    else if (codepoint <= 0xffff)
    {
        out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }
    else
    {
        out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }
    return out;
}

static void charCallback(GLFWwindow* window, unsigned int code) {
	Gui::dispatchChar(unicodeToUTF8(code));
}

static void cursorPosCallback(GLFWwindow *window, double x, double y) {
	x /= Draw::screenWidth;
	y /= Draw::screenHeight;
	Gui::dispatchMousePos((float)x, 1.0f - (float)y);
}

static void buttonCallback(int button, int action, int mods) {
	auto x = Input::getMouseX() / Draw::screenWidth;
	auto y = Input::getMouseY() / Draw::screenHeight;
	Gui::dispatchMouseButton((float)x, 1.0f - (float)y, button, action);
}

static void keyCallback(int key, int scancode, int action, int mods) {
	Gui::KeyState state;
	switch (action) {
		case GLFW_PRESS:
			state = Gui::KeyState::PRESSED;
			break;
		case GLFW_REPEAT:
			state = Gui::KeyState::HELD;
			break;
		default:
			state = Gui::KeyState::RELEASED;
			break;
	}

	// This is a bit messy, but no clear way to get enum
	// from value for noncontinuous values.
	switch (static_cast<Gui::Key>(key)) {
		case Gui::Key::BACKSPACE:
		case Gui::Key::ESCAPE:
		case Gui::Key::ENTER:
		case Gui::Key::TAB:
		case Gui::Key::LEFT:
		case Gui::Key::RIGHT:
		case Gui::Key::UP:
		case Gui::Key::DOWN:
		case Gui::Key::DEL:
		case Gui::Key::END:
			Gui::dispatchKey(static_cast<Gui::Key>(key), state);
			break;

	}
}

void Gui::setupInputListeners(GLFWwindow *window) {
	glfwSetCharCallback(window, charCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	Input::addKeyCallback(keyCallback);
	Input::addMouseButtonCallback(buttonCallback);
}

void Gui::onElementRemoved(GuiElement *element) {
	for (auto child : element->getChildren()) {
		child->remove();
	}
	auto it = std::find(rootElements.begin(), rootElements.end(), element);
	if (it != rootElements.end()) {
		rootElements.erase(it);
	}
	it = std::find(allElements.begin(), allElements.end(), element);
	if (it != allElements.end()) {
		allElements.erase(it);
	}
	delete element;
}

void Gui::onElementParentSet(GuiElement *element, GuiElement *parent) {
	// If there is a parent, it is no longer a root element.
	// If there is no parent, make sure rootElements won't have duplicates.
	auto it = std::find(rootElements.begin(), rootElements.end(), element);
	if (it != rootElements.end()) {
		if (parent) {
			rootElements.erase(it);
		}
		return;
	}

	// If no parent, then this is a root level element.
	if (!parent) {
		rootElements.push_back(element);
	}
}

void Gui::draw() {
	for (auto it = rootElements.begin(); it != rootElements.end(); it++) {
		(*it)->drawElement();
	}
}

void Gui::update(float dt) {
	for (auto element : allElements) {
		element->update(dt);
	}
}

void Gui::cleanUp() {
	for (auto it = rootElements.begin(); it != rootElements.end(); it++) {
		delete *it;
	}
	rootElements.clear();
}

void Gui::dispatchMouseButton(float x, float y, int button, int action) {
	for (auto element : rootElements) {
		if (element->dispatchMouseButton(x, y, button, action)) {
			return;
		}
	}
}

void Gui::dispatchMousePos(float x, float y) {
	for (auto element : rootElements) {
		if (element->dispatchMousePos(x, y)) {
			return;
		}
	}
}

void Gui::dispatchChar(const std::string &c) {
	for (auto element : rootElements) {
		if (element->dispatchChar(c)) {
			return;
		}
	}
}

void Gui::dispatchKey(Gui::Key key, Gui::KeyState state) {
	for (auto element : rootElements) {
		if (element->dispatchKey(key, state)) {
			return;
		}
	}
}
