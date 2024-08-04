#pragma once

#ifdef VOX_PLATFORM_WINDOWS

extern Voxen::Application* Voxen::CreateApplication();

int main(int argc, char** argv)
{
	Voxen::Log::Init();
	VOX_CORE_INFO("Starting entry point");

	VOX_PROFILE_BEGIN_SESSION("Startup", "VoxenProfile-Startup.json");
	auto app = Voxen::CreateApplication();
	VOX_PROFILE_END_SESSION();

	VOX_PROFILE_BEGIN_SESSION("Runtime", "VoxenProfile-Runtime.json");
	app->Run();
	VOX_PROFILE_END_SESSION();

	VOX_PROFILE_BEGIN_SESSION("Shutdown", "VoxenProfile-Shutdown.json");
	delete app;
	VOX_PROFILE_END_SESSION();

	return 0;
}

#else
#error Voxen only supports Windows
#endif // !VOX_PLATFORM_WINDOWS