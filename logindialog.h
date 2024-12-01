#pragma once

#include "ui_logindialog.h"
#include "common.h"

#include <QMouseEvent>
#include <QRegExp>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialogClass; };
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = nullptr);
	~LoginDialog();

signals:
    void sigLoginSuccess(QString userName);

protected:
	void mousePressEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
	void sltBtnRegFontClicked();

	void sltBtnLoginClicked();

	void sltBtnRegClicked();

	void sltBtnServerSetClicked();

private:
	void readConfigFile();

    // void serverSet();

    void saveLoginInfoData(QString userName, QString token, QString ip, QString port);

private:
	Ui::LoginDialogClass *ui;

	QPoint position_;
	bool isMoving_;
	Common* common_;

    QNetworkAccessManager *m_manager;
};
