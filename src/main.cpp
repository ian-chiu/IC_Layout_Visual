#include <iostream>
#include "3D_Visual.h"

int main()
{
    try
    {
        Log::Init();
        LOG_WARN("Initialized Log!");
        
        auto app = new Sandbox;
        app->Run();
		delete app;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}