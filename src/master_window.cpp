#include "master_window.hpp"
#include "TGUI/Layout.hpp"
#include "TGUI/Vector2.hpp"
#include "TGUI/Widget.hpp"
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/CheckBox.hpp"
#include "TGUI/Widgets/ComboBox.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/Group.hpp"
#include "TGUI/Widgets/HorizontalLayout.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/MessageBox.hpp"
#include "TGUI/Widgets/SplitContainer.hpp"
#include "TGUI/Widgets/VerticalLayout.hpp"
#include "about_dialog.hpp"
#include "center_layout_widget.hpp"
#include "proxy_handler.hpp"
#include "logger.hpp"
#include "manage_window.hpp"
#include "switch_widget.hpp"
#include <array>
#include <exception>
#include <expected>
#include <iterator>
#include <memory>
#include <mutex>
#include <string_view>
#include <ranges>
#include <vector>


MasterWindow::MasterWindow(GLFWwindow* w,ProxyHandler* p):
tgui::Gui(w),
main_switch(std::make_shared<SwitchWidget>()),
force_off_button(tgui::Button::create("Force Off")),
ip_input(tgui::EditBox::create()),
port_input(tgui::EditBox::create()),
snap_chkbox(tgui::CheckBox::create("Snap Proxy")),
bookmark_cmbobx(std::make_shared<tgui::ComboBox>()),
manage_btn(tgui::Button::create("Manage")),
command_input(tgui::EditBox::create()),
commnad_go_btn(tgui::Button::create(" > ")),
actions_combobox(tgui::ComboBox::create()),
about_btn(tgui::Button::create("About")),
manage_window(std::make_shared<ManageWindow>(std::bind_front(&MasterWindow::bookmarks_changed,this))),
logger(std::make_shared<LoggerWidget>()),
proxy_handler(p)
{
    constexpr auto default_height = 28; // this is default height for most buttons
    const auto title_label = tgui::Label::create("ProxyMaster");
    ip_input->setDefaultText("IP Address");
    port_input->setDefaultText("Port");
    command_input->setDefaultText("App Launcher");
    title_label->setWidth("100%");
    title_label->setPosition(0,10);
    title_label->setTextSize(32);
    title_label->setHeight(48);
    title_label->getScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);
    title_label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
    // Actions are still unused 
    actions_combobox->setOrigin(0.5f,0);
    actions_combobox->setPosition("50%",180);
    actions_combobox->setDefaultText("Actions");
    actions_combobox->addItem("Force Off");
    actions_combobox->addItem("Force On");
    actions_combobox->addItem("Test...");
    actions_combobox->setWidth(90);
    actions_combobox->onItemSelect(&MasterWindow::on_actions_cmbobx_changed,this);
    force_off_button->setPosition("50%",180);
    main_switch->onClick(&MasterWindow::on_mainswitch_clicked,this);
    force_off_button->onClick(&MasterWindow::on_force_off_cliced,this);
    manage_btn->onClick(&MasterWindow::on_manage_clicked,this);
    commnad_go_btn->onClick(&MasterWindow::on_go_command_clicked,this);
    about_btn->onClick(&MasterWindow::on_about_btn_clicked,this);
    logger->setOrigin(0.5,0);
    logger->setPosition("50%",430);
    logger->log("Initialized.");
    main_switch->setOrigin(0.5,0);
    main_switch->setSize(128,128);
    main_switch->setPosition("50%",48);
    force_off_button->setOrigin(0.5,0);
    const auto ip_vert_layout = tgui::VerticalLayout::create();
    const auto split_manage = tgui::SplitContainer::create();
    split_manage->add(bookmark_cmbobx);
    split_manage->add(manage_btn);
    split_manage->setSplitterWidth(0);
    split_manage->setSplitterOffset("75%");
    ip_vert_layout->setOrigin(.5,0);
    ip_vert_layout->setPosition("50%",215);
    ip_vert_layout->setSize("90%",122);
    ip_vert_layout->getRenderer()->setSpaceBetweenWidgets(3);
    for(const auto& w:std::array<std::shared_ptr<tgui::Widget>, 4>{ip_input,port_input,snap_chkbox,split_manage})ip_vert_layout->add(w);
    const auto launcher_group = std::make_shared<tgui::Group>();
    launcher_group->setSize("90%",default_height+13);
    launcher_group->setPosition("50%",364);
    launcher_group->setOrigin(0.5,0);
    launcher_group->add(command_input);
    launcher_group->add(commnad_go_btn);
    command_input->setAutoLayout(tgui::AutoLayout::Fill);
    about_btn->setHeight(default_height);
    about_btn->setOrigin(0.5,1);
    about_btn->setPosition("50%","100%-20");
    //workaround for streched checkbox
    const auto checkbox_orig_size = snap_chkbox->getSize().y;
    // Possible tgui bug: onViewChange can extend the lifetime of the widget beyond the tgui::Gui's lifetime. We can use the get function to let the smart pointer release the memory. 
    // before the lifetime of the object
    onViewChange(static_cast<void(tgui::CheckBox::*)(const tgui::Layout)>(&tgui::CheckBox::setWidth),snap_chkbox.get(),checkbox_orig_size);
    bookmark_cmbobx->onItemSelect(&MasterWindow::on_bookmark_cmbobx_changed,this);
    bookmark_cmbobx->setDefaultText("Bookmarks");
    command_input->setAutoLayout(tgui::AutoLayout::Fill);
    commnad_go_btn->setAutoLayout(tgui::AutoLayout::Right);
    // get data from previous config
    force_off_button->setHeight(default_height);
    const auto ip_entries = manage_window->get_entries();
    bookmarks_changed(ip_entries);
    const auto snap_available = proxy_handler->snap_available();
    if(snap_available)
    {
        logger->log("snap package manager detected.");
        actions_combobox->addItem("Re-enable with snap");
        actions_combobox->addItem("Re-disable with snap");
    }
    snap_chkbox->setEnabled(snap_available);
    const auto prev_ip_port = proxy_handler->get();
    logger->log(std::format("Current State: {}",prev_ip_port.has_value()?"<b>ON</b>":"<b>OFF</b>"),
                            prev_ip_port.has_value()?LoggerWidget::SUCCESS: LoggerWidget::WARNING);
    if(prev_ip_port.has_value())
    {
        const auto& [prev_ip,prev_port,prev_snap_on] = *prev_ip_port; //already checked so it's safe plus we have fhardened now.
        ip_input->setText(prev_ip);
        port_input->setText(prev_port);
        snap_chkbox->setChecked(prev_snap_on);
        main_switch->set_state(SwitchWidget::STATE::ON);
        if(const auto find = std::ranges::find_if(*ip_entries,[&prev_ip,&prev_port](const auto &e){const auto& [_,ip,port] = e;return ip == prev_ip&&port == prev_port;});
        find != ip_entries->end())
        {
            const auto index_find = std::distance(ip_entries->begin(),find);
            bookmark_cmbobx->setSelectedItemByIndex(index_find);
        }
    }
    else
    {
        main_switch->set_state(SwitchWidget::STATE::OFF);
        logger->log(prev_ip_port.error(),LoggerWidget::ERROR);
    }
    ip_input->onTextChange(&MasterWindow::on_any_input_changed,this);
    port_input->onTextChange(&MasterWindow::on_any_input_changed,this);
    add(title_label);
    add(main_switch);
    add(force_off_button);
    add(logger);
    add(ip_vert_layout);
    add(launcher_group);
    add(about_btn);
    // add(actions_combobox);
    snap_chkbox->setSize(checkbox_orig_size, checkbox_orig_size);
}



