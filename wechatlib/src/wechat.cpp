#include "WeChat.h"

#include <iostream>
#include <assert.h>

#pragma comment(lib, "ws2_32.lib")

// 网络相关
namespace network
{
    // 获取hostname
    std::string getHostname()
    {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        return hostname;
    }

    // 发送GET请求
    // 成功返回数据，失败返回空字符串
    std::string sendGetRequest(const char *url)
    {
        std::string response;

        // 初始化WinINet
        HINTERNET hInternet = InternetOpenA("WinINet", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (hInternet != NULL)
        {
            // 打开URL
            HINTERNET hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
            if (hConnect != NULL)
            {
                // 读取响应
                char buffer[1024];
                DWORD bytesRead;
                while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
                {
                    response.append(buffer, bytesRead);
                }

                // 关闭连接
                InternetCloseHandle(hConnect);
            }
            else
            {
                std::cerr << "[WeChatLib]Failed to open URL. Error code: " << GetLastError() << std::endl;
            }

            // 关闭WinINet
            InternetCloseHandle(hInternet);
        }
        else
        {
            std::cerr << "[WeChatLib]Failed to initialize WinINet. Error code: " << GetLastError() << std::endl;
        }

        return response;
    }

    // 发送POST请求
    // host 域名
    // path 接口路径
    // postData post数据
    // https 是否使用https
    // 成功返回数据，失败返回空字符串
    std::string sendPostRequest(const char *host, const char *path, const char *postData, bool https = true)
    {
        std::string response;

        HINTERNET hInternet = InternetOpenA("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (NULL == hInternet)
        {
            std::cerr << "[WeChatLib]Failed to open Internet. Error code: " << GetLastError() << std::endl;
            return response;
        }

        HINTERNET hHttpSession = InternetConnectA(hInternet, host, https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (NULL == hHttpSession)
        {
            std::cerr << "[WeChatLib]Failed to open session. Error code: " << GetLastError() << std::endl;

            // 关闭Internet
            InternetCloseHandle(hInternet);

            return response;
        }

        HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "POST", path, NULL, NULL, NULL, https ? INTERNET_FLAG_SECURE : 0, 1);
        if (NULL == hHttpRequest)
        {
            std::cerr << "[WeChatLib]Failed to open request. Error code: " << GetLastError() << std::endl;
        }
        else
        {
            char buffer[1024];
            DWORD bytesRead;
            while (InternetReadFile(hHttpRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
            {
                response.append(buffer, bytesRead);
            }

            if (response != "")
            {
                std::cout << "[WeChatLib]" << response << std::endl;
                response = "";
            }

            if (HttpSendRequestA(hHttpRequest, NULL, 0, (LPVOID)postData, strlen(postData)))
            {
                while (InternetReadFile(hHttpRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
                {
                    response.append(buffer, bytesRead);
                }
            }
            else
            {
                std::cerr << "[WeChatLib]Failed to send message. Error code: " << GetLastError() << std::endl;
            }

            // 关闭hHttpRequest
            InternetCloseHandle(hHttpRequest);
        }

        // 关闭hHttpSession
        InternetCloseHandle(hHttpSession);

        // 关闭Internet
        InternetCloseHandle(hInternet);

        return response;
    }
}

// 编码
namespace encodeing
{
    std::string GbkToUtf8(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
        wchar_t *wstr = new wchar_t[len + 1];
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char *str = new char[len + 1];
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        std::string strTemp = str;
        if (wstr)
            delete[] wstr;
        if (str)
            delete[] str;
        return strTemp;
    }

    std::string Utf8ToGbk(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
        wchar_t *wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char *szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        std::string strTemp(szGBK);
        if (wszGBK)
            delete[] wszGBK;
        if (szGBK)
            delete[] szGBK;
        return strTemp;
    }
}

void WeChat::init(const std::string &_corp_id, const std::string &_corp_sectet, bool _debug)
{
    corp_id = _corp_id;
    corp_sectet = _corp_sectet;
    debug = _debug;

    // 获取hostname
    auto hostname = network::getHostname();
    header = std::string("[") + hostname + "]";
}

bool WeChat::update_token()
{
    assert(corp_id != "" && corp_sectet != "");

    // 成功返回：
    // {"errcode":0,"errmsg":"ok","access_token":"MCSdHQIKf07w4v4j2zohV1_fzc8fPyVj1N3tmD0ph6ouM-B4Uh1yny7OxtjbGWQ8D93kPOskyTeRKhZrM2lz6mMP2T_oI5q_7pyJ1zX2RIkutXZxUsuVlIEO7qIDlacx4-uPON055bMElKzgCnywn4M3AO-XeXdsziDSjiU9PD4ekHXLWzhl_Gf8EJ4l1hXT9YPcUp3f387A3BsUpnl-ZA","expires_in":7200}
    auto res = network::sendGetRequest((std::string("https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid=") + corp_id + "&corpsecret=" + corp_sectet).c_str());

    // 提取token
    auto pos = res.find("\"access_token\":\"");
    if (pos != std::string::npos)
    {
        pos += strlen("\"access_token\":\"");
        token = res.substr(pos, res.find("\"", pos) - pos);
        return true;
    }
    else
    {
        std::cerr << "[WeChatLib]Failed to update token. Error code: " << res << std::endl;
        return false;
    }
}

std::string WeChat::get_token()
{
    // 获取当前时间戳
    auto now = time(NULL);

    // 如果token过期
    if (now - token_time >= 7200)
    {
        // 更新token
        if (update_token())
        {
            // 更新token时间戳
            token_time = now;
        }
        else
        {
            // 更新失败，返回空字符串
            return "";
        }
    }

    return token;
}

WeChat &WeChat::instance()
{
    static WeChat instance;
    return instance;
}

bool WeChat::send_message(const std::string &msg)
{
    if (msg == "")
    {
        return false;
    }

    // 获取token
    auto token = get_token();
    if (token == "")
    {
        return false;
    }

    // post路径
    std::string path = "/cgi-bin/message/send?access_token=" + token;
    if (debug)
    {
        path += "&debug=1";
    }

    // 转码
    auto _msg = encodeing::GbkToUtf8(msg.c_str());

    // post数据
    std::string sendValues = R"({"touser": "@all","msgtype": "text","agentid": "1000002","text": {"content": ")";
    sendValues += (_msg + R"("}})");

    // 成功返回:
    // {"errcode":0,"errmsg":"ok","msgid":"mrVtVXE39it1tWVvd57npK32GSboxHIjWGclIqAZHKUGjRWrqxdyz5VT78In4WFDg_4M9-3mpoDyKUkjBo6w8A"}
    auto res = network::sendPostRequest("qyapi.weixin.qq.com", path.c_str(), sendValues.c_str());

    if (debug)
    {
        std::cout << "[WeChatLib]发送数据:\n"
                  << sendValues << std::endl;
        std::cout << "[WeChatLib]返回数据:\n"
                  << res << std::endl;
    }

    // 检查是否成功
    // ok
    if (res.find("ok") != std::string::npos)
    {
        return true;
    }
    else
    {
        std::cerr << "[WeChatLib]Failed to send message. \n"
                  << res << std::endl;
        return false;
    }
}