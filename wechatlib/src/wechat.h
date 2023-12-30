#pragma once

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include <string>

class WeChat
{
private:
    // 禁用
    WeChat() = default;
    WeChat(const WeChat &) = delete;
    WeChat &operator=(const WeChat &) = delete;

    // 消息头
    std::string header = "";

    // id
    std::string corp_id = "";
    // 密钥
    std::string corp_sectet = "";
    // 是否开启调试
    bool debug = false;

    // token时间戳
    time_t token_time = 0;

    // token
    std::string token = "";

    // 更新token
    // 返回值：是否更新成功
    bool update_token();

    // 获取token
    std::string get_token();

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