////////////////////// Second constructor for UI redesign ///////////////////////
MasterWindow::MasterWindow(GLFWwindow* w,ProxyHandler* p, const experimental_ui):
tgui::Gui(w),
main_switch(std::make_shared<SwitchWidget>()),
force_off_button(tgui::Button::create("Force Off")),
ip_input(tgui::EditBox::create()),
port_input(tgui::EditBox::create()),
snap_chkbox(tgui::CheckBox::create("Snap Proxy")),
bookmark_cmbobx(std::make_shared<tgui::ComboBox>()),
manage_btn(tgui::Button::create("Manage")),
command_input(tgui::EditBox::create()),
commnad_go_btn(tgui::Button::create(" > ")),
actions_combobox(tgui::ComboBox::create()),
about_btn(tgui::Button::create("About")),
manage_window(std::make_shared<ManageWindow>(std::bind_front(&MasterWindow::bookmarks_changed,this))),
logger(std::make_shared<LoggerWidget>()),
proxy_handler(p)
{
    const auto proxy_master_lbl = tgui::Label::create("Proxy Master");
    proxy_master_lbl->setTextSize(28);
    proxy_master_lbl->getScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);
    proxy_master_lbl->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
    proxy_master_lbl->setWidth("100%");
    // proxy_master_lbl->setAutoLayout(tgui::AutoLayout::Top);
    add(proxy_master_lbl);


    const auto master_group = tgui::Group::create();
    master_group->setOrigin(.5f,.5f);
    master_group->setPosition("50%","50%");
    master_group->setWidth("100%-10"); //10 px padding


    const auto main_switch_container = std::make_shared<CenterLayoutWidget>();
    const auto force_off_button_wrapper = tgui::Group::create();
    main_switch->onClick(&MasterWindow::on_mainswitch_clicked,this);
    main_switch->setHeight(148);
    force_off_button->onClick(&MasterWindow::on_force_off_cliced,this);
    force_off_button_wrapper->setHeight(30);
    force_off_button->setOrigin(0.5,0.5);
    force_off_button->setPosition("50%");
    force_off_button_wrapper->add(force_off_button);

    main_switch_container->add_widget(main_switch);
    main_switch_container->add_widget(force_off_button_wrapper);
    main_switch_container->setWidth("30%");
    main_switch_container->setAutoLayout(tgui::AutoLayout::Left);
    master_group->add(main_switch_container);
    

    const auto ip_port_container = std::make_shared<CenterLayoutWidget>();
    ip_input->setDefaultText("IP Address");
    port_input->setDefaultText("Port");
    ip_port_container->setHeight("100%");
    ip_port_container->setAutoLayout(tgui::AutoLayout::Fill);
    ip_port_container->set_spacing(10);
    ip_port_container->add_widget(ip_input);
    ip_port_container->add_widget(port_input);
    ip_port_container->add_widget(snap_chkbox);    
    onViewChange(static_cast<void(tgui::CheckBox::*)(const tgui::Layout)>(&tgui::CheckBox::setWidth),snap_chkbox.get(),snap_chkbox->getSize().y);
    
    const auto spacing = std::make_shared<tgui::Group>();//for separating ip port and booksmarks a bit
    spacing->setHeight(1);
    ip_port_container->add_widget(spacing);
    const auto bookmarks_container = tgui::HorizontalLayout::create();
    bookmarks_container->getRenderer()->setSpaceBetweenWidgets(10);
    bookmarks_container->setHeight(24);
    bookmarks_container->add(bookmark_cmbobx);
    bookmarks_container->add(manage_btn);
    ip_port_container->add_widget(bookmarks_container);
    const auto command_runner_container = tgui::Group::create();
    command_runner_container->setHeight(ip_input->getSize().y);
    command_input->setDefaultText("App Launcher");
    command_input->setAutoLayout(tgui::AutoLayout::Fill);
    commnad_go_btn->setAutoLayout(tgui::AutoLayout::Right);
    command_runner_container->add(command_input);
    command_runner_container->add(commnad_go_btn);
    ip_port_container->add_widget(command_runner_container);
    master_group->add(ip_port_container);
    
    add(master_group);
    
    
    const auto bookmarks_from_cfg = manage_window->get_entries();
    for(const auto& title : *bookmarks_from_cfg | std::views::elements<0>)bookmark_cmbobx->addItem(title);
    snap_chkbox->setEnabled(proxy_handler->snap_available());
    if(const auto prev_ip_port = proxy_handler->get();
       prev_ip_port.has_value())
    {
        main_switch->set_state(SwitchWidget::STATE::ON);
        const auto& [ip,port,snap] = *prev_ip_port;
        ip_input->setText(ip);
        port_input->setText(port);
        snap_chkbox->setChecked(snap);
        if(const auto prev_selected_bookmark = std::ranges::find_if(*bookmarks_from_cfg,[&ip,&port](const auto &bookmark){
            const auto &[_,_i,_p] = bookmark;
            return _i == ip and _p==(port);
        });
        prev_selected_bookmark!=bookmarks_from_cfg->end())
        {
            bookmark_cmbobx->setSelectedItemByIndex(std::distance(bookmarks_from_cfg->begin(),prev_selected_bookmark));
        }
    }
    else 
    {
        main_switch->set_state(SwitchWidget::STATE::OFF);
    }
    ip_input->onTextChange(&MasterWindow::on_any_input_changed,this);
    port_input->onTextChange(&MasterWindow::on_any_input_changed,this);
    bookmark_cmbobx->onItemSelect(&MasterWindow::on_bookmark_cmbobx_changed,this);
    manage_btn->onClick(&MasterWindow::on_manage_clicked,this);
    commnad_go_btn->onClick(&MasterWindow::on_go_command_clicked,this);
    
    snap_chkbox->setWidth(snap_chkbox->getSize().y);
}



