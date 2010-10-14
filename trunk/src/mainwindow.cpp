/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "mdichild.h"
#include "burnconfigdialog.h"
#include "avroutputdialog.h"

MainWindow::MainWindow()
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("eePe - EEPROM Editor"));
    setUnifiedTitleAndToolBarOnMac(true);
    this->setWindowIcon(QIcon(":/icon.ico"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();

    if(!child->parentWidget()->isMaximized() && !child->parentWidget()->isMinimized()) child->parentWidget()->resize(400,500);
}

void MainWindow::open()
{
    QSettings settings("er9x-eePe", "eePe");
    QString fileName = QFileDialog::getOpenFileName(this,"Open",settings.value("lastDir").toString(),tr("EEPROM files (*.bin *.hex);;BIN files (*.bin);;HEX files (*.hex)"));
    if (!fileName.isEmpty()) {
        settings.setValue("lastDir",QFileInfo(fileName).dir().absolutePath());

        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
            if(!child->parentWidget()->isMaximized() && !child->parentWidget()->isMinimized()) child->parentWidget()->resize(400,500);
        } else {
            child->close();
        }
    }
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::cut()
{
    if (activeMdiChild())
    {
        activeMdiChild()->cut();
        updateMenus();
    }
}

void MainWindow::copy()
{
    if (activeMdiChild())
    {
        activeMdiChild()->copy();
        updateMenus();
    }
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}

void MainWindow::burnTo()
{
    if (activeMdiChild())
        activeMdiChild()->burnTo();
}

void MainWindow::simulate()
{
    if (activeMdiChild())
        activeMdiChild()->simulate();
}

void MainWindow::burnFrom()
{
    burnConfigDialog bcd;
    QString avrdudeLoc = bcd.getAVRDUDE();
    QString tempDir    = bcd.getTempDir();
    QString programmer = bcd.getProgrammer();
    QStringList args   = bcd.getAVRArgs();

    QString tempFile = tempDir + "/temp.hex";
    QString str = "eeprom:r:" + tempFile + ":i"; // writing eeprom -> MEM:OPR:FILE:FTYPE"

    QStringList arguments;
    arguments << "-c" << programmer << "-p" << "m64" << args << "-U" << str;

    avrOutputDialog ad(this, avrdudeLoc, arguments,"Read EEPROM From Tx"); //, AVR_DIALOG_KEEP_OPEN);
    int res = ad.exec();

    if(QFileInfo(tempFile).exists() && res)
    {
        MdiChild *child = createMdiChild();
        child->newFile();
        child->loadFile(tempFile,false);
        child->setModified();
        child->show();
        if(!child->parentWidget()->isMaximized() && !child->parentWidget()->isMinimized()) child->parentWidget()->resize(400,500);
    }
}

void MainWindow::burnToFlash()
{
    QSettings settings("er9x-eePe", "eePe");
    QString fileName = QFileDialog::getOpenFileName(this,"Open",settings.value("lastDir").toString(),tr("FLASH files (*.bin *.hex);;BIN files (*.bin);;HEX files (*.hex)"));
    if (!fileName.isEmpty())
    {
        settings.setValue("lastDir",QFileInfo(fileName).dir().absolutePath());

        burnConfigDialog bcd;
        QString avrdudeLoc = bcd.getAVRDUDE();
        QString programmer = bcd.getProgrammer();
        QStringList args   = bcd.getAVRArgs();

        QString str = "flash:w:" + fileName; // writing eeprom -> MEM:OPR:FILE:FTYPE"
        if(QFileInfo(fileName).suffix().toUpper()=="HEX") str += ":i";
        else if(QFileInfo(fileName).suffix().toUpper()=="BIN") str += ":r";
        else str += ":a";

        QStringList arguments;
        arguments << "-c" << programmer << "-p" << "m64" << args << "-U" << str;

        avrOutputDialog ad(this, avrdudeLoc, arguments, "Write Flash To Tx");
        ad.exec();
    }
}

void MainWindow::burnFromFlash()
{
    QSettings settings("er9x-eePe", "eePe");
    QString fileName = QFileDialog::getSaveFileName(this,"Open",settings.value("lastDir").toString(),tr("HEX files (*.hex);;BIN files (*.bin);;FLASH files (*.bin *.hex)"));
    if (!fileName.isEmpty())
    {
        settings.setValue("lastDir",QFileInfo(fileName).dir().absolutePath());

        burnConfigDialog bcd;
        QString avrdudeLoc = bcd.getAVRDUDE();
        QString programmer = bcd.getProgrammer();
        QStringList args   = bcd.getAVRArgs();

        QString str = "flash:r:" + fileName; // writing eeprom -> MEM:OPR:FILE:FTYPE"
        if(QFileInfo(fileName).suffix().toUpper()=="HEX") str += ":i";
        else if(QFileInfo(fileName).suffix().toUpper()=="BIN") str += ":r";
        else str += ":a";

        QStringList arguments;
        arguments << "-c" << programmer << "-p" << "m64" << args << "-U" << str;

        avrOutputDialog ad(this, avrdudeLoc, arguments, "Read Flash From Tx");
        ad.exec();
    }

}

void MainWindow::burnConfig()
{
    burnConfigDialog bcd;
    bcd.exec();
}

void MainWindow::burnList()
{
    burnConfigDialog bcd;
    bcd.listProgrammers();
}

void MainWindow::about()
{
    QString str = VER;
    str.remove("M");
    str.remove("S");
    if(str.contains(":"))
    {
        QStringList sl = str.split(":");
        str = sl.takeAt(1);
    }
    QMessageBox::about(this, tr("About eePe"),
                      tr("<center><img src=\":/images/eepe-title.png\"><br>"
                      "Copyright Erez Raviv &copy;2010<br>"
                      "<a href='http://code.google.com/p/eepe/'>http://code.google.com/p/eepe/</a><br>Revision: %1, %2</center>").arg(str.toInt()+1).arg(__DATE__));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild ? activeMdiChild()->hasPasteData() : false);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    burnToAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),simulateAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

//! [0]
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
//! [0]

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));


    burnToAct = new QAction(QIcon(":/images/write_eeprom.png"), tr("&Write EEPROM To Tx"), this);
    burnToAct->setShortcut(tr("Ctrl+Alt+W"));
    burnToAct->setStatusTip("Write EEPROM to transmitter");
    connect(burnToAct,SIGNAL(triggered()),this,SLOT(burnTo()));

    burnFromAct = new QAction(QIcon(":/images/read_eeprom.png"), tr("&Read EEPROM From Tx"), this);
    burnFromAct->setShortcut(tr("Ctrl+Alt+R"));
    burnFromAct->setStatusTip("Read EEPROM from transmitter");
    connect(burnFromAct,SIGNAL(triggered()),this,SLOT(burnFrom()));

    burnToFlashAct = new QAction(QIcon(":/images/write_flash.png"), tr("Write Flash memory"), this);
    burnToFlashAct->setStatusTip("Write flash memory to transmitter");
    connect(burnToFlashAct,SIGNAL(triggered()),this,SLOT(burnToFlash()));

    burnFromFlashAct = new QAction(QIcon(":/images/read_flash.png"), tr("Read Flash memory"), this);
    burnFromFlashAct->setStatusTip("Read flash memory to transmitter");
    connect(burnFromFlashAct,SIGNAL(triggered()),this,SLOT(burnFromFlash()));

    burnConfigAct = new QAction(QIcon(":/images/configure.png"), tr("&Configure..."), this);
    burnConfigAct->setStatusTip("Configure burning software");
    connect(burnConfigAct,SIGNAL(triggered()),this,SLOT(burnConfig()));

    burnListAct = new QAction(tr("&List programmers"), this);
    burnListAct->setStatusTip("List available programmers");
    connect(burnListAct,SIGNAL(triggered()),this,SLOT(burnList()));

    simulateAct = new QAction(QIcon(":/images/simulate.png"), tr("&Simulate"), this);
    simulateAct->setShortcut(tr("Alt+S"));
    simulateAct->setStatusTip("Simulate selected model.");
    simulateAct->setEnabled(false);
    connect(simulateAct,SIGNAL(triggered()),this,SLOT(simulate()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    //aboutQtAct = new QAction(tr("About &Qt"), this);
    //aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    //connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(simulateAct);
    fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    burnMenu = menuBar()->addMenu("&Burn");
    burnMenu->addAction(burnToAct);
    burnMenu->addAction(burnFromAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnToFlashAct);
    burnMenu->addAction(burnFromFlashAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnConfigAct);
    burnMenu->addAction(burnListAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    //helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(simulateAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    burnToolBar = addToolBar(tr("Burn"));
    burnToolBar->addAction(burnToAct);
    burnToolBar->addAction(burnFromAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnToFlashAct);
    burnToolBar->addAction(burnFromFlashAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnConfigAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("er9x-eePe", "eePe");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("er9x-eePe", "eePe");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
