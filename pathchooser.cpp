#include "pathchooser.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>

static QString appBundleExpandedPath(const QString &path)
{
#ifdef Q_OS_MAC
    if (path.endsWith(".app")) {
        // possibly expand to Foo.app/Contents/MacOS/Foo
        QFileInfo info(path);
        if (info.isDir()) {
            QString exePath = path + "/Contents/MacOS/" + info.completeBaseName();
            if (QFileInfo::exists(exePath))
                return exePath;
        }
    }
#endif
    return path;
}

PathChooser::PathChooser(QWidget *parent) :
    QWidget(parent),
    m_hLayout(new QHBoxLayout),
    m_lineEdit(new QLineEdit)
{
    m_hLayout->setContentsMargins(0, 0, 0, 0);
    m_lineEdit->setMinimumWidth(120);
    m_hLayout->addWidget(m_lineEdit);
    m_hLayout->setSizeConstraint(QLayout::SetMinimumSize);

    auto onTextChanged = [this](const QString &text) {
        setPath(QDir::fromNativeSeparators(text));
    };
    connect(m_lineEdit, &QLineEdit::textChanged, this, onTextChanged);

    addButton(browseButtonLabel(), this, [this] { slotBrowse(); });

    setLayout(m_hLayout);
    setFocusProxy(m_lineEdit);
    setFocusPolicy(m_lineEdit->focusPolicy());
}

void PathChooser::setExpectedKind(PathChooser::Kind expected)
{
    if (m_kind == expected)
        return;
    m_kind = expected;
    emit expectedKindChanged(expected);
}

void PathChooser::setPath(QString path)
{
    if (m_path == path)
        return;
    m_path = std::move(path);
    m_lineEdit->setText(QDir::toNativeSeparators(m_path));
    emit pathChanged(m_path);
}

void PathChooser::setPromptDialogFilter(QString filter)
{
    if (m_dialogFilter == filter)
        return;
    m_dialogFilter = std::move(filter);
    emit promptDialogFilterChanged(m_dialogFilter);
}

QString PathChooser::browseButtonLabel()
{
#ifdef Q_OS_MAC
    return tr("Choose...");
#else
    return tr("Browse...");
#endif
}

void PathChooser::addButton(
        const QString &text, QObject *context, const std::function<void ()> &callback)
{
    insertButton(m_buttons.size(), text, context, callback);
}

void PathChooser::insertButton(size_t index, const QString &text, QObject *context, const std::function<void ()> &callback)
{
    auto button = new QPushButton;
    button->setText(text);
    connect(button, &QAbstractButton::clicked, context, callback);
    m_hLayout->insertWidget(index + 1/*line edit*/, button);
    m_buttons.insert(m_buttons.begin() + index, button);
}

void PathChooser::slotBrowse()
{
    QString predefined = path();
    QFileInfo fi(predefined);

    if (!predefined.isEmpty() && !fi.isDir()) {
        predefined = fi.path();
        fi.setFile(predefined);
    }

//    if ((predefined.isEmpty() || !fi.isDir())
//            && !d->m_initialBrowsePathOverride.isNull()) {
//        predefined = d->m_initialBrowsePathOverride;
//        fi.setFile(predefined);
//        if (!fi.isDir()) {
//            predefined.clear();
//            fi.setFile(QString());
//        }
//    }

    // Prompt for a file/dir
    QString newPath;
    switch (m_kind) {
    case Kind::Directory:
    case Kind::ExistingDirectory:
        newPath = QFileDialog::getExistingDirectory(this,
                makeDialogTitle(tr("Choose Directory")), predefined);
        break;
    case Kind::ExistingCommand:
    case Kind::Command:
        newPath = QFileDialog::getOpenFileName(this,
                makeDialogTitle(tr("Choose Executable")), predefined,
                m_dialogFilter);
        newPath = appBundleExpandedPath(newPath);
        break;
    case Kind::File: // fall through
        newPath = QFileDialog::getOpenFileName(this,
                makeDialogTitle(tr("Choose File")), predefined,
                m_dialogFilter);
        newPath = appBundleExpandedPath(newPath);
        break;
    case Kind::SaveFile:
        newPath = QFileDialog::getSaveFileName(this,
                makeDialogTitle(tr("Choose File")), predefined,
                m_dialogFilter);
        break;
    case Kind::Any: {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setWindowTitle(makeDialogTitle(tr("Choose File")));
        if (fi.exists())
            dialog.setDirectory(fi.absolutePath());
        // FIXME: fix QFileDialog so that it filters properly: lib*.a
        dialog.setNameFilter(m_dialogFilter);
        if (dialog.exec() == QDialog::Accepted) {
            // probably loop here until the *.framework dir match
            QStringList paths = dialog.selectedFiles();
            if (!paths.isEmpty())
                newPath = paths.at(0);
        }
        break;
        }

    default:
        break;
    }

    if (!newPath.isEmpty())
        setPath(newPath);
}

QString PathChooser::makeDialogTitle(const QString &title)
{
//    if (d->m_dialogTitleOverride.isNull())
        return title;
//    else
//        return d->m_dialogTitleOverride;
}

