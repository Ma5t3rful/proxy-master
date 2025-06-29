#include "switch_widget.hpp"
#include "TGUI/Color.hpp"
#include "TGUI/Renderers/ButtonRenderer.hpp"
#include "TGUI/Texture.hpp"
#include <array>
#include <functional>
#include <iterator>
#include <utility>

// Finally embed. No need to use xxd -i and CMake garbage for this.
unsigned char power_button_icon_on_png[]={
    #embed "power_button_icon_on.png"
};

unsigned char power_button_icon_off_png[]={
    #embed "power_button_icon_off.png"
};

unsigned char power_button_icon_limbo_png[]={
    #embed "power_button_icon_limbo.png"
};

SwitchWidget::SwitchWidget():
m_state(SwitchWidget::STATE::OFF)
{
    m_on_texture.loadFromMemory(power_button_icon_on_png, std::size(power_button_icon_on_png));
    m_off_texture.loadFromMemory(power_button_icon_off_png, std::size(power_button_icon_off_png));
    m_limbo_texture.loadFromMemory(power_button_icon_limbo_png, std::size(power_button_icon_limbo_png));
    
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

void SwitchWidget::set_state (const SwitchWidget::STATE state)
{
    //todo add limbo textures
    const auto textures = std::array<std::reference_wrapper<tgui::Texture>, 3> {m_limbo_texture,m_off_texture,m_on_texture};
    setImage(textures.at(std::to_underlying(state)+1));
    m_state = state;
}

auto SwitchWidget::state() const -> SwitchWidget::STATE
{
    return m_state;
}