#pragma once
#include "TGUI/Texture.hpp"
#include "TGUI/Widgets/BitmapButton.hpp"


// TODO: Add a "limbo" state

class SwitchWidget:
public tgui::BitmapButton
{
    public:
    SwitchWidget();
    void set_state (const bool);
    bool state () const;
    
    private:
    tgui::Texture m_on_texture;
    tgui::Texture m_off_texture;
    bool m_state;
};