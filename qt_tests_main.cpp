#define CATCH_CONFIG_RUNNER

#include <QApplication>
#include <catch2/catch.hpp>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    const int catchResult = Catch::Session().run(argc, argv);

    return (catchResult < 0xff ? catchResult : 0xff);
}
