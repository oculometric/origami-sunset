#pragma once

#include <stdint.h>

typedef size_t ORIInputAction;

class ORIInput
{
public:
	static ORIInputAction registerAction(const char* name, uint32_t key);
	static void unregisterAction(const ORIInputAction& action);

	static void updateActionStates();
	
	static bool isActionDown(const ORIInputAction& action);
	static bool wasActionPressed(const ORIInputAction& action);
	static bool wasActionReleased(const ORIInputAction& action);
	
	static const char* getActionName(const ORIInputAction& action);
	static size_t getActionHoldCount(const ORIInputAction& action);
};
