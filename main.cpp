/*****************************************************************************
* gta5sync GRAND THEFT AUTO V SYNC
* Copyright (C) 2016-2017 Syping
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "TranslationClass.h"
#include "SnapmaticPicture.h"
#include "ProfileDatabase.h"
#include "DatabaseThread.h"
#include "SavegameDialog.h"
#include "OptionsDialog.h"
#include "PictureDialog.h"
#include "UserInterface.h"
#include "CrewDatabase.h"
#include "SavegameData.h"
#include "IconLoader.h"
#include "AppEnv.h"
#include "config.h"
#include <QDesktopWidget>
#include <QStringBuilder>
#include <QStyleFactory>
#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include <QTranslator>
#include <QFileInfo>
#include <QSysInfo>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QFont>
#include <QFile>

#ifdef GTA5SYNC_WIN
#include "windows.h"
#include <iostream>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(GTA5SYNC_APPSTR);
    a.setApplicationVersion(GTA5SYNC_APPVER);
    a.setQuitOnLastWindowClosed(false);

    QSettings settings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    settings.beginGroup("Startup");

    bool isFirstStart = settings.value("IsFirstStart", true).toBool();
    bool customStyle = settings.value("CustomStyle", false).toBool();
    QString appStyle = settings.value("AppStyle", "Default").toString();

    if (customStyle)
    {
        if (QStyleFactory::keys().contains(appStyle, Qt::CaseInsensitive))
        {
            a.setStyle(QStyleFactory::create(appStyle));
        }
    }

#ifdef GTA5SYNC_WIN
#if QT_VERSION >= 0x050400
    bool alwaysUseMessageFont = settings.value("AlwaysUseMessageFont", false).toBool();
    if (QSysInfo::windowsVersion() >= 0x0080 || alwaysUseMessageFont)
    {
        // Get Windows Font
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        LOGFONTW uiFont = ncm.lfMessageFont;
        QString uiFontStr(QString::fromStdWString(std::wstring(uiFont.lfFaceName)));

#ifdef GTA5SYNC_DEBUG
        qDebug() << QApplication::tr("Font") << QApplication::tr("Selected Font: %1").arg(uiFontStr);
#endif

        // Set Application Font
        QFont appFont(uiFontStr, 9);
        a.setFont(appFont);
    }
#endif
#endif

    QString pluginsDir = AppEnv::getPluginsFolder();
    if (QFileInfo(pluginsDir).exists())
    {
        a.addLibraryPath(pluginsDir);
    }

    TCInstance->initUserLanguage();
    TCInstance->loadTranslation(&a);

    if (isFirstStart)
    {
        QMessageBox::StandardButton button = QMessageBox::information(a.desktop(), QString("%1 %2").arg(GTA5SYNC_APPSTR, GTA5SYNC_APPVER), QApplication::tr("<h4>Welcome to %1!</h4>You want to configure %1 before you start using it?").arg(GTA5SYNC_APPSTR), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (button == QMessageBox::Yes)
        {
            ProfileDatabase profileDB;
            OptionsDialog optionsDialog(&profileDB);
            optionsDialog.setWindowIcon(IconLoader::loadingAppIcon());
            optionsDialog.show();
            optionsDialog.exec();
        }
        settings.setValue("IsFirstStart", false);
    }

    settings.endGroup();

    QStringList applicationArgs = a.arguments();
    QString selectedAction;
    QString arg1;
    applicationArgs.removeAt(0);

    for (QString currentArg : applicationArgs)
    {
        QString reworkedArg;
        if (currentArg.left(9) == "-showpic=" && selectedAction == "")
        {
            reworkedArg = currentArg.remove(0,9);
            arg1 = reworkedArg;
            selectedAction = "showpic";
        }
        else if (currentArg.left(9) == "-showsgd=" && selectedAction == "")
        {
            reworkedArg = currentArg.remove(0,9);
            arg1 = reworkedArg;
            selectedAction = "showsgd";
        }
        else if (selectedAction == "")
        {
            QFile argumentFile(currentArg);
            QFileInfo argumentFileInfo(argumentFile);
            if (argumentFile.exists())
            {
                QString argumentFileName = argumentFileInfo.fileName();
                QString argumentFileType = argumentFileName.left(4);
                QString argumentFileExt = argumentFileName.right(4);

                if (argumentFileType == "PGTA" || argumentFileExt == ".g5e")
                {
                    arg1 = currentArg;
                    selectedAction = "showpic";
                }
                else if (argumentFileType == "SGTA")
                {
                    arg1 = currentArg;
                    selectedAction = "showsgd";
                }
                else if (argumentFileType == "MISR")
                {
                    arg1 = currentArg;
                    selectedAction = "showsgd";
                }
            }
        }
    }

    if (selectedAction == "showpic")
    {
        CrewDatabase crewDB;
        ProfileDatabase profileDB;
        DatabaseThread threadDB(&crewDB);
        PictureDialog picDialog(true, &profileDB, &crewDB);
        SnapmaticPicture picture;

        bool readOk = picture.readingPictureFromFile(arg1);
        picDialog.setWindowIcon(IconLoader::loadingAppIcon());
        picDialog.setSnapmaticPicture(&picture, readOk);
#ifndef Q_OS_LINUX
        picDialog.setWindowFlags(picDialog.windowFlags()^Qt::Dialog^Qt::Window);
#endif

        int crewID = picture.getSnapmaticProperties().crewID;
        if (crewID != 0) { crewDB.addCrew(crewID); }
        if (!readOk) { return 1; }

        QObject::connect(&threadDB, SIGNAL(crewNameFound(int, QString)), &crewDB, SLOT(setCrewName(int, QString)));
        QObject::connect(&threadDB, SIGNAL(crewNameUpdated()), &picDialog, SLOT(crewNameUpdated()));
        QObject::connect(&threadDB, SIGNAL(playerNameFound(int, QString)), &profileDB, SLOT(setPlayerName(int, QString)));
        QObject::connect(&threadDB, SIGNAL(playerNameUpdated()), &picDialog, SLOT(playerNameUpdated()));
        QObject::connect(&threadDB, SIGNAL(finished()), &a, SLOT(quit()));
        QObject::connect(&picDialog, SIGNAL(endDatabaseThread()), &threadDB, SLOT(terminateThread()));
        threadDB.start();

        picDialog.show();

        return a.exec();
    }
    else if (selectedAction == "showsgd")
    {
        SavegameDialog savegameDialog;
        SavegameData savegame;

        bool readOk = savegame.readingSavegameFromFile(arg1);
        savegameDialog.setWindowIcon(IconLoader::loadingAppIcon());
        savegameDialog.setSavegameData(&savegame, arg1, readOk);
        savegameDialog.setWindowFlags(savegameDialog.windowFlags()^Qt::Dialog^Qt::Window);

        if (!readOk) { return 1; }

        a.setQuitOnLastWindowClosed(true);
        savegameDialog.show();

        return a.exec();
    }

    CrewDatabase crewDB;
    ProfileDatabase profileDB;
    DatabaseThread threadDB(&crewDB);

    QObject::connect(&threadDB, SIGNAL(crewNameFound(int,QString)), &crewDB, SLOT(setCrewName(int, QString)));
    QObject::connect(&threadDB, SIGNAL(playerNameFound(int, QString)), &profileDB, SLOT(setPlayerName(int, QString)));
    QObject::connect(&threadDB, SIGNAL(finished()), &a, SLOT(quit()));
    threadDB.start();

    UserInterface uiWindow(&profileDB, &crewDB, &threadDB);
    uiWindow.setWindowIcon(IconLoader::loadingAppIcon());
    uiWindow.setupDirEnv();
#ifdef Q_OS_ANDROID
    uiWindow.showMaximized();
#else
    uiWindow.show();
#endif

    return a.exec();
}
