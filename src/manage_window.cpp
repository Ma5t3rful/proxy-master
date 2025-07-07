#include "manage_window.hpp"
#include "TGUI/Layout.hpp"
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/HorizontalLayout.hpp"
#include "TGUI/Widgets/SplitContainer.hpp"
#include "TGUI/Widgets/VerticalLayout.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <functional>
#include <ios>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <print>

ManageWindow::ManageWindow( const std::function< void(const std::vector<ip_entry>*) > of):
tgui::ChildWindow(),
ok_func{of},
name_edtbx(tgui::EditBox::create()),
ip_edtbx(tgui::EditBox::create()),
port_edtbx(tgui::EditBox::create()),
add_btn(tgui::Button::create("Add")),
remove_btn(tgui::Button::create("Remove")),
ok_button(tgui::Button::create("OK")),
cancel(tgui::Button::create("Cancel")),
up(tgui::Button::create("↑")),
down(tgui::Button::create("↓")),
ip_listview(tgui::ListView::create()),
m_config_file_address("/home"/std::filesystem::path(std::getenv("USER"))/".config/proxy-master3.conf")
{
    setResizable();
    load_from_config_file();
    setTitle("Manage Bookmarks");
    setSize(350, 200);
    setOrigin(0.5,0.5);
    setPosition("50%","50%");
    add_btn->onClick(&ManageWindow::on_add_btn_clicked,this);
    add_btn->setEnabled(false);
    remove_btn->onClick(&ManageWindow::on_remove_btn_clicked,this);
    ok_button->onClick(&ManageWindow::on_ok_clicked,this);
    cancel->onClick(&ManageWindow::on_cancel_clicked,this);
    up->onClick(&ManageWindow::on_up_btn_clicked,this);
    down->onClick(&ManageWindow::on_down_btn_clicked,this);
    name_edtbx->onTextChange(&ManageWindow::on_input_text_changed,this);
    ip_edtbx->onTextChange(&ManageWindow::on_input_text_changed,this);
    port_edtbx->onTextChange(&ManageWindow::on_input_text_changed,this);
    name_edtbx->setDefaultText("Name");
    ip_edtbx->setDefaultText("IP");
    port_edtbx->setDefaultText("Port");
    cancel->setTextSize(10);
    const auto sl = tgui::SplitContainer::create();
    sl->add(ip_listview);
    sl->setSplitterWidth(0);
    sl->setSplitterOffset("60%");
    const auto vl = tgui::VerticalLayout::create();
    vl->setWidth(20);
    vl->add(name_edtbx);
    vl->add(ip_edtbx);
    vl->add(port_edtbx);
    vl->add(add_btn);
    vl->add(remove_btn);
    const auto ok_and_other_stuff = tgui::HorizontalLayout::create();
    ok_and_other_stuff->add(up);
    ok_and_other_stuff->add(down);
    ok_and_other_stuff->add(ok_button);
    ok_and_other_stuff->add(cancel);
    vl->add(ok_and_other_stuff);
    vl->setWidth(20);
    vl->getRenderer()->setSpaceBetweenWidgets(5);
    sl->add(vl);
    add(sl);
    ip_listview->addColumn("Name");
    ip_listview->addColumn("Ip");
    ip_listview->addColumn("Port");
    ip_listview->setColumnAutoResize(1, true);
    ip_listview->setColumnAutoResize(2, true);
    ip_listview->setResizableColumns(true);
    ip_listview->getRenderer()->setScrollbarWidth(8);
    for(const auto& [name,ip,port]:m_ip_list)ip_listview->addItem({name,ip,port});
}  

