// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "app.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG);

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDir>
#include <QString>
#include <QUrl>

#include <QMetaEnum>

#ifdef QT_DEBUG
constexpr auto DEFAULT_LOG_LEVEL = spdlog::level::debug;
#else
constexpr auto DEFAULT_LOG_LEVEL = spdlog::level::info;
#endif

void dumpQrcDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        qWarning() << "QRC path does not exist:" << path;
        return;
    }
    spdlog::info("Contents of {}:", path);
    const auto entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &entry : entries) {
        const QString fullPath = path + (path.endsWith('/') ? "" : "/") + entry;
        spdlog::info("--> {}", fullPath);
        QFileInfo file_info(fullPath);
        if (file_info.isDir()) {
            dumpQrcDirectory(fullPath);
        }
    }
}

int main(int argc, char *argv[])
{
    // Set the default logging level before we do anything.
    spdlog::set_level(DEFAULT_LOG_LEVEL);

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

    engine.load(QUrl("qrc:/qt/qml/Acquisition/Main.qml"));

    return app.exec();
}
