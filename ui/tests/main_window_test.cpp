//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QWidget>
#include <QtTest/QTest>
#include <catch2/catch.hpp>

#include <QBuffer>
#include <QDataStream>
#include <QDebug>

#include <iostream>

#include "applicationsettings.h"
#include "mainwindow.h"

std::ostream &operator<<(std::ostream &os, const QRect &r) {
    os << "QRect("
       << r.x() << ", "
       << r.y() << ", "
       << r.width() << ", "
       << r.height() << ")";

    return os;
}

TEST_CASE("MainWindow") {
    SECTION("initial rect") {

        WindowGeometryManager::resetSavedGeometry();

        SECTION("when first opened") {
            auto w = MainWindow();
            REQUIRE(w.width() == ApplicationSettings::windowInitialWidth);
        }

        SECTION("should persist it's previous rect") {
            auto w = MainWindow();
            auto testRect = QRect(
                100,
                100,
                ApplicationSettings::windowMinimumWidth,
                ApplicationSettings::windowMinimumHeight);

            w.setGeometry(testRect);
            w.close();

            auto w2 = MainWindow();
            auto lastGeometry = w2.geometry();

            REQUIRE(lastGeometry == testRect);
        }

        WindowGeometryManager::resetSavedGeometry();
    }
}