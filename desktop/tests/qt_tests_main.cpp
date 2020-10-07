#define CATCH_CONFIG_RUNNER

#include <QApplication>
#include <catch2/catch.hpp>

#include "application.h"
#include "backends.h"

void setupCredentials() {
    QCoreApplication::setOrganizationName("Dmitry Khrykin");
    QCoreApplication::setOrganizationDomain("khrykin.com");
    QCoreApplication::setApplicationName("StrategrTest");
}

int main(int argc, char *argv[]) {
    setupCredentials();
    setupBackends();

    Application a(argc, argv);
    const int catchResult = Catch::Session().run(argc, argv);

    return (catchResult < 0xff ? catchResult : 0xff);
}
