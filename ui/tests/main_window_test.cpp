//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QWidget>
#include <QtTest/QTest>
#include <catch2/catch.hpp>

#include <QDataStream>
#include <QBuffer>
#include <QDebug>

#include <iostream>

#include "mainwindow.h"
#include "applicationsettings.h"

std::ostream &operator<<(std::ostream &os, const QRect &r) {
    os << "QRect(" << r.x() << ", " << r.y() << ", " << r.width() << ", " << r.height() << ")";
    return os;
};


TEST_CASE("MainWindow") {
    SECTION("initial rect") {

        WindowGeometryManager::resetSavedGeometry();

        SECTION("when first opened") {
            auto w = MainWindow();
            REQUIRE(w.width() == ApplicationSettings::windowInitialWidth);
        }

        SECTION("should persist it's previous rect") {
            auto w = new MainWindow();
            auto testRect = QRect(
                    0,
                    0,
                    ApplicationSettings::windowMinimumWidth,
                    ApplicationSettings::windowMinimumHeight);

            w->setGeometry(testRect);
            delete w;

            auto w2 = new MainWindow();
            auto lastGeometry = w2->geometry();
            delete w2;

            REQUIRE(lastGeometry == testRect);
        }

        WindowGeometryManager::resetSavedGeometry();
    }
}