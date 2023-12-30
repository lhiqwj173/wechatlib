#pragma once

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include <string>

class WeChat
{

public:
    // 单例
    static WeChat &instance();
    ~WeChat() = default;

    // 发送消息
    // 返回值：是否发送成功
    bool send_message(const std::string &msg);

    // 初始化
    // 参数：corp_id        企业ID
    // 参数：corp_sectet    企业密钥
    void init(const std::string &_corp_id, const std::string &_corp_sectet, bool _debug = false);
};