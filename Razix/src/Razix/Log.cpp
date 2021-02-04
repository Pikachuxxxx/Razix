#include "rzxpch.h"
#include "Log.h"


namespace Razix
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ApplicationLogger;

    void Log::InitLogger()
    {
        // TODO: Add src file and line logging to the pattern
        // Try using __FUNCTION__, __LINE__ and __FILE__ etc.instead of using Macro-ed global SPDLog functions
        spdlog::set_pattern("%^ %n [%T] :: %v %$");
        
        s_CoreLogger = spdlog::stdout_color_mt("Razix Core");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ApplicationLogger = spdlog::stdout_color_mt("Razix Application");
        s_ApplicationLogger->set_level(spdlog::level::info);
    }

}