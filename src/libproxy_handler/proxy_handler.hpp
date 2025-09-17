#pragma once
#include <functional>
#include <string>
#include <filesystem>
#include <string_view>
#include <utility>
#include <expected>

class Proxy {
    public:
    Proxy(const std::string&,const std::string&,const bool);
    Proxy(std::string&&,std::string&&,const bool);
    auto ip  () const -> const std::string&;
    auto port() const -> const std::string& ;
    auto snap() const -> bool;
    //the code base is too big to replace everything so lets make these aliases to be (a little more we still need () ) compatible with std::pair
    [[deprecated("deprecated: use ip() instead")]]
    const std::function<const std::string&()> first  ;
    [[deprecated("deprecated: use port() instead")]]const std::function<const std::string&()> second  ;
    private:
    std::string m_ip;
    std::string m_port;
    bool m_snap;
};

class ProxyHandler{
    public:
    ProxyHandler();
    auto set(const std::string& ip, const std::string& port) -> std::expected<void, std::string>;
    //auto get() -> std::expected<std::pair<std::string,std::string>,std::string>;
    auto get() -> std::expected<Proxy,std::string>;
    [[maybe_unused]] int run (const std::string& ip, const std::string& port,const std::string& cmd);
    void reset();
    void set_snap (const bool);
    bool snap_available() const;
    bool is_on() const;
    static bool cmd_exists (const std::string_view executable);
    private:
    bool gsettings,snap,kde,git;
    void proxy_file_set(const std::string_view);
    auto validate_inputs(const std::string& ip, const std::string& port)->std::expected<void, std::string> const;
    const std::filesystem::path username;
    const std::filesystem::path proxyfile_path;
};