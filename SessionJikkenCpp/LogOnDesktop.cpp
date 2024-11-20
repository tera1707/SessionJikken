#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <shlobj.h>
#include <time.h>
#include <thread>   // std::this_thread::get_id()を使うのに必要
#include <fstream>  // std::wofstreamを使うのに必要
#include <filesystem>
#include <mutex>

#include "LogOnDesktop.h"

// ※このコードをブラウザからコピーしてVisualStudioに貼り付けると、ビルドしたときに
// 　文字コード云々でエラーになったことがあった。そういうときは、コード中のコメントを全部消すとエラー消えた。

std::filesystem::path LogOnDesktop::AppendLogFilePathOnDesktopString(std::wstring filename)
{
    WCHAR* buf = nullptr;
    std::filesystem::path desktop;

    auto hr = SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT, NULL, &buf);

    if (SUCCEEDED(hr)) {
        desktop = buf;
        desktop.append(filename);
    }

    CoTaskMemFree(buf);

    return desktop;
}

std::wstring LogOnDesktop::GetTimeString()
{
    WCHAR buf[64];
    auto t = time(nullptr);
    auto tmv = tm();
    auto error = localtime_s(&tmv, &t); // ローカル時間(タイムゾーンに合わせた時間)を取得

    wcsftime(buf, 64, L"%Y/%m/%d %H:%M:%S", &tmv);
    auto aika = std::wstring(buf);

    return aika;
}

void LogOnDesktop::WriteLine(std::wstring txt)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto desktop = AppendLogFilePathOnDesktopString(L"mylog.log");

    DWORD processId = GetCurrentProcessId();

    auto aika = GetTimeString();

    // 現在のスレッドIDを出力
    auto thId = std::this_thread::get_id();

    // ファイルを開く(なければ作成)
    std::wofstream ofs(desktop, std::ios::app);

    if (!ofs)
        return;

    // 現在時刻とスレッドIDを付けたログをファイルに書き込み
    ofs << aika << L" " << processId << L" " << thId << L" " << txt << std::endl;

    // ファイル閉じる
    ofs.close();
}
