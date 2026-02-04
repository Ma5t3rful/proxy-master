#include "center_layout_widget.hpp"
#include "TGUI/Layout.hpp"
#include "TGUI/String.hpp"
#include "TGUI/Widget.hpp"
#include "TGUI/Widgets/Group.hpp"
#include "TGUI/Widgets/GrowVerticalLayout.hpp"
#include <memory>

CenterLayoutWidget::CenterLayoutWidget():
m_vertlayout(tgui::GrowVerticalLayout::create())
{
    setSize("100%");
    m_vertlayout->setOrigin(0,0.5);
    m_vertlayout->setPosition("0","50%");
    tgui::Group::add(m_vertlayout);
}

void CenterLayoutWidget::add_widget(const std::shared_ptr<tgui::Widget>& widget,const tgui::String& str)
{
    m_vertlayout->add(widget,str);
}

void CenterLayoutWidget::add(const std::shared_ptr<tgui::Widget>& widget,const tgui::String& str)
{
    m_vertlayout->add(widget,str);
}

void CenterLayoutWidget::set_spacing(const float space)
{
    m_vertlayout->getRenderer()->setSpaceBetweenWidgets(space);
}