//
// Created by ekaterina on 05.07.2021.
//

#include "Scanner.hpp"

Scanner::Scanner()
{
}

void Scanner::Analyzer(filesystem::path &path, std::ostream &out) {
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
}



void Scanner::printAnalytics(filesystem::path path) {
    Analyzer(path, cout);
    cout << endl << "====== Scan result ======" << endl << endl;
    cout << "Processed files: " << ProcessedFiles << endl;
    cout << "JS detects:      " <<  Js << endl;
    cout << "MacOs detects:   " <<  Mac << endl;
    cout << "Unix detects:    " << Unix << endl;
    cout << "Errors:          " << Error << endl;
    cout << "===========================" << endl;
}
