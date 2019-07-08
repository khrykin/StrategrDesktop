//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QWidget>
#include <QtTest/QTest>
#include <catch2/catch.hpp>

#include "mainwindow.h"

TEST_CASE("MainWindow") {
    auto w = MainWindow();

    QTest::mouseClick(w.button(), Qt::LeftButton);

    REQUIRE(w.button()->text().toStdString() == "bitch you did it");
}