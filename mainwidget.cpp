#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    init();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::init()
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/img/cloud.png"));
}

void MainWidget::on_tBtnClose_clicked()
{
    this->close();
}