void MasterWindow::on_mainswitch_clicked()
{
    proxy_handler->set_snap(snap_chkbox->isChecked());
    if(main_switch->state()==SwitchWidget::STATE::ON)// if on then turn it off
    {
        try{
        proxy_handler->reset();
        main_switch->set_state(SwitchWidget::STATE::OFF);
        logger->log("Current State: <b>OFF</b>",LoggerWidget::WARNING);
        }catch(const std::exception& e){
            logger->log(e.what(),LoggerWidget::ERROR);
        }
        return;
    }
    const auto ip = ip_input->getText().toStdString();
    const auto port = port_input->getText().toStdString();
    if(const auto result = proxy_handler->set(ip, port);
    result)
    {
        main_switch->set_state(SwitchWidget::STATE::ON);
        logger->log(std::format("addr: http://{}:{}",ip,port),LoggerWidget::SUCCESS);
        logger->log("Current State: <b>ON</b>", LoggerWidget::SUCCESS);
    }
    else
    {
        logger->log(result.error(),LoggerWidget::ERROR);
        show_messagebox("Failed to properly set the proxy", result.error());
    }
}

void MasterWindow::on_force_off_cliced()
{
    proxy_handler->reset();
    main_switch->set_state(SwitchWidget::STATE::OFF);
    logger->log("All proxy settings has been resetted", LoggerWidget::SUCCESS);
}

