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

#ifndef QUICKDIALOG_H
#define QUICKDIALOG_H

#include <QCheckBox>
#include <QModelIndex>
#include <QString>
#include <QUrl>
#include <QWidget>

class QFileSystemModel;
class QHBoxLayout;
class QLineEdit;
class QListView;
class QToolButton;
class QVBoxLayout;
class QCompleter;

class QuickDialog : public QWidget
{
    Q_OBJECT

    public:
        QuickDialog(QWidget *parent = 0, const QString &currentPath = "");
        bool isOpenNewWindow() { return m_openWindow->isChecked(); }
        void setOpenNewWindow(bool isChecked)
        { m_openWindow->setChecked(isChecked); }

    signals:
        void fileSelected(QString, bool);

    private slots:
        void goUpperDirectory();
        void goToDirectory();
        void refreshPath(QString);
        void openItem(QModelIndex);

    private:
        void setupWidgets();
        void setupLayouts();
        void setupConnections();

        QLineEdit *pathEdit;
        QToolButton *upButton;
        QListView *dirView;
        QVBoxLayout *mainLayout;
        QHBoxLayout *topLayout;
        QFileSystemModel *m_fsModel;
        QString m_path;
        QCheckBox *m_openWindow;
        QCompleter *m_completer;
};

#endif // QUICKDIALOG_H