void ManageWindow::load_from_config_file()
{
    // Edit: Fixed
    // Known Bug: Currently, If We add a new entry and use qoutation marks the entry will be ignored.
    // Regular expression is not a good way of doing this. Maybe we need a proper config file library. "Inipp" would do the job better.
    // Or maybe lets just keep the show going with the standard lib and stop adding more deps.
    // Yeah, I guess i will continiue with std::regex but will make it more mature. Something like xml-like tags would help.
    // Edit: Fixed
    std::ifstream config_file(m_config_file_address);
    if(!config_file)return;
    const std::regex entries_regex("<lphentry>(.*?)</lphentry>");
    for(std::string line;std::getline(config_file,line);)
    {
        if(line.starts_with('#'))continue;
        std::array<std::string, 3> matchs_in_a_line;// one for name one for ip one for port this should be replaced with std::inplace_vector
        std::ranges::copy_if(std::sregex_token_iterator(line.cbegin(),line.cend(),entries_regex,1),
                            std::sregex_token_iterator(),
                            matchs_in_a_line.begin(),
                            [i=0,&matchs_in_a_line](const auto& srgx)mutable{if(i>=matchs_in_a_line.size())return false;else i++;return true;});// safe enough
        if(std::ranges::none_of(matchs_in_a_line,&std::string::empty))
            m_ip_list.emplace_back(std::move(matchs_in_a_line[0]),std::move(matchs_in_a_line[1]),std::move(matchs_in_a_line[2]));
    }
}

auto ManageWindow::get_entries () const -> const std::vector<ip_entry>* // maybe i just remove this? NO
{
    return &m_ip_list;
}

void ManageWindow::on_input_text_changed()
{
    add_btn->setEnabled(!name_edtbx->getText().empty() && !ip_edtbx->getText().empty() && !port_edtbx->getText().empty() );
}

void ManageWindow::on_add_btn_clicked()
{
    if(ip_edtbx->getText().empty()||port_edtbx->getText().empty()||name_edtbx->getText().empty())return;
    ip_listview->addItem({
        name_edtbx->getText(),
        ip_edtbx->getText(),
        port_edtbx->getText(),
    });
    for(const auto d:{name_edtbx,ip_edtbx,port_edtbx})d->setText("");
}

void ManageWindow::on_remove_btn_clicked ()
{
    if(const auto selected_item=ip_listview->getSelectedItemIndex();selected_item != -1)
    {
        ip_listview->removeItem(selected_item);
    }
}

void ManageWindow::on_up_btn_clicked()
{
    const auto current_index = ip_listview->getSelectedItemIndex();
    if(current_index <= 0)return; // 0 and -1 will be returned from "current_index"
    const auto current_row = ip_listview->getItemRow(current_index);
    const auto tmp = ip_listview->getItemRow(current_index-1);
    for(auto i = 0 ;i<3;i++)
    {
        ip_listview->changeSubItem(current_index, i, tmp.at(i));
        ip_listview->changeSubItem(current_index-1, i, current_row.at(i));
    }
    ip_listview->setSelectedItem(current_index-1);
}

void ManageWindow::on_down_btn_clicked()
{
    const auto current_index = ip_listview->getSelectedItemIndex();
    if(const auto last_item = ip_listview->getItemCount()-1; current_index == last_item || current_index==-1)return;
    const auto current_row = ip_listview->getItemRow(current_index);
    const auto tmp = ip_listview->getItemRow(current_index+1);
    for(auto i = 0 ;i<3;i++)
    {
        ip_listview->changeSubItem(current_index, i, tmp.at(i));
        ip_listview->changeSubItem(current_index+1, i, current_row.at(i));
    }
    ip_listview->setSelectedItem(current_index+1);
}

void ManageWindow::on_ok_clicked () // writes to config flie...
{
    std::ofstream config_file_out(m_config_file_address);
    m_ip_list.clear();
    std::println(config_file_out,"## Don't edit this config file unless you know what you're doning ##");
    for(const auto& row:ip_listview->getItemRows())
    {
        m_ip_list.emplace_back(row.at(0),row.at(1),row.at(2));
        std::println(config_file_out,"<lphentry>{}</lphentry> <lphentry>{}</lphentry> <lphentry>{}</lphentry>",
             row.at(0).toStdString(),row.at(1).toStdString(),row.at(2).toStdString());
    }
    ok_func(&m_ip_list);
    close();
}

void ManageWindow::on_cancel_clicked()
{
    ip_listview->removeAllItems();
    for(const auto& [name,ip,port]:m_ip_list)ip_listview->addItem({name,ip,port});
    close();
}