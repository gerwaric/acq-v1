// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "app.h"

#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "util/json.h"

//#include <QtQml/QQmlExtensionPlugin>
//Q_IMPORT_QML_PLUGIN(AcquisitionPlugin)

void dumpQrcDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        qWarning() << "QRC path does not exist:" << path;
        return;
    };
    spdlog::info("Contents of {}:", path);
    const auto entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &entry : entries) {
        const QString fullPath = path + (path.endsWith('/') ? "" : "/") + entry;
        spdlog::info("--> {}", fullPath);
        QFileInfo file_info(fullPath);
        if (file_info.isDir()) {
            dumpQrcDirectory(fullPath);
        };
    };
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Force constrution of the App here.
    engine.singletonInstance<App *>("Acquisition", "App");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        [](const QUrl &url) {
            spdlog::error("Qml object creation failed: {}", url.toDisplayString());
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(QUrl("qrc:/qt/qml/Acquisition/main.qml"));

    return app.exec();
}
