#include "about_dialog.hpp"
#include "TGUI/Cursor.hpp"
#include "TGUI/Layout.hpp"
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/Group.hpp"
#include "TGUI/Widgets/Label.hpp"
#include <format>
#include <memory>
#include "cmake_version.hpp"


AboutDialog::AboutDialog():
description_lbl(tgui::Label::create(std::format(
"This app lets you easily manage and set your user-wide proxy on Linux.\n"
"It's mostly developed by just using modern C++ and its standard library. Only the GUI is created with TGUI.\n"
"This software is free software and It's developed under the terms of MIT licence.\n"
"Made with ♥\n"
"Proxy Master version: {}\n"
"TGUI version: {}.{}\n",ProxyMaster::version(),TGUI_VERSION_MAJOR, TGUI_VERSION_MINOR
))),
ok_btn(tgui::Button::create("OK")),
source_and_donation(tgui::RichTextLabel::create("<u>Source and Donation</u>"))
{
    setOrigin(0.5,0.5);
    setPosition("50%","50%");
    setSize(320,240);
    setTitle("About ProxyMaster");
    const auto ok_btn_cntainr = std::make_shared<tgui::Group>();
    ok_btn_cntainr->setHeight(30);
    ok_btn->onClick(&tgui::ChildWindow::close,this);
    ok_btn->setSize(70, 28);
    ok_btn->setOrigin(0.5,0.5);
    ok_btn->setPosition("50%","50%");
    source_and_donation->onClick(&std::system,"xdg-open https://github.com/Ma5t3rful");
    description_lbl->setAutoLayout(tgui::AutoLayout::Fill);
    source_and_donation->getRenderer()->setTextColor("#225dFF");
    source_and_donation->setMouseCursor(tgui::Cursor::Type::Hand);
    source_and_donation->setAutoLayout(tgui::AutoLayout::Bottom);

    ok_btn_cntainr->setAutoLayout(tgui::AutoLayout::Bottom);
    ok_btn_cntainr->add(ok_btn);
    add(description_lbl);
    add(ok_btn_cntainr);
    add(source_and_donation);
}

AboutDialog::~AboutDialog()
{
    
}