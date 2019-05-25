#include "activityeditor.h"
#include <QPainter>
#include <QStyleOption>
#include <QLabel>
#include <QAction>

ActivityEditor::ActivityEditor(QWidget *parent) :  QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
    layout()->setSpacing(0);
    layout()->setMargin(0);

    setStyleSheet("ActivityEditor {"
                  "background-color: white;"
                  "}");

    navBar = new Navbar();

    layout()->addWidget(navBar);

    navBar->setTitle("New Activity");
    navBar->setLeftButton("‹ Back", this, &ActivityEditor::getBack);
    navBar->setRightButton("Done", this, &ActivityEditor::save);

    formLayout = new QVBoxLayout();
    formLayout->setSpacing(5);

    titleEditor = new QLineEdit("Some");

    auto *titleLabel = new QLabel("Title");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleLabel->setStyleSheet("font-weight: bold;");

    titleError = new QLabel("Already exists");
    titleError->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleError->setStyleSheet("font-weight: bold; color: red;");
    titleError->hide();

    formLayout->addWidget(titleLabel);
    formLayout->addWidget(titleEditor);
    formLayout->addWidget(titleError);
    formLayout->setMargin(8);
    mainLayout->addLayout(formLayout);

    layout()->addWidget(new QWidget());

    auto *saveAction = new QAction("Save", this);
    saveAction->setShortcut(QKeySequence(Qt::Key_Return));
    addAction(saveAction);
    connect(saveAction,
            &QAction::triggered,
            this,
            &ActivityEditor::save);


    auto *getBackAction = new QAction("Get Back", this);
    getBackAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(getBackAction);
    connect(getBackAction,
            &QAction::triggered,
            this,
            &ActivityEditor::getBack);


    titleEditor->setFocus();
}

void ActivityEditor::getBack()
{
    parentStackedWidget()->setCurrentIndex(1);
}

void ActivityEditor::save()
{
    if (!titleEditor->text().isEmpty()) {
        if (_activity.has_value()) {
            _activity.value().name = titleEditor->text().toStdString();
            emit done(_activity.value(), false);
        } else {
            auto newActivity = Activity(titleEditor->text().toStdString());
            emit done(newActivity, true);
        }
    }
}

std::optional<Activity> &ActivityEditor::getActivity()
{
    return _activity;
}

void ActivityEditor::setActivity(const std::optional<Activity> &activity)
{
    _activity = activity;
    titleEditor->setText(activity.has_value()
                         ? QString::fromStdString(activity.value().name)
                         : "");
    navBar->setTitle(activity.has_value() ? "Edit Activity" : "New Activity");
}

void ActivityEditor::showError(QString key, QString message)
{
    if (key == "Name") {
        titleError->setText(message);
        titleError->show();
    }
}

void ActivityEditor::reset(std::optional<Activity> activity)
{
    titleError->hide();
    titleEditor->setText(activity.has_value()
                         ? QString::fromStdString(activity.value().name)
                         : "");
    titleEditor->setFocus();
}

void ActivityEditor::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QStackedWidget *ActivityEditor::parentStackedWidget()
{
    return static_cast<QStackedWidget *>(parentWidget());
}
