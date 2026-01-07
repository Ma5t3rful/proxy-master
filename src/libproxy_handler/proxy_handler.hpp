#pragma once
#include <string>
#include <filesystem>
#include <string_view>
#include <expected>

struct Proxy {
    std::string ip;
    std::string port;
    bool snap;
};

class ProxyHandler{
    public:
    ProxyHandler();
    auto set(const std::string& ip, const std::string& port) -> std::expected<void, std::string>;
    auto get() -> std::expected<Proxy,std::string>;
    [[maybe_unused]] int run (const std::string& ip, const std::string& port,const std::string& cmd);
    void reset();
    void set_snap (const bool);
    bool snap_available() const;
    bool is_on() const;
    static bool cmd_exists (const std::string_view executable);
    
    private:
    void proxy_file_set(const std::string_view);
    auto validate_inputs(const std::string& ip, const std::string& port)->std::expected<void, std::string> const;
    bool gsettings,snap,kde,git;
    const std::filesystem::path username;
    const std::filesystem::path proxyfile_path;
};