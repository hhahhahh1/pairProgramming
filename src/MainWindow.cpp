/*************************************************************************    
 *
 *   Ned - Simple, graphical, cross-platform editor
 *   Copyright (C) 2010 Ákos Kovács <akoskovacs@gmx.com> 
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

#include "MainWindow.h"

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QDockWidget>
#include <QDrag>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QFontDialog>
#include <QFontInfo>
#include <QLabel>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QUrl>

MainWindow::MainWindow(QStringList files, QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new QPlainTextEdit;
    textEdit->setAcceptDrops(false);
    setCentralWidget(textEdit);

    // We enable drop on the whole window
    setAcceptDrops(true);

    m_savedFileName = "";
    m_isFileNameKnown = false;

    connect(textEdit, SIGNAL(textChanged())
            ,this, SLOT(textEditModified()));
    connect(textEdit, SIGNAL(textChanged())
            ,this, SLOT(updateStatusBar()));

    setWindowTitle(tr("%1[*] - Ned").arg(tr("Untitled")));
    createActions();
    createMenus();
    createToolBars();
    createContextMenus();

    readSettings();

    createStatusBar();
    setWindowIcon(QIcon(":/images/text-editor.png"));

    if (files.count() == 1) {
        m_savedFileName = files.first();
        m_isFileNameKnown = true;
        loadFile(files.first());
    } else {
        createWindows(files);
    }
}

void MainWindow::createWindows(QStringList &files)
{
    MainWindow *w;
    QStringList tmp;
    QStringListIterator it(files);
    while (it.hasNext()) {
       tmp << it.next();
       w = new MainWindow(tmp);
       w->show();
       tmp.clear();
    }
}

/*
 * createActions: Creates the main actions associated with some properties
 * for menus, toolbars and context menus.
 */
void MainWindow::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon::fromTheme("document-new"
                    ,QIcon(":/images/document-new.png")));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create new document"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon::fromTheme("document-open"
                    ,QIcon(":/images/document-open.png")));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an exsisiting document"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon::fromTheme("document-save"
                   ,QIcon(":/images/document-save.png")));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the edited document"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

    saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setIcon(QIcon::fromTheme("document-save-as"
                   ,QIcon(":/images/document-save-as.png")));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save as a document"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setEnabled(false);
    undoAction->setIcon(QIcon::fromTheme("edit-undo"
                   ,QIcon(":/images/edit-undo.png")));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo a change in the editor area"));
    connect(undoAction, SIGNAL(triggered()), textEdit, SLOT(undo()));
    connect(textEdit, SIGNAL(undoAvailable(bool)), undoAction,  SLOT(setEnabled(bool)));

    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setEnabled(false);
    redoAction->setIcon(QIcon::fromTheme("edit-redo"
                   ,QIcon(":/images/edit-redo.png")));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setStatusTip(tr("Redo a change in the editor area"));
    connect(textEdit, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
    connect(redoAction, SIGNAL(triggered()), textEdit, SLOT(redo()));

    findAction = new QAction(tr("&Find..."), this);
    findAction->setIcon(QIcon::fromTheme("edit-find"));
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip(tr("Find a text"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(createFindDialog()));

    clearAllAction = new QAction(tr("Cl&ear All"), this);
    clearAllAction->setIcon(QIcon::fromTheme("edit-clear"
                   ,QIcon(":/images/edit-delete.png")));
    clearAllAction->setShortcut(QKeySequence::Delete);
    clearAllAction->setStatusTip(tr("Delete a part of a text"));
    connect(clearAllAction, SIGNAL(triggered()), textEdit, SLOT(clear()));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setEnabled(false);
    copyAction->setIcon(QIcon::fromTheme("edit-copy"
                   ,QIcon(":/images/edit-copy.png")));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy a text part"));
    connect(textEdit, SIGNAL(copyAvailable(bool)),copyAction, SLOT(setEnabled(bool)));
    connect(copyAction, SIGNAL(triggered()), textEdit, SLOT(copy()));

    cutAction = new QAction(tr("C&ut"), this);
    cutAction->setEnabled(false);
    cutAction->setIcon(QIcon::fromTheme("edit-cut"
                   ,QIcon(":/images/edit-cut.png")));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut a text part"));
    connect(textEdit, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));
    connect(cutAction, SIGNAL(triggered()), textEdit, SLOT(cut()));

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setIcon(QIcon::fromTheme("edit-paste"
                   ,QIcon(":/images/edit-paste.png")));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Paste the cutted text"));
    connect(pasteAction, SIGNAL(triggered()), textEdit, SLOT(paste()));

    selectAllAction = new QAction(tr("&Select All"), this);
    selectAllAction->setIcon(QIcon::fromTheme("edit-select-all"
                   ,QIcon(":/images/edit-select-all.png")));
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setStatusTip(tr("Select all text in the text editor"));
    connect(selectAllAction, SIGNAL(triggered()), textEdit, SLOT(selectAll()));

    aboutAction = new QAction(tr("About this"), this);
    aboutAction->setIcon(QIcon::fromTheme("help-about"));
    aboutAction->setStatusTip(tr("About this application ..."));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    exitAction = new QAction(tr("Exit"), this);
    exitAction->setIcon(QIcon::fromTheme("application-exit"
                   ,QIcon(":/images/window-close.png")));
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit from the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    pasteDateTimeAction = new QAction(tr("Paste date and time"), this);
    pasteDateTimeAction->setIcon(QIcon::fromTheme("edit-paste"
                   ,QIcon(":/images/edit-paste.png")));
    pasteDateTimeAction->setShortcut(tr("Ctrl+D"));
    pasteDateTimeAction->setStatusTip(tr("Paste the current date and time"));
    connect(pasteDateTimeAction, SIGNAL(triggered()), this, SLOT(pasteDateTime()));

    fontDialogAction = new QAction(tr("Select font ..."), this);
    fontDialogAction->setStatusTip(tr("Select the font for the text area"));
    connect(fontDialogAction, SIGNAL(triggered()), this, SLOT(selectFont()));

    defaultsAction = new QAction(tr("Set to defaults"), this);
    defaultsAction->setStatusTip(tr("Set everything to it's default setting, like font"));
    connect(defaultsAction, SIGNAL(triggered()), this, SLOT(setDefaults()));

    quickDialogAction = new QAction(tr("Show Quick File Browser ..."), this);
    quickDialogAction->setIcon(QIcon::fromTheme(("folder")
                    , QIcon(":/images/folder-documents.png")));
    quickDialogAction->setShortcut(tr("Ctrl+B"));
    connect(quickDialogAction, SIGNAL(triggered())
            , this, SLOT(showQuickDialog()));

    aboutQtAction = new QAction(tr("About Qt ..."), this);
    connect(aboutQtAction, SIGNAL(triggered())
            , this, SLOT(aboutQt()));
}

/* createContextMenus: Create some editing menus for the central
 * widget. In this case this is a QPlainTextEdit.
 * It will be appeare when the user clicks to it with the left
 * mouse button.
*/
void MainWindow::createContextMenus()
{
    textEdit->addAction(copyAction);
    textEdit->addAction(cutAction);
    textEdit->addAction(pasteAction);
    textEdit->addAction(selectAllAction);
    textEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*
 * createToolBars: Set up the main tool bar.
*/
void MainWindow::createToolBars()
{
    baseToolBar = addToolBar(tr("&Base"));
    baseToolBar->addAction(newAction);
    baseToolBar->addAction(openAction);
    baseToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(undoAction);
    editToolBar->addSeparator();
    editToolBar->addAction(clearAllAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(cutAction);
    editToolBar->addAction(cutAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();

    utilToolBar = addToolBar(tr("&Utils"));
    utilToolBar->addAction(quickDialogAction);
}

/*
 * createMenus: Set up the main menus.
*/
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(quickDialogAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addSeparator();
    editMenu->addAction(clearAllAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAction);

    pasteMenu = menuBar()->addMenu(tr("&Paste"));
    pasteMenu->addAction(pasteDateTimeAction);

    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(fontDialogAction);
    settingsMenu->addAction(defaultsAction);

    menuBar()->addSeparator();
    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
    aboutMenu->addSeparator();
    aboutMenu->addAction(aboutQtAction);
}

/*
 * createStatusBar: Create a simple status bar.
*/
void MainWindow::createStatusBar()
{
    characters = new QLabel("Lines");
    lines = new QLabel("bla");
    mode = new QLabel;
    fontLabel = new QLabel();

    statusBar()->addWidget(characters);
    statusBar()->addWidget(lines);
    statusBar()->addWidget(mode);
    statusBar()->addWidget(fontLabel);

    characters->setAlignment(Qt::AlignLeft);
    lines->setAlignment(Qt::AlignHCenter);
    mode->setAlignment(Qt::AlignRight);

    characters->show();
    lines->show();
    mode->show();
    fontLabel->show();

    updateStatusBar();
}

/*
 * updateStatusBar: This slot invoked every tine when
 * the document modified, so it write the total character,
 * line count and the access mode to the status bar.
*/
void MainWindow::updateStatusBar()
{
    QTextDocument *textDocument = textEdit->document();
    QFontInfo fontInfo(currentFont);

    characters->setText(tr("Total characters: %1")
                        .arg(textDocument->characterCount()-1));
    lines->setText(tr("Total lines: %1")
                   .arg(textDocument->lineCount()));

    if (!m_savedFileName.isEmpty()) {
        if (!QFileInfo(m_savedFileName).isWritable())
                mode->setText(tr("Access: Read only"));
    } else  {
                 mode->setText(tr("Access: RW"));
    }
    fontLabel->setText(tr("Font: %1 %2")
                       .arg(fontInfo.family())
                       .arg(fontInfo.pointSize()));
}

/*
 * readSettings: It reads the settings on every startup.
 * Now the setting only contain the main window geometry.
 */
void MainWindow::readSettings()
{
    int pointSize = 12;
    QString family;

    QSettings settings("Akos Kovacs", "Ned");
    settings.beginGroup("Window");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    settings.beginGroup("Font");
    family = settings.value("family").toString();
    pointSize = settings.value("size").toInt();
    currentFont.setFamily(family);
    currentFont.setPointSize(pointSize);
    textEdit->setFont(currentFont);
    settings.endGroup();

    settings.beginGroup("Windows");
    if (settings.value("quick_dialog_autostart").toBool())
        showQuickDialog();

    settings.endGroup();
}

/*
 * writeSettings: It writes the settings before the user
 * close the application. Only writes the geometry.
*/
void MainWindow::writeSettings()
{
    QFontInfo fontInfo(currentFont);
    QSettings settings("Akos Kovacs", "Ned");
    settings.beginGroup("Window");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("Font");
    settings.setValue("family", fontInfo.family());
    settings.setValue("size", fontInfo.pointSize());
    settings.endGroup();

    settings.beginGroup("Windows");
    bool isDialogShown = true;
    if (quickDialogDock == reinterpret_cast<QDockWidget *>(0))
        isDialogShown = false;

    settings.setValue("quick_dialog_autostart", isDialogShown);
    settings.endGroup();
}

/* closeEvent: This function invoked every time when
 * the user wants to quit the application. If the 
 * document has unwritten modifications, a popup
 * window will appear and ask the user of the
 * modifications. If the answer is Cancel the event
 * will ignored.
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

/*
 * dragEnterEvent: It accepts the drag events.
*/
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

/*
 * dropEvent: It handles the drop events, by open the
 * file which has dropped into the application's drag area.
*/
void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    QString fileName = urls.first().toLocalFile();

    if (fileName.isEmpty())
        return;

    if (readFile(fileName))
        setCurrentFile(fileName);
}

/* setCurrentFile: It set the current titlebar, statusbar when
 * a new file created, opened, saved, etc...
*/
void MainWindow::setCurrentFile(const QString &fileName)
{
    QString currentFile;

    if (fileName.isEmpty())
        currentFile = tr("Untitled");
    else
        currentFile = QFileInfo(fileName).fileName();

   setWindowModified(false);

   setWindowTitle(tr("%1[*] - Ned").arg(currentFile));
   updateStatusBar();
}

/*
 * maybeSave: Actived, every time when the user wants to quit
 * somehow, and popup a question if the document has unwritten changes.
*/
bool MainWindow::maybeSave()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Ned")
                                 ,tr("The document has been modified.\n"
                                      "Do you want to save the changes?")
                                 ,QMessageBox::Yes | QMessageBox::No
                                 | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return saveFile();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this
                                        , tr("Open text files"), QDir::homePath()
                                        , tr("Every file (*.*);;Text files (*.txt)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
        m_savedFileName = fileName;
        m_isFileNameKnown = true;
    }
}

bool MainWindow::loadFile(QString &fileName)
{
    if (!readFile(fileName))
    {
        statusBar()->showMessage(tr("File loading cancelled"), 2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    updateStatusBar();
    return true;
}

bool MainWindow::saveFile()
{
    QString fileName;

    if (!m_isFileNameKnown) {
        fileName = QFileDialog::getSaveFileName(this
                                            , tr("Save text files"), QDir::homePath() 
                                            , tr("Every file (*);;Text files (*.txt)"));
        m_savedFileName = fileName;
    }
    if (!m_savedFileName.isEmpty()) {
        if (QFileInfo(m_savedFileName).exists()
                && !QFileInfo(m_savedFileName).isWritable()) {
            QMessageBox::warning(this, tr("Ned - Error"), tr("The file is not writable"));
            return false;
        } else
            writeFile(m_savedFileName);
    } else
        return false;

    m_isFileNameKnown = true;
    setCurrentFile(m_savedFileName);
    return true;
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this
                                , tr("Save as text file"), QDir::homePath() 
                                , tr("Every file (*);;Text files (*.txt)"));

    if (!fileName.isEmpty())
        writeFile(fileName);
    setCurrentFile(fileName);
}

void MainWindow::newFile()
{
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
}

void MainWindow::createFindDialog()
{
  if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(find(QString, QTextDocument::FindFlags))
                ,this ,SLOT(findText(QString, QTextDocument::FindFlags)));
   }

    //findDialog->exec();
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::about()
{
    QMessageBox::about(this
               ,tr("About Ned")
               ,tr("<b><h3>Ned, version %1</h3></b>"
                   "Ned is a simple, cross-platform text editor "
                   "written in C++, using the Qt4 graphical "
                   "framework.<br>"
                   "<i>Copyright (C) Akos Kovacs - 2010</i>")
                   .arg(APP_VERSION));
}

bool MainWindow::readFile(QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this
                  ,tr("Critical Error - Ned")
                  ,tr("An error occured when trying to read "
                      "%1 file from the disk").arg(fileName));
        return false;
    }
    QTextStream in(&file);
    textEdit->setPlainText(in.readAll());
    return true;
}

bool MainWindow::writeFile(QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this
                  ,tr("Critical Error - Ned")
                  ,tr("An error occured when trying to write"
                      "%1 file to the disk").arg(fileName));
        return false;
    }
    QTextStream out(&file);
    out << textEdit->toPlainText();
    return true;
}

void MainWindow::textEditModified()
{
    setWindowModified(true);
}

void MainWindow::findText(QString str, QTextDocument::FindFlags ff)
{
    textEdit->find(str, ff);
}

void MainWindow::pasteDateTime()
{
    textEdit->textCursor()
            .insertText(QDateTime::currentDateTime().toString());
}

void MainWindow::selectFont()
{
    bool isOk;
    QFont font = QFontDialog::getFont(&isOk, currentFont, this);
    if (isOk) {
        textEdit->setFont(font);
        currentFont = font;
    }
    updateStatusBar();
}

void MainWindow::setDefaults()
{
    QFont dFont = QApplication::font();
    textEdit->setFont(dFont);
    currentFont = dFont;
    updateStatusBar();
}

void MainWindow::showQuickDialog()
{
    if (quickDialogDock != reinterpret_cast<QDockWidget *>(0)) {
            statusBar()->showMessage(tr("The dialog is already open!"), 2000);
            return;
    }

    quickDialogDock = new QDockWidget(tr("Quick File Chooser"), this);
    quickDialogDock->setAttribute(Qt::WA_DeleteOnClose);
    quickDialogDock->setAllowedAreas(Qt::LeftDockWidgetArea
                                    | Qt::RightDockWidgetArea);

    quickDialog = new QuickDialog(this
                  , (m_isFileNameKnown) ? QFileInfo(m_savedFileName).path() : "");
    quickDialogDock->setWidget(quickDialog);
    addDockWidget(Qt::LeftDockWidgetArea, quickDialogDock);
    connect(quickDialog, SIGNAL(fileSelected(QString, bool))
                                ,this, SLOT(onFileSelected(QString,bool)));
}

void MainWindow::onFileSelected(QString path, bool inNewWindow)
{
    if (inNewWindow) {
        QStringList file;
        file << path;
        MainWindow *w = new MainWindow(file);
        w->show();
        return;
    }

    if (maybeSave()) {
        m_isFileNameKnown = true;
        m_savedFileName = path;
        loadFile(path);
    }
}
