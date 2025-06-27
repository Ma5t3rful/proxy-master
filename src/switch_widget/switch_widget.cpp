#include "switch_widget.hpp"
#include "TGUI/Color.hpp"
#include "TGUI/Renderers/ButtonRenderer.hpp"
#include "TGUI/Texture.hpp"
#include <functional>

// Finally embed. No need to use xxd -i and CMake garbage for this.
unsigned char power_button_icon_on_png[]={
    #embed "power_button_icon_on.png"
};

unsigned char power_button_icon_off_png[]={
    #embed "power_button_icon_off.png"
};


SwitchWidget::SwitchWidget():
m_state(false)
{
    m_on_texture.loadFromMemory(power_button_icon_on_png, sizeof(power_button_icon_on_png));
    m_off_texture.loadFromMemory(power_button_icon_off_png, sizeof(power_button_icon_off_png));
    setImageScaling(1);
    setImage(m_off_texture);
    for(const auto renderer = getRenderer();const auto &f: {
        &tgui::ButtonRenderer::setBackgroundColor,
        &tgui::ButtonRenderer::setBackgroundColorDown,
        &tgui::ButtonRenderer::setBackgroundColorHover,
        &tgui::ButtonRenderer::setBackgroundColorDownFocused,
        &tgui::ButtonRenderer::setBackgroundColorFocused,
        &tgui::ButtonRenderer::setBackgroundColorDownHover,
        &tgui::ButtonRenderer::setBorderColorFocused,
        &tgui::ButtonRenderer::setBorderColor,
        &tgui::ButtonRenderer::setBorderColorDown,
        &tgui::ButtonRenderer::setBorderColorHover,
    }) std::invoke(f,renderer,tgui::Color("transparent")); //makes everything transparent so the button will be hidden and only the switch is shown.
}

void SwitchWidget::set_state (const bool state)
{
    setImage(state?m_on_texture:m_off_texture);
    m_state = state;
}

bool SwitchWidget::state() const 
{
    return m_state;
}