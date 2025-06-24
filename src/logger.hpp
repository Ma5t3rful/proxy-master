#pragma once
#include "TGUI/Texture.hpp"
#include <TGUI/Widgets/RichTextLabel.hpp>
#include <mutex>
#include <string>

class LoggerWidget
:public tgui::RichTextLabel
{
    public:
    enum MSG_TYPE{
        INFO,
        SUCCESS,
        WARNING,
        ERROR,
    };
    public:
    LoggerWidget();
    //thread-safe logginhg function
    void log (const std::string_view text ,const MSG_TYPE type = INFO);
    //std::shared_ptr<const tgui::RichTextLabel> get ();
    private:
    std::mutex m_log_mutex;
    std::string m_buffer;//tgui rich text needs a seperate buffer for storing text. It does not have a method to append text to it
    const tgui::Texture m_on_texture;
    const tgui::Texture m_off_texture;
    void on_click();
};