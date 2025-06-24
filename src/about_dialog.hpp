#pragma once
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/Label.hpp"
#include "TGUI/Widgets/RichTextLabel.hpp"
#include <TGUI/Widgets/ChildWindow.hpp>


class AboutDialog
:public tgui::ChildWindow
{
    public:
    AboutDialog();
    ~AboutDialog();
    private:
    const tgui::Label::Ptr title_lbl;
    const tgui::Label::Ptr description_lbl;
    const tgui::RichTextLabel::Ptr source_and_donation;
    const tgui::Button::Ptr ok_btn;
};