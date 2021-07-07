//
// Created by ekaterina on 05.07.2021.
//

#include "Scanner.hpp"

Scanner::Scanner()
{
}

void Scanner::Analyzer(filesystem::path &path) {
    auto start = std::chrono::high_resolution_clock::now();
    if(!filesystem::is_directory(path)){
        throw runtime_error("Works, but not properly");
    }
    if(!filesystem::exists(path)){
        throw runtime_error("Directory does not exist");
    }
    if(filesystem::is_symlink(path)){
        throw runtime_error("Can not examine symlink");
    }
    if(path.empty()){
        throw runtime_error("Directory is empty");
    }
    for (const filesystem::directory_entry &x : filesystem::directory_iterator{path}) {
        if (!filesystem::is_directory(x.path())) {
            ifstream in(x.path());
            string tmp;
            if (in.is_open()) {
                getline(in, tmp);
                if (tmp.find(MacSuspicious) != tmp.npos) { Mac++; }
                if (tmp.find(JsSuspicious) != tmp.npos && x.path().extension() == ".js") { Js++; }
                if (tmp.find(UnixSuspicious) != tmp.npos) { Unix++;}
            } else {Error++;}
            ProcessedFiles++;
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    elapsed_seconds +=std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
}



string Scanner::printAnalytics(filesystem::path path) {
    Analyzer(path);
    Output =  "\n====== Scan result ======\n";
    Output += "Processed files: " + to_string(ProcessedFiles) + '\n' ;
    Output += "JS detects:      " + to_string(Js) + '\n' ;
    Output += "MacOs detects:   " + to_string(Mac) + '\n' ;
    Output += "Unix detects:    " + to_string(Unix) + '\n' ;
    Output += "Errors:          " + to_string(Error) + '\n' ;
    Output += "Execution time:  " + to_string(elapsed_seconds) + " mcrs" + '\n';
    Output += "===========================\n";
    return Output;
}
