/*****************************************************************************
* gta5sync GRAND THEFT AUTO V SYNC
* Copyright (C) 2016 Syping Gaming Team
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

#include "PictureDialog.h"
#include "ui_PictureDialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QDebug>

PictureDialog::PictureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PictureDialog)
{
    ui->setupUi(this);
    jsonDrawString = ui->labJSON->text();
}

PictureDialog::~PictureDialog()
{
    delete ui;
}

void PictureDialog::setSnapmaticPicture(QPixmap pixmap)
{
    ui->labPicture->setPixmap(pixmap);
}

void PictureDialog::setJsonString(QString jsonStr)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonStr.toLatin1());
    QJsonObject jsonObject = jsonDocument.object();
    QVariantMap jsonMap = jsonObject.toVariantMap();

    QString locX;
    QString locY;
    QString locZ;
    QStringList plyrsList;

    if (jsonMap.contains("loc"))
    {
        QJsonObject locObject = jsonObject["loc"].toObject();
        QVariantMap locMap = locObject.toVariantMap();
        if (locMap.contains("x")) { locX = locMap["x"].toString(); }
        if (locMap.contains("y")) { locY = locMap["y"].toString(); }
        if (locMap.contains("z")) { locZ = locMap["z"].toString(); }
    }
    if (jsonMap.contains("plyrs"))
    {
        plyrsList = jsonMap["plyrs"].toStringList();
    }

    QString plyrsStr;
    foreach (const QString &player, plyrsList)
    {
        plyrsStr.append(", ");
        plyrsStr.append(player);
    }
    if (plyrsStr.length() >= 1) { plyrsStr.remove(0,2); }

    ui->labJSON->setText(jsonDrawString.arg(locX, locY, locZ, plyrsStr));
}
