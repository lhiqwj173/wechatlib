#include <iostream>

#include "wechat.h"
#pragma comment(lib, "wechat.lib")

int main()
{
    auto &wx = WeChat::instance();
    // wx.init("wwbdf90f755eadc2b3", "NFf11bf7u7UjJdw-_YsdqKwU4bBlIAscn976GRQQwEc");

    std::string msg;
    while (1)
    {
        std::cout << "请输入内容: ";
        std::getline(std::cin, msg);

        wx.send_message(msg);
    }

    return 0;
}