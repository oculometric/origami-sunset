#include "../inc/input.h"

#include <vector>

#if defined(OPENGL)
#include "../inc/screen.h"
#include <GLFW/glfw3.h>
#endif

struct ORIInputActionInternal
{
	const char* name = nullptr;
	uint32_t binding = 0;
	bool is_down = false;
	bool down_last_update = false;
	size_t ticks_held_for = 0;
};

static std::vector<ORIInputActionInternal*> actions;

ORIInputAction ORIInput::registerAction(const char* name, uint32_t key)
{
	auto iai = new ORIInputActionInternal{ name, key, false, false, 0 };

	actions.push_back(iai);

	return (ORIInputAction)iai;
}

bool ORIInput::isActionDown(const ORIInputAction& action)
{
	auto iai = (ORIInputActionInternal*)action;
	
	return iai->is_down;
}

bool ORIInput::wasActionPressed(const ORIInputAction& action)
{
	auto iai = (ORIInputActionInternal*)action;
	return (iai->is_down && !iai->down_last_update);
}

bool ORIInput::wasActionReleased(const ORIInputAction& action)
{
	auto iai = (ORIInputActionInternal*)action;

	return (!iai->is_down && iai->down_last_update);
}

void ORIInput::updateActionStates()
{
	glfwPollEvents();
	for (auto act : actions)
	{
		act->down_last_update = act->is_down;
#if defined(OPENGL)
		act->is_down = (glfwGetKey(ORIScreen::getWindow(), act->binding) == GLFW_PRESS);
		if (act->is_down)
			act->ticks_held_for++;
		else
			act->ticks_held_for = 0;
#endif
	}
}

void ORIInput::unregisterAction(const ORIInputAction& action)
{
	auto iai = (ORIInputActionInternal*)action;

	auto it = actions.begin();
	while (it != actions.end())
	{
		if (*it == iai)
		{
			actions.erase(it);
			break;
		}
	}
	delete iai;
}

const char* ORIInput::getActionName(const ORIInputAction& action)
{
	auto iai = (ORIInputActionInternal*)action;

	return iai->name;
}

size_t ORIInput::getActionHoldCount(const ORIInputAction& action)
{
	auto iai = (ORIInputActionInternal*)action;

	return iai->ticks_held_for;
}
