#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <filesystem>
#include "TGUI/Backend/GLFW-OpenGL3.hpp"
#include "TGUI/Loading/Theme.hpp"
#include "libproxy_handler/proxy_handler.hpp"
#include "master_window.hpp"
#include "sdl_backend_helper.hpp"

auto parse_arg (const std::string_view arg) -> std::string
{
    if (arg.starts_with("--")) {
        return std::string(arg.substr(2));
    }
    const std::unordered_map<std::string_view, std::string> flag_name{
        {"-h","help"},
        {"-r","reset"},
    };  
    if(const auto find = flag_name.find(arg);find != flag_name.end())
        return find->second;
    return std::string(arg);
}


auto main (int argc,char**argv) -> int
{
    ProxyHandler proxy;
    if(argc>1)
    {
        const auto main_argv = parse_arg(argv[1]);
        std::println("Proxy Master command line mode:");
        if(main_argv == "help")std::println("USAGE: proxy-master [COMMAND] [ARGS]...");
        else if(main_argv == "set")
        {
            if(argc != 4)
            {
                std::println("number of args for setting the proxy must be 3");
                return 2;
            }
            if(const auto result = proxy.set(argv[2], argv[3]);result)std::println("Proxy is set!");
            else std::println("Error: {}",result.error());
        }
        else if(main_argv == "check") std::println("Proxy is {}",proxy.is_on()?"ON":"OFF");
        else std::println("Unknown command");
        return 0;
    }
    
    //this software is already linux-only. linux-specific way to find current exe
    const auto theme_path = std::filesystem::canonical("/proc/self/exe").remove_filename()/"dark.txt";
    if(std::filesystem::exists(theme_path))
        tgui::Theme::setDefault(theme_path.string());
    else 
        std::println("ERROR: Couldn't find the theme file.");
    glfw_helper helper;
    tgui::Gui gui (helper.window);
    
    MasterWindow w(helper.window,&proxy);
    w.mainLoop("#1a1a1a");
}
