#pragma once
#include "TGUI/Texture.hpp"
#include "TGUI/Widgets/BitmapButton.hpp"


// TODO: Add a "limbo" state

class SwitchWidget:
public tgui::BitmapButton
{
    public:
    enum class STATE{
        LIMBO=-1,
        OFF,//0
        ON,//1
    };
    SwitchWidget();
    void set_state (const STATE);
    STATE state () const;
    
    private:
    tgui::Texture m_on_texture;
    tgui::Texture m_off_texture;
    tgui::Texture m_limbo_texture;
    STATE m_state;
};