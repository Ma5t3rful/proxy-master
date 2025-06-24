#pragma once
#include "TGUI/Backend/GLFW-OpenGL3.hpp"
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/CheckBox.hpp"
#include "TGUI/Widgets/ComboBox.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "logger.hpp"
#include "libproxy_handler/proxy_handler.hpp"
#include "manage_window.hpp"
#include "switch_widget.hpp"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/GLFW-OpenGL3.hpp>
#include <memory>
#include <vector>

class MasterWindow:
public tgui::Gui
{
    public:
    MasterWindow(GLFWwindow*,ProxyHandler*);
    private:
    void on_mainswitch_clicked();
    void on_manage_clicked();
    void on_go_command_clicked ();
    void on_force_off_cliced();
    void on_actions_cmbobx_changed(const int);
    void on_bookmark_cmbobx_changed (const int);
    void on_about_btn_clicked();
    void bookmarks_changed(const std::vector<ip_entry>*const bookmarks);
    void show_messagebox(const std::string_view title,const std::string_view message);

    private:
    std::mutex messagebox_mutex; // for show_messagebox();
    const tgui::Button::Ptr manage_btn;
    const tgui::CheckBox::Ptr snap;
    const tgui::ComboBox::Ptr bookmark_cmbobx;
    const tgui::Button::Ptr commnad_go_btn;
    const tgui::EditBox::Ptr ip_input;
    const tgui::EditBox::Ptr port_input;
    const tgui::EditBox::Ptr command_input;
    const tgui::Button::Ptr force_off_button;
    const tgui::ComboBox::Ptr actions_combobox;
    const std::shared_ptr<SwitchWidget> main_switch;
    const tgui::Button::Ptr about_btn;
    const std::shared_ptr<ManageWindow> manage_window;
    const std::shared_ptr<LoggerWidget> logger; 
    ProxyHandler *const proxy_handler;
};