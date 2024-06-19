#pragma once

#include "Voxen/Core.h"

#include <string>
#include <functional>

namespace Voxen
{
	// Events are currently blocking, so when an event occurs it
	// will immediately get dispatched and will be delt with in that
	// exact moment. For the future, I will make a buffer to buffer events
	// into a buss and process them during the 'event' part of the update loop

	enum class 
}