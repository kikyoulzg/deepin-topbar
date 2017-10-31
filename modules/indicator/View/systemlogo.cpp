/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "systemlogo.h"
#include "../indicatormodel.h"
#include "dwidgetaction.h"

#include <QIcon>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QProcess>
#include <QAction>
#include <QDebug>

using namespace dtb;
using namespace dtb::indicator;
using namespace dtb::widgets;

SystemLogo::SystemLogo(QWidget *parent)
    : ContentModule(parent)
    , m_menu(new QMenu(this))
{
    QHBoxLayout *layout = new QHBoxLayout;

    layout->setMargin(0);
    layout->setSpacing(0);

    QLabel *logoLbl = new QLabel;
    QPixmap pixmap = QIcon::fromTheme("dde").pixmap(QSize(22, 22) * devicePixelRatioF());
    pixmap.setDevicePixelRatio(devicePixelRatioF());

    logoLbl->setPixmap(pixmap);

    layout->addWidget(logoLbl);

    setStyleSheet("QLabel {color: black;}");

    setLayout(layout);

    QAction *about = new QAction(tr("About"), this);
    QLabel *preference = new QLabel(tr("Preference setting"), this);
    m_appstore = new AppstoreAction;
    QAction *forceQuit = new QAction(tr("Force quit"), this);
    QAction *sleep = new QAction(tr("Sleep"), this);
    QAction *restart = new QAction(tr("Restart"), this);
    QAction *shutdown = new QAction(tr("Power off"), this);
    QAction *logout = new QAction(tr("Logout for %1").arg(QString(qgetenv("USER"))), this);

    DWidgetAction *app = new DWidgetAction(m_appstore);
    DWidgetAction *pre = new DWidgetAction(preference);

    m_menu->addAction(about);
    m_menu->addSeparator();
    m_menu->addAction(pre);
    m_menu->addAction(app);
    m_menu->addSeparator();
    m_menu->addAction(forceQuit);
    m_menu->addSeparator();
    m_menu->addAction(sleep);
    m_menu->addAction(restart);
    m_menu->addAction(shutdown);
    m_menu->addSeparator();
    m_menu->addAction(logout);

    QSignalMapper *signalMapper = new QSignalMapper(this);

    connect(about, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(pre, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(app, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(forceQuit, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(sleep, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(restart, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(shutdown, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(logout, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));

    signalMapper->setMapping(about, About);
    signalMapper->setMapping(pre, Preference);
    signalMapper->setMapping(app, Appstore);
    signalMapper->setMapping(forceQuit, ForceQuit);
    signalMapper->setMapping(sleep, "Suspend");
    signalMapper->setMapping(restart, "Restart");
    signalMapper->setMapping(shutdown, "Shutdown");
    signalMapper->setMapping(logout, "Logout");

    connect(signalMapper, static_cast<void (QSignalMapper::*)(const int)>(&QSignalMapper::mapped), this, &SystemLogo::handleAction);
    connect(signalMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this, &SystemLogo::handleShutdownAction);
}

QMenu *SystemLogo::menu() const
{
    return m_menu;
}

void SystemLogo::setModel(IndicatorModel *model)
{
    m_model = model;

    m_appstore->setModel(model);
}

void SystemLogo::handleAction(const int &action)
{
    switch (action) {
    case About:
        QProcess::startDetached("dbus-send --print-reply --dest=com.deepin.dde.ControlCenter /com/deepin/dde/ControlCenter com.deepin.dde.ControlCenter.ShowModule \"string:systeminfo\"");
        break;
    case Preference:
        break;
    case Appstore:
        QProcess::startDetached("/usr/bin/deepin-appstore");
        break;
    case ForceQuit:
        emit requestForceQuit();
        break;
    default:
        break;
    }
}

void SystemLogo::handleShutdownAction(const QString &action)
{
    const QString command = QString("dbus-send --print-reply --dest=com.deepin.dde.shutdownFront " \
                                    "/com/deepin/dde/shutdownFront " \
                                    "com.deepin.dde.shutdownFront.%1").arg(action);

    QProcess::startDetached(command);
}
