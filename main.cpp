// Copyright 2021 Artyom Martirosyan
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>

#include <boost/regex.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using boost::regex;

using boost::gregorian::date;
using boost::gregorian::from_simple_string;

using boost::posix_time::ptime;
using boost::posix_time::time_duration;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid number of command lind arguments" << std::endl;
        return 0;
    }
    std::ifstream inputFile(argv[1], std::ifstream::in);
    if (!inputFile.is_open()) {
        std::cout << "Unable to open file \"" << argv[1] << "\"" << std::endl;
        return 0;
    }

    std::string outputFileName(std::string(argv[1]) + ".rpt");
    std::ofstream outputFile;
    outputFile.open(outputFileName.c_str());

    boost::smatch match;

    std::string sdate("([0-9]{4})-([0-9]{1,2})-([0-9]{1,2}) ");
    std::string stime("([0-9]{2}):([0-9]{2}):([0-9]{2})");
    std::string sboot("(.*log.c.166.*)");
    std::string sdone("(.*oejs.AbstractConnector:Started SelectChannelConnector.*)");

    regex rboot(sdate + stime + sboot);
    regex rdone(sdate + stime + sdone);

    int ln = 1;
    bool booting = false;
    std::string line;
    ptime time1, time2;

    while (getline(inputFile, line)) {
        if (regex_match(line, match, rboot)) {
            if (booting) {
                outputFile << "**** Incomplete boot **** \n" << std::endl;
            }
            date d(from_simple_string(match[0]));
            ptime temp(d, time_duration(atoi(match[4].str().c_str()),
                atoi(match[5].str().c_str()), atoi(match[6].str().c_str())));
            time1 = temp;

            outputFile << "**** Device boot ****" << std::endl;
            outputFile << ln << "(" << argv[1] << "): ";
            outputFile << match[1] << "-" << match[2] << "-" << match[3] << " ";
            outputFile << match[4] << ":" << match[5] << ":" << match[6] << " ";
            outputFile << "Boot Start" << std::endl;
            booting = true;
        } else if (regex_match(line, match, rdone)) {
            if (booting) {
                date d(from_simple_string(match[0]));
                ptime tempT(d, time_duration(atoi(match[4].str().c_str()), atoi(match[5].str().c_str()), atoi(match[6].str().c_str())));
                time2 = tempT;

                time_duration td = time2 - time1;

                outputFile << ln << "(" << argv[1] << "): ";
                outputFile << match[1] << "-" << match[2]
                                << "-" << match[3] << " ";
                outputFile << match[4] << ":" << match[5]
                                << ":" << match[6] << " ";
                outputFile << "Boot Completed" << std::endl;

                outputFile << "\t" << "Boot Time: ";
                outputFile << td.total_milliseconds() << "ms \n" << std::endl;

                booting = false;
            } else {
                outputFile << "**** Unexpected boot ****\n" << std::endl;
            }
        }
        ln++;
    }

    return 0;
}
