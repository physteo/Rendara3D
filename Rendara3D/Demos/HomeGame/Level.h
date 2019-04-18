#pragma once
#include "../../Window.h"

namespace homelife {


	class Level
	{
	public:
		virtual void load()                        = 0;
		virtual void render(Window& window)        = 0;
		virtual void update(Window& window)        = 0;
		virtual void gameOver(Window& window)      = 0;
		virtual void gameCompleted(Window& window) = 0;
	};



}