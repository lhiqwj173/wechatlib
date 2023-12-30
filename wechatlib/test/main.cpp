﻿#include <iostream>
#include <string>

#include "wechat.h"
#pragma comment(lib, "wechat.lib")

int main()
{
    auto &wx = WeChat::instance();
    wx.init("xxx", "xxx");

    std::string msg;
    while (1)
    {
        std::cout << "请输入内容: ";
        std::getline(std::cin, msg);

        wx.send_message(msg);
    }

    return 0;
}