#include "proxy_handler.hpp"
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <expected>
#include <fstream>
#include <iterator>
#include <limits>
#include <regex>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <tuple>
#include <format>
#include <ranges>
#include <print> //It's needed for printing to files


ProxyHandler::ProxyHandler():
gsettings(cmd_exists("gsettings")),
snap(cmd_exists("snap")),
kde(std::string(const_cast<const char*>(std::getenv("XDG_CURRENT_DESKTOP"))) == "KDE"), // Dangerous
username(std::getenv("USER")?std::getenv("USER"):""),
proxyfile_path("/home"/username/".config/proxy-set.sh"),
git(cmd_exists("git"))
{}

bool ProxyHandler::cmd_exists(const std::string_view executable)
{
    const static auto path_env = std::getenv("PATH");
    if(not path_env)throw std::runtime_error("The environment variable PATH does not exist on your system.");
    const auto path = std::string (path_env);
    
    auto path_seperated_view = path
    | std::views::split(':')
    | std::views::transform([](const auto c){return std::filesystem::path(c.begin(),c.end());});
    return std::ranges::any_of(
        path_seperated_view,
        [executable](const auto &p){return std::filesystem::exists(p/executable);}
    );
}

void ProxyHandler::set_snap(const bool b)
{
    this->snap = b;
}

bool ProxyHandler::snap_available() const
{
    return snap;
}

auto ProxyHandler::validate_inputs(const std::string& ip,const std::string& port)->std::expected<void, std::string> const
{
    if (ip.find_first_not_of(".1926807345") != std::string::npos)return std::unexpected("Ip address must only contain numbers and dots.");
    if (port.find_first_not_of("1234567890")!=std::string::npos)return std::unexpected("Port must only be a number");
    try{if (std::stoi(port) > std::numeric_limits<std::uint16_t>::max())return std::unexpected("The value of port is too large.");}
    catch(const std::exception &e){return std::unexpected("Port value is incorrect");}
    return {};
}

auto ProxyHandler::set(const std::string &ip, const std::string &port) -> std::expected<void, std::string>
{
    // TODO: I should just use std::expected<> and avoid using 'throw'.
    if(const auto input_check_res = this->validate_inputs(ip, port);!input_check_res)
        return std::unexpected(input_check_res.error());
    
    const auto address = std::format("{}://{}:{}","http",ip,port);//protocol has a placeholder
    this->proxy_file_set(address);

    if(snap)
    {
        const auto snap_enable_proxy_cmd = std::format("pkexec sh -c 'snap set system proxy.http={0} && "
                                                                     "snap set system proxy.https={0}'",address);
        std::ignore = std::system(snap_enable_proxy_cmd.c_str());
        /*std::ignore = std::system(("pkexec sh -c 'snap set system proxy.http=" + address +
                                   " && "+          "snap set system proxy.https="+ address+'\'').c_str());*/
    }
    if(kde)
    {
        for(const auto& protocol:{"httpProxy","httpsProxy","socksProxy","ftpProxy"})
        {
            const auto cmd = std::format("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key {} http://{}:{}",protocol,ip,port);
            std::ignore = std::system(cmd.c_str());
        }
        std::ignore = std::system("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 1");
    }
    if (git)
    {
        const auto git_set_proxy_cmd = std::format("git config --global http.proxy  {0} &&"
                                                   "git config --global https.proxy {0}",address);
        std::ignore = std::system(git_set_proxy_cmd.c_str());
    }
    if(gsettings)
    {
        auto error = 0;
        error += std::system("gsettings set org.gnome.system.proxy mode 'manual'");
        for(const auto &protocol:{"http","https","socks"})
        {
            const auto cmd = std::format("gsettings set org.gnome.system.proxy.{0} host {1} "
                                      "&& gsettings set org.gnome.system.proxy.{0} port {2} ",protocol,ip,port);
            error+=std::system(cmd.c_str());
        }
        error += std::system("gsettings set org.gnome.system.proxy use-same-proxy true");
        if(error!=0) return std::unexpected("gsettings failed to set your proxy.");
    }
    
    return {};
}

void ProxyHandler::reset()
{
    this->proxy_file_set("");
    if(snap)
    {
        std::ignore = std::system("pkexec snap unset system proxy.http proxy.https");
    }
    if (kde)
    {
        std::ignore = std::system("kwriteconfig5 --file kioslaverc --group 'Proxy Settings' --key ProxyType 0");
    }
    if(gsettings)
    {
        if(
        std::system("gsettings set org.gnome.system.proxy mode 'none'")!=0)
        throw std::runtime_error("gsettings failed to unset your proxy.");
    }
    if(git)
    {
        std::ignore = std::system("git config --global --unset http.proxy&&"
                                  "git config --global --unset https.proxy");
    }
}

auto ProxyHandler::get() -> std::expected<Proxy, std::string>
{
    if(!this->is_on())return std::unexpected("No proxy is set");
    std::ifstream proxy_file(proxyfile_path);
    const std::string address((std::istreambuf_iterator<char>(proxy_file)),std::istreambuf_iterator<char>());
    std::smatch ip_match,port_match,snap_match ;
    std::regex_search(address,ip_match,std::regex(R"([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"));
    std::regex_search(address,port_match,std::regex(R"([0-9]+(?=\"|\s|\/))"));
    std::regex_search(address,snap_match,std::regex("snap=(\\w*)"));
    const auto was_snap_active { snap_match[1] == "true" };
    if(ip_match.empty()||port_match.empty())return std::unexpected("Currupted config file detected in: " + proxyfile_path.string() + " Set proxy again to override or delete the config file");
    //return std::make_tuple(ip_match[0],port_match[0]);//implicit conversion
    return Proxy(ip_match[0],port_match[0],was_snap_active);
}

int ProxyHandler::run(const std::string &ip, const std::string &port, const std::string &cmd)
{
    if(cmd.empty())throw std::runtime_error("Empty Command.");
    if(const auto res = this->validate_inputs(ip, port);!res)throw std::runtime_error(res.error());
    const auto address = std::format("{}://{}:{}","http",ip,port);
    //set env var for app
    for(const auto* e:{"http_proxy","https_proxy","HTTP_PROXY","HTTPS_PROXY","socks_proxy"})setenv(e, address.c_str(), true);
    return std::system((cmd + " & > /dev/null").c_str());
}

void ProxyHandler::proxy_file_set(const std::string_view addr)
{
    // File is open but changing its premissions won't make any issues. 
    // the order is important because for the first time the config file does not exist and changing premission of a non-existant file will throw.
    std::ofstream proxyfile(proxyfile_path);
    std::filesystem::permissions(proxyfile_path, 
                std::filesystem::perms::owner_all | 
                std::filesystem::perms::group_all,
                std::filesystem::perm_options::add);
    if (addr.empty())std::println(proxyfile);// override to a single endline
    else std::println(proxyfile,
R"(#!/bin/sh
# snap={0}
export  http_proxy="{1}"
export https_proxy="{1}"
export  HTTP_PROXY="{1}"
export HTTPS_PROXY="{1}"
alias yt-dlp="yt-dlp --proxy='{1}'"
alias sudo="sudo -E"
# Auto-generated file by libproxy_handler 3.0. Do not edit.)",
    snap,
    addr);
}

bool ProxyHandler::is_on() const
{
    return (std::filesystem::exists(proxyfile_path) && std::filesystem::file_size(proxyfile_path) > 3);
}
