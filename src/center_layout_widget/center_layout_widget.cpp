#include "center_layout_widget.hpp"
#include "TGUI/Layout.hpp"
#include "TGUI/String.hpp"
#include "TGUI/Widget.hpp"
#include "TGUI/Widgets/GrowVerticalLayout.hpp"
#include <memory>
#include <print>

CenterLayoutWidget::CenterLayoutWidget():
m_vertlayout(tgui::GrowVerticalLayout::create())
{
    setSize("100%");
    std::println("Center constructed");
    m_vertlayout->setOrigin(0,0.5);
    m_vertlayout->setPosition("0","50%");
    add(m_vertlayout);
}

void CenterLayoutWidget::add_widget(const std::shared_ptr<tgui::Widget>& widget,const tgui::String& str)
{
    m_vertlayout->add(widget,str);
}

void CenterLayoutWidget::set_spacing(const float space)
{
    m_vertlayout->getRenderer()->setSpaceBetweenWidgets(space);
}