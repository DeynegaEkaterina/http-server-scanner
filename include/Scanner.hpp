//
// Created by ekaterina on 04.07.2021.
//

#ifndef SCANNER_SCANNER_HPP
#define SCANNER_SCANNER_HPP

#include "filesystem"
#include "fstream"
#include "iostream"

using namespace std;
class Scanner{

public:
    explicit Scanner();
    static void Analyzer(filesystem::path &path);
    static string printAnalytics(filesystem::path path);

private:
    inline static string Output{};
    inline static const string JsSuspicious = "<script>evil_script()</script>";
    inline static const string UnixSuspicious = "rm -rf ~/Documents";
    inline static const string MacSuspicious = "system(\"launchctl load /Library/LaunchAgents/com.malware.agent\")";
    inline static int Js = 0;
    inline static int Mac = 0;
    inline static int Unix = 0;
    inline static int ProcessedFiles = 0;
    inline static int Error = 0;
    inline static long elapsed_seconds = 0;
};




#endif //SCANNER_SCANNER_HPP