void MasterWindow::on_actions_cmbobx_changed (const int selected_idx)
{
    logger->log(std::format("Item changed to: {}",selected_idx));
    switch(selected_idx)
    {
        case 0:{
            on_force_off_cliced();
            break;
        }
        case 1:{
            break;
        }
        default:{
            logger->log("Undesired value received by combobox. This is a ui issue in master window.",LoggerWidget::MSG_TYPE::ERROR);
        }
    }
    actions_combobox->deselectItem();
}


void MasterWindow::on_any_input_changed()
{
    const auto state = main_switch->state(); 
    if(state == SwitchWidget::STATE::ON)
    {
        main_switch->set_state(SwitchWidget::STATE::LIMBO);
    }
}


void MasterWindow::on_bookmark_cmbobx_changed (const int selected_idx)
{
    const auto entries = manage_window->get_entries();
    const auto& [_,ip,port] = entries->at(selected_idx);
    ip_input->setText(ip);
    port_input->setText(port);
    logger->log(std::format("Bookmark changed to: {}",bookmark_cmbobx->getSelectedItem().toStdString()));
}

void MasterWindow::on_manage_clicked()
{
    logger->log("** Manage Bookmarks **");
    add(manage_window);
}
void MasterWindow::bookmarks_changed (const std::vector<ip_entry>* const bookmarks)
{
    bookmark_cmbobx->removeAllItems();
    for(const auto& name : *bookmarks|std::views::elements<0>) // I dont know why but gcc crashes when I use C++26's placeholder variables - fixed in gcc 15
    {
        bookmark_cmbobx->addItem(name);
    }
}

void MasterWindow::on_go_command_clicked ()
{
    const auto command = command_input->getText().toStdString();
    try{
    logger->log("$ "+command);
    if(proxy_handler->is_on())logger->log("└ WARNING: the global proxy settings is currently set. Some programs may ignore AppLauncher configs.",LoggerWidget::WARNING);
    if(const auto ret_command = proxy_handler->run(ip_input->getText().toStdString(), port_input->getText().toStdString(), command);
       ret_command != 0)logger->log(std::format("└ Returned: {}",ret_command),LoggerWidget::ERROR);
    
    }catch(const std::exception& err){logger->log(err.what(),LoggerWidget::ERROR);}
}

void MasterWindow::on_about_btn_clicked()
{
    const auto about_dialog = std::make_shared<AboutDialog>();
    add(about_dialog);
}

void MasterWindow::show_messagebox(const std::string_view title,const std::string_view message)
{
    constexpr auto messagebox_width = 230.f;
    std::lock_guard _ (messagebox_mutex);
    const auto messagebox_ptr = tgui::MessageBox::create(std::string(title),std::string(message),{"ok"});
    messagebox_ptr->setOrigin(0.5f,0.5f);
    messagebox_ptr->setPosition("50%", "50%");
    const auto& widgets = messagebox_ptr->getWidgets();
    const auto msgbox_label = std::static_pointer_cast<tgui::Label>(widgets.at(0));
    msgbox_label->setMaximumTextWidth(messagebox_width);
    messagebox_ptr->setSize(std::max(msgbox_label->getFullSize().x + 20, messagebox_width),
                            120);
    messagebox_ptr->onButtonPress(&tgui::MessageBox::close,messagebox_ptr);
    add(messagebox_ptr);
}