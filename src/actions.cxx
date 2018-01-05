#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>

#include "actions.hh"
#include "clipboard.hh"
#include "tabwidget.hh"

void Actions::openCurrentFile() {
    QString path = TabWidget::currentWidget()->fsCurrentPath();
    path+=TabWidget::currentWidget()->currentItemName();
    QDesktopServices::openUrl(QUrl(path));
}

void Actions::newFolder() {
    QInputDialog dialog;
    dialog.setWindowTitle("Create Folder");
    dialog.setLabelText("What do you want to name your folder?");
    dialog.setTextValue("untitled folder");
    if (dialog.exec()) {
        QString val = dialog.textValue();
        QString path = TabWidget::currentWidget()->fsCurrentPath();
        path+=val;
        if (QDir(path).exists()) {
            QMessageBox msg;
            msg.setWindowTitle("Error");
            msg.setIcon(QMessageBox::Critical);
            msg.setText("A file or folder with that name already exists.");
            msg.exec();
        } else {
            QDir().mkdir(path);
        }
    }
}

void Actions::newFile() {
    QInputDialog dialog;
    dialog.setWindowTitle("Create File");
    dialog.setLabelText("What do you want to name your file?");
    dialog.setTextValue("untitled file");
    if (dialog.exec()) {
        QString val = dialog.textValue();
        QString path = TabWidget::currentWidget()->fsCurrentPath();
        path+=val;
        if (QDir(path).exists()) {
            QMessageBox msg;
            msg.setWindowTitle("Error");
            msg.setIcon(QMessageBox::Critical);
            msg.setText("A file or folder with that name already exists.");
            msg.exec();
        } else {
            QFile file(path);
            if (file.open(QFile::WriteOnly)) {
                file.close();
            }
        }
    }
}

void Actions::rename() {
    QString currentPath = TabWidget::currentWidget()->fsCurrentPath();
    QString currentName = TabWidget::currentWidget()->currentItemName();

    QInputDialog dialog;
    dialog.setWindowTitle("Rename");
    dialog.setLabelText("Enter your new file/folder name:");
    dialog.setTextValue(currentName);
    if (dialog.exec()) {
        QString newName = dialog.textValue();
        if ((newName==currentName)||(QFile(currentPath+newName).exists())) {
            QMessageBox msg;
            msg.setWindowTitle("Error");
            msg.setIcon(QMessageBox::Critical);
            msg.setText("A file or folder already exists with that same name.");
            msg.exec();
        } else {
            QString newPath = currentPath+newName;
            QString oldPath = currentPath+currentName;
            bool ret = QFile(oldPath).rename(newPath);
            if (ret==false) {
                QMessageBox msg;
                msg.setWindowTitle("Error");
                msg.setIcon(QMessageBox::Critical);
                msg.setText("There was an error renaming this file or folder!");
                msg.setDetailedText("This could mean that the file is read-only, "
                                    "or that you do not have the proper permissions "
                                    "to access it.");
                msg.exec();
            }
        }
    }
}

void Actions::deleteFile() {
    QMessageBox msg;
    msg.setWindowTitle("Warning!");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("This will permanently delete this file or folder.\n"
                "If it is a folder, all its contents will be deleted also.\n"
                "This CANNOT be undone!\n\n"
                "Do you wish to continue?");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msg.exec();
    if (ret==QMessageBox::Yes) {
        QString toDelete = TabWidget::currentWidget()->fsCurrentPath();
        toDelete+=TabWidget::currentWidget()->currentItemName();
        bool ret;
        if (QFileInfo(toDelete).isDir()) {
            ret = QDir(toDelete).removeRecursively();
        } else {
            ret = QFile(toDelete).remove();
        }
        if (ret==false) {
            QMessageBox msg;
            msg.setWindowTitle("Error");
            msg.setIcon(QMessageBox::Critical);
            msg.setText("There was an error deleting this file or folder!");
            msg.setDetailedText("This could mean that the file is read-only, "
                                "or that you do not have the proper permissions "
                                "to access it.");
            msg.exec();
        }
    }
}

void Actions::deleteFolder() {
    //We'll just check to see if the object in question
    //is a file or folder in the function above, and then delete
    //accordingly.
    //This will use less code, and make things cleaner.
    //We'll keep this as a separate function in case we wish to change
    //anything later on.
    deleteFile();
}

void Actions::paste() {
    if (!clipboard.oldPath.endsWith("/")) {
        clipboard.oldPath+="/";
    }
    if (!clipboard.newPath.endsWith("/")) {
        clipboard.newPath+="/";
    }

    QString oldAddr = clipboard.oldPath;
    QString newAddr = clipboard.newPath;

    oldAddr += clipboard.fileName;
    newAddr += clipboard.fileName;

    if (clipboard.action==Clipboard_Actions::CUT) {
        bool ret = QFile(oldAddr).rename(newAddr);
        if (ret==false) {
            if (QFileInfo(newAddr).exists()) {
                newAddr = handleDuplicate(newAddr);
                ret = QFile(oldAddr).rename(newAddr);
                if (ret==false) {
                    QMessageBox msg;
                    msg.setWindowTitle("Fatal Error");
                    msg.setText("An unknown critical error has occurred.");
                    msg.setIcon(QMessageBox::Critical);
                    msg.exec();
                }
            } else {
                QMessageBox msg;
                msg.setWindowTitle("Error");
                msg.setIcon(QMessageBox::Critical);
                msg.setText("There was an error moving this file!");
                msg.setDetailedText("This could mean that the file is read-only, "
                                    "or that you do not have the proper permissions "
                                    "to access it.");
                msg.exec();
            }
        }
    } else {
        bool ret;
        if (clipboard.filetype==Clipboard_FileType::TYPE_FILE) {
            ret = QFile(oldAddr).copy(newAddr);
        } else {
            ret = copyDirectory(oldAddr,newAddr);
        }
        if (ret==false) {
            if (QFileInfo(newAddr).exists()) {
                newAddr = handleDuplicate(newAddr);
                if (clipboard.filetype==Clipboard_FileType::TYPE_FILE) {
                    ret = QFile(oldAddr).copy(newAddr);
                } else {
                    ret = copyDirectory(oldAddr,newAddr);
                }
                if (ret==false) {
                    QMessageBox msg;
                    msg.setWindowTitle("Fatal Error");
                    msg.setText("An unknown critical error has occurred.");
                    msg.setIcon(QMessageBox::Critical);
                    msg.exec();
                }
            } else {
                QMessageBox msg;
                msg.setWindowTitle("Error");
                msg.setIcon(QMessageBox::Critical);
                msg.setText("There was an error moving this file!");
                msg.setDetailedText("This could mean that the file is read-only, "
                                    "or that you do not have the proper permissions "
                                    "to access it.");
                msg.exec();
            }
        }
    }

    if (clipboard.action==Clipboard_Actions::CUT) {
        clipboard.action = Clipboard_Actions::NONE;
    }
}

QString Actions::handleDuplicate(QString newAddr) {
    QMessageBox msg;
    msg.setWindowTitle("Warning");
    msg.setText("A file with the same name already exists. "
                "Do you wish to overwrite it?");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setIcon(QMessageBox::Question);
    int ret = msg.exec();
    if (ret==QMessageBox::Yes) {
        if (QFileInfo(newAddr).isDir()) {
            QDir(newAddr).removeRecursively();
        } else {
            QFile(newAddr).remove();
        }
    } else {
        newAddr = clipboard.newPath+"(copy) "+clipboard.fileName;
    }
    return newAddr;
}

bool Actions::copyDirectory(QString oldPath, QString newPath) {
    if (!oldPath.endsWith("/")) {
        oldPath+="/";
    }
    if (!newPath.endsWith("/")) {
        newPath+="/";
    }
    if (QDir(newPath).exists()) {
        return false;
    }
    QDir().mkpath(newPath);
    QStringList contents = QDir(oldPath).entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (int i = 0; i<contents.size(); i++) {
        QString current = oldPath+contents.at(i);
        if (QFileInfo(current).isDir()) {
            copyDirectory(current,newPath+contents.at(i));
        } else {
            QFile(current).copy(newPath+contents.at(i));
        }
    }
    return true;
}
