#include "logger.hpp"
#include <cstddef>
#include <limits>
#include <format>
#include <mutex>


LoggerWidget::LoggerWidget():
m_buffer ("<color=#ffffff>- **LOGS**\n</color>")
{
    const auto renderer = getRenderer();
    renderer->setBackgroundColor("#3a3a3a");
    onClick(&LoggerWidget::on_click,this);
    setText(m_buffer);
    setSize(250,170);
    getRenderer()->setScrollbarWidth(8);
}

void LoggerWidget::log(const std::string_view text,const MSG_TYPE type)
{
    const std::array color {"#00cfdf","green","#df00df","red"};
    m_buffer += std::format("<color={}>- {}</color>\n",color[static_cast<std::size_t>(type)],text);
    std::lock_guard _(m_log_mutex);
    setText(m_buffer);
    getScrollbar()->setValue(std::numeric_limits<unsigned int>::max());
    if(m_buffer.size()>100000)m_buffer.assign("** Buffer Cleaned **\n");
}

void LoggerWidget::on_click()
{
    static auto t = 0;
    t++;
    if(t==6)log(
R"(<color=#ff8800>Congrats: You found a Pizza!
// ""--.._
||  (_)  _ "-._
||    _ (_) °  '-.
||   (_) O __..-'
 \\__..--""</color>)");
}