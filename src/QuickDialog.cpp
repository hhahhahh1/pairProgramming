/*************************************************************************
 *
 *   Ned - Simple, graphical, cross-platform editor
 *   Copyright (C) 2011 Ákos Kovács <akoskovacs@gmx.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/

#include "QuickDialog.h"

#include <QCompleter>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QListView>
#include <QModelIndex>
#include <QToolButton>
#include <QVBoxLayout>

QuickDialog::QuickDialog(QWidget *parent, const QString &currentPath)
        : QWidget(parent), m_path(currentPath)
{
    setupWidgets();
    setupLayouts();
    setupConnections();
}

void QuickDialog::setupWidgets()
{
    pathEdit = new QLineEdit(this);
    m_fsModel = new QFileSystemModel(this);
    if (m_path.isEmpty())
        m_path = QDir::homePath();

    m_completer = new QCompleter;

    upButton = new QToolButton(this);
    upButton->setIcon(QIcon::fromTheme("go-up", QIcon(":/images/go-up.png")));

    dirView = new QListView(this);

    m_openWindow = new QCheckBox(tr("&Open in new window"), this);

    dirView->setModel(m_fsModel);
    QModelIndex index = m_fsModel->setRootPath(m_path);
    dirView->setRootIndex(index);
    m_completer->setModel(m_fsModel);
    pathEdit->setText(m_path);
    pathEdit->setCompleter(m_completer);
}

void QuickDialog::setupLayouts()
{
    topLayout = new QHBoxLayout;
    mainLayout = new QVBoxLayout;

    topLayout->addWidget(upButton);
    topLayout->addWidget(pathEdit);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(dirView);
    mainLayout->addWidget(m_openWindow);
    setLayout(mainLayout);
}

void QuickDialog::goUpperDirectory()
{
    QDir dir = m_path;
    if (!dir.cdUp())
        return;

    refreshPath(dir.path());
}

void QuickDialog::goToDirectory()
{
    refreshPath(m_path);
}

void QuickDialog::setupConnections()
{
   connect(upButton, SIGNAL(clicked())
           , this, SLOT(goUpperDirectory()));

   connect(dirView, SIGNAL(clicked(QModelIndex))
           , this, SLOT(openItem(QModelIndex)));

   connect(pathEdit, SIGNAL(textChanged(QString))
           , this, SLOT(refreshPath(QString)));

}

void QuickDialog::refreshPath(QString path)
{
    if (!path.isEmpty()) {
        dirView->setRootIndex(m_fsModel->setRootPath(path));
        m_path = path;
        pathEdit->setText(path);
    }
}

void QuickDialog::openItem(QModelIndex index)
{
    QString itemName = m_fsModel->filePath(index);
    if (QFileInfo(itemName).isDir())
       refreshPath(itemName);
    else
       emit fileSelected(itemName, m_openWindow->isChecked());
}
