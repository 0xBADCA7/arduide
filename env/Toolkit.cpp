/**
 * \file Toolkit.cpp
 * \author Denis Martinez
 */

#include "Toolkit.h"

#include <QStringList>
#include <QDir>
#include <QFile>

#include "../env/Board.h"

QStringList Toolkit::findSketchesInDirectory(const QString &directory)
{
    QStringList sketches;
    QDir dir(directory);
    foreach (const QString &sketch, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
    {
        static const QString format = "%0/%0.pde";
        if (dir.exists(format.arg(sketch)))
            sketches.append(sketch);
    }
    return sketches;
}

QStringList Toolkit::findExampleCategories()
{
    QDir exampleDir(Settings::instance().arduinoPath() + "/examples");
    return exampleDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
}

QStringList Toolkit::findExamples(const QString &category)
{
    QString categoryDir(Settings::instance().arduinoPath() + "/examples/" + category);
    return findSketchesInDirectory(categoryDir);
}

QString Toolkit::exampleFileName(const QString &category, const QString &example)
{
    static const QString format = "%0/examples/%1/%2/%2.pde";
    return format.arg(Settings::instance().arduinoPath()).arg(category).arg(example);
}

QString Toolkit::referencePath()
{
    return QDir(Settings::instance().arduinoPath()).filePath("reference");
}

QString Toolkit::hardwarePath()
{
    return QDir(Settings::instance().arduinoPath()).filePath("hardware");
}

QString Toolkit::avrPath()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_DARWIN)
    return QDir(hardwarePath()).filePath("tools/avr/bin");
#else
    // the AVR toolchain should be already present in the PATH
    return QString();
#endif
}

QString Toolkit::avrTool(Toolkit::AVRTool tool)
{
    QString path = avrPath();
    QString toolName;
    switch (tool)
    {
    case AvrGcc:
        toolName = "avr-gcc";
        break;
    case AvrGxx:
        toolName = "avr-g++";
        break;
    case AvrAr:
        toolName = "avr-ar";
        break;
    case AvrObjcopy:
        toolName = "avr-objcopy";
        break;
    default:
        return QString();
    }

    if (path.isNull())
        return toolName;
    else
        return QDir(path).filePath(toolName);
}

QStringList Toolkit::avrCFlags(const Board *board)
{
    QStringList cflags;
    cflags
        << "-g"
        << "-Os"
        << "-w"
        << "-fno-exceptions"
        << "-ffunction-sections"
        << "-fdata-sections"
        << QString("-mmcu=%0").arg(board->attribute("build.mcu"))
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-DARDUINO=%0").arg(ARDUINO_REVISION);
    return cflags;
}

QStringList Toolkit::avrCxxFlags(const Board *board)
{
    return avrCFlags(board);
}

QStringList Toolkit::avrSFlags(const Board *board)
{
    QStringList sflags;
    sflags
        << "-g"
        << "-assembler-with-cpp"
        << QString("-mmcu=%0").arg(board->attribute("build.mcu"))
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-DARDUINO=%0").arg(ARDUINO_REVISION);
    return sflags;
}

QStringList Toolkit::avrLdFlags(const Board *board)
{
    QStringList ldflags;
    ldflags
        << "-Os"
        << "-Wl,--gc-sections"
        << QString("-mmcu=%0").arg(board->attribute("build.mcu"));
    return ldflags;
}

QString Toolkit::corePath(const Board *board)
{
    return QDir(hardwarePath()).filePath(QString("cores/%0").arg(board->attribute("build.core")));
}

QString Toolkit::avrdudePath()
{
    return QDir(hardwarePath()).filePath("tools/avrdude");
}

QStringList Toolkit::avrdudeFlags(const Board *board)
{
    QStringList flags;
    flags
        << "-C"
#if defined(Q_OS_WIN32) || defined(Q_OS_DARWIN)
        << QDir(hardwarePath()).filePath("tools/avr/etc/avrdude.conf")
#else
        << QDir(hardwarePath()).filePath("tools/avrdude.conf")
#endif
        << QString("-p%0").arg(board->attribute("build.mcu"));
    return flags;
}