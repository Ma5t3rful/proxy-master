#pragma once
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/ListView.hpp"
#include <filesystem>
#include <functional>
#include <string>
#include <vector>


using ip_entry=std::tuple<std::string,std::string ,std::string>;

class ManageWindow:
public tgui::ChildWindow
{
    public:
    ManageWindow(const std::function<void(  const std::vector<ip_entry>*  )>);
    auto get_entries () const -> const std::vector<ip_entry>*;
    private:
    void load_from_config_file ();
    void on_ok_clicked();
    void on_up_btn_clicked ();
    void on_down_btn_clicked ();
    void on_cancel_clicked();
    void on_add_btn_clicked();
    void on_remove_btn_clicked();
    void on_input_text_changed ();
    const std::function<void(const std::vector<ip_entry>*)> ok_func;//will be ran inside on_ok_clicked()
    const tgui::EditBox::Ptr name_edtbx;
    const tgui::EditBox::Ptr ip_edtbx;
    const tgui::EditBox::Ptr port_edtbx;
    const tgui::ListView::Ptr ip_listview;
    const tgui::Button::Ptr add_btn;
    const tgui::Button::Ptr remove_btn;
    const tgui::Button::Ptr ok_button;
    const tgui::Button::Ptr cancel;
    const tgui::Button::Ptr up;
    const tgui::Button::Ptr down;
    std::vector<ip_entry> m_ip_list;
    std::filesystem::path m_config_file_address;
};