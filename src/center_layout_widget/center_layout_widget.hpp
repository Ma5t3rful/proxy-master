#pragma once
#include "TGUI/Layout.hpp"
#include "TGUI/Widget.hpp"
#include "TGUI/Widgets/Group.hpp"
#include "TGUI/Widgets/GrowVerticalLayout.hpp"
#include <memory>


class CenterLayoutWidget
:public tgui::Group
{
    public:
    CenterLayoutWidget();
    
    void add (const std::shared_ptr<tgui::Widget>&,const tgui::String& = "") override;
    
    [[deprecated("Use add() instead.")]]
    void add_widget (const std::shared_ptr<tgui::Widget>&,const tgui::String& = "");
    
    void set_spacing (const float);
    // void add (const std::shared_ptr<tgui::Widget>&,const tgui::String&) ;
    private:
    tgui::GrowVerticalLayout::Ptr m_vertlayout;
};
