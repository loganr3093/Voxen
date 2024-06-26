#pragma once

#ifdef VOX_PLATFORM_WINDOWS

extern Voxen::Application* Voxen::CreateApplication();

int main(int argc, char** argv)
{
	Voxen::Log::Init();
	VOX_CORE_INFO("Starting entry point");
	VOX_CORE_INFO("Initializing Core");
	VOX_INFO("Initializing Client");

	auto app = Voxen::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#else
#error Voxen only supports Windows
#endif // !VOX_PLATFORM_WINDOWS