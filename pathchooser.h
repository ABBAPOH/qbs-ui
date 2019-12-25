#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QLineEdit;
class QAbstractButton;
QT_END_NAMESPACE

class PathChooser : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged DESIGNABLE true)
    Q_PROPERTY(Kind expectedKind READ expectedKind WRITE setExpectedKind NOTIFY expectedKindChanged DESIGNABLE true)
public:
    enum class Kind {
        ExistingDirectory,
        Directory, // A directory, doesn't need to exist
        File,
        SaveFile,
        ExistingCommand, // A command that must exist at the time of selection
        Command, // A command that may or may not exist at the time of selection (e.g. result of a build)
        Any
    };
    Q_ENUM(Kind)

    explicit PathChooser(QWidget *parent = nullptr);

    // Default is <Directory>
    Kind expectedKind() const noexcept { return m_kind; }
    Q_SLOT void setExpectedKind(Kind expected);
    Q_SIGNAL void expectedKindChanged(Kind);

    const QString &path() const noexcept { return m_path; }
    Q_SLOT void setPath(QString path);
    Q_SIGNAL void pathChanged(const QString &);

    const QString &promptDialogFilter() const noexcept { return m_dialogFilter; }
    Q_SLOT void setPromptDialogFilter(QString filter);
    Q_SIGNAL void promptDialogFilterChanged(const QString &);

    QLineEdit *lineEdit() const noexcept { return m_lineEdit; }

private:
    void addButton(const QString &text, QObject *context, const std::function<void ()> &callback);
    void insertButton(
            size_t index,
            const QString &text,
            QObject *context,
            const std::function<void ()> &callback);
    void slotBrowse();
    QString browseButtonLabel();
    QString makeDialogTitle(const QString &title);

private:
    Kind m_kind{Kind::Directory};
    QString m_path;
    QString m_dialogFilter;

    QHBoxLayout *m_hLayout{nullptr};
    QLineEdit *m_lineEdit{nullptr};
    std::vector<QAbstractButton *> m_buttons;
};

#endif // PATHCHOOSER_H
