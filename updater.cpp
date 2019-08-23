//
// Created by Dmitry Khrykin on 2019-08-13.
//

#include <cstdlib>
#include <string>
#include <iostream>

std::string commandWithArgsApplied(const std::string &command,
                                   const std::string &args) {
    return command
           + (command.empty() ? "" : " ")
           + args;
}

int main(int argc, char *argv[]) {
    using namespace std;

    if (argc != 3) {
        return 1;
    }

    string pid = argv[1];
    string launchDestination = "\"" + string{argv[2]} + "\"";
    string launchCommand;
    string killCommand = "pkill -9";

#if defined(__APPLE__)
    launchCommand = "open";
    killCommand = "kill -9";
#endif

    auto kill = commandWithArgsApplied(killCommand, pid);
    auto killResult = std::system(kill.c_str());
    std::cout << "kill: " << kill << " result: " << killResult << std::endl;

    if (killResult != 0)
        return 1;

    auto command = commandWithArgsApplied(launchCommand, launchDestination);

    auto launchResult = std::system(command.c_str());
    std::cout << "open: " << command << " result: " << launchResult << std::endl;

    return launchResult != 0;
}