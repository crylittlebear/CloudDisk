#pragma execution_character_set("utf-8")

#include "logindialog.h"

#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
// #include "logininfoinstance.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
	, ui(new Ui::LoginDialogClass())
{
	ui->setupUi(this);

	setWindowFlag(Qt::FramelessWindowHint);

	isMoving_ = false;

    common_ = Common::instance();
    m_manager = Common::instance()->getNetworkAccessManager();

	//mainWindow_ = new Mainwindow;

	connect(ui->widgetTitle, &LoginTitle::sigBtnConfigClicked, [=](){
		ui->stackedWidget->setCurrentWidget(ui->pageConfig);
	});

	connect(ui->widgetTitle, &LoginTitle::sigBtnMinClicked, [=](){
		this->showMinimized();
	});

	connect(ui->widgetTitle, &LoginTitle::sigBtnCloseClicked, [=](){
		if (ui->stackedWidget->currentWidget() == ui->pageConfig) {
			ui->stackedWidget->setCurrentWidget(ui->pageLogin);
		} else {
			this->close();
		}
	});

	connect(ui->tBtnRegFont, &QToolButton::clicked, this, &LoginDialog::sltBtnRegFontClicked);
	connect(ui->toolButtonLogin, &QToolButton::clicked, this, &LoginDialog::sltBtnLoginClicked);
	connect(ui->tBtnReg, &QToolButton::clicked, this, &LoginDialog::sltBtnRegClicked);
	connect(ui->tBtnServerSet, &QToolButton::clicked, this, &LoginDialog::sltBtnServerSetClicked);

	ui->stackedWidget->setCurrentWidget(ui->pageLogin);

	ui->lineEditUser->setIconPath(":/img/user.png");
	ui->lineEditPasswd->setIconPath(":/img/lock.png");

	readConfigFile();
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		isMoving_ = true;
		position_ = event->pos();
	}
}

void LoginDialog::mouseMoveEvent(QMouseEvent* event)
{
	if (isMoving_ && (event->buttons() & Qt::LeftButton)) {
		this->move(event->globalPos() - position_);
	}
}

void LoginDialog::mouseReleaseEvent(QMouseEvent* event)
{
	if (isMoving_) {
		isMoving_ = false;
	}
}

void LoginDialog::sltBtnLoginClicked()
{
	QString userName = ui->lineEditUser->text();
	QString passwd = ui->lineEditPasswd->text();

	QRegExp reg(USER_REG);
	if (!reg.exactMatch(userName)) {
		QMessageBox::warning(this, "警告", "用户名格式不正确");
		ui->lineEditUser->clear();
		ui->lineEditUser->setFocus();
		return;
	}
	reg.setPattern(PASSWD_REG);
	if (!reg.exactMatch(passwd)) {
		QMessageBox::warning(this, "警告", "密码格式不正确");
		ui->lineEditPasswd->clear();
		ui->lineEditPasswd->setFocus();
		return;
	}

	// 发送HTTP请求
	QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
	QString url(QString("http://%1:%2/login").arg(ip).arg(port));
	qDebug() << "url = " << url;
	request.setUrl(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	// 将文本数据封装成json

	QJsonObject paramObj;
	paramObj.insert("user", userName);
	paramObj.insert("pwd", common_->getStrMD5(passwd));

	QJsonDocument document(paramObj);

	QByteArray data = document.toJson();
    qDebug() << "data = " << data;

    QNetworkReply* reply = m_manager->post(request, data);

	// 读取服务器返回的数据
	connect(reply, &QNetworkReply::readyRead, this, [=](){
		// 读数据
		QByteArray data = reply->readAll();
		qDebug() << data;

		// 解析返回的数据
		QJsonParseError err;
		QJsonDocument document = QJsonDocument::fromJson(data, &err);
		if (err.error != QJsonParseError::NoError) {
			QMessageBox::critical(this, "错误", "解析服务器返回消息失败");
		} else {
			QJsonObject rootObj = document.object();
			QJsonValue codeValue = rootObj.value("code");
			if (codeValue.type() == QJsonValue::String) {
				QString msg = codeValue.toString();
				if ("000" == msg) {
					QMessageBox::information(this, "消息", "登录成功");
					// 将用户信息保存到cfg.json文件
					bool isCheck = ui->checkBox->isChecked();
					if (isCheck == false) {
						ui->lineEditPasswd->setText("");
					}
					// 保存配置信息
					common_->writeLoginInfo(userName, passwd, isCheck);

                    //获取token
                    QJsonValue tokenValue = rootObj.value("token");
                    saveLoginInfoData(userName, tokenValue.toString(), ip, port);

					// 显示主窗口，隐藏登录窗口
                    this->hide();
                    emit sigLoginSuccess(userName);

				} else if ("001" == msg) {
					QMessageBox::critical(this, "错误", "登录失败");
				}
			}
		}
	});
}

void LoginDialog::sltBtnRegClicked()
{
	QString user = ui->lineEditRegUser->text();
	QString nickName = ui->lineEditRegNickName->text();
	QString passwd = ui->lineEditRegPasswd->text();
	QString passwdConfirm = ui->lineEditRegPasswdConfirm->text();
	QString phone = ui->lineEditRegPhone->text();
	QString email = ui->lineEditRegEmail->text();
	// 校验输入

	// 用户名校验
	QRegExp regName(USER_REG);
	if (!regName.exactMatch(user)) {
		QMessageBox::warning(this, "警告", "用户名格式不正确");
		ui->lineEditRegUser->clear();
		ui->lineEditRegUser->setFocus();
		return;
	}

	// 昵称校验
	if (!regName.exactMatch(nickName)) {
		QMessageBox::warning(this, "警告", "昵称格式不正确");
		ui->lineEditRegNickName->clear();
		ui->lineEditRegNickName->setFocus();
		return;
	}

	// 密码校验

	// 首先检查两次密码是否一样
	if (passwd != passwdConfirm) {
		QMessageBox::warning(this, "警告", "两次密码输入不一致");
		ui->lineEditRegPasswd->setFocus();
		return;
	}

	QRegExp regPasswd(PASSWD_REG);

	if (!regPasswd.exactMatch(passwd)) {
		QMessageBox::warning(this, "警告", "密码格式不正确");
		ui->lineEditRegPasswd->clear();
		ui->lineEditRegPasswd->setFocus();
		return;
	}

	if (!regPasswd.exactMatch(passwdConfirm)) {
		QMessageBox::warning(this, "警告", "确认密码格式不正确");
		ui->lineEditRegPasswdConfirm->clear();
		ui->lineEditRegPasswdConfirm->setFocus();
		return;
	}

	// 电话号码校验
	QRegExp regPhone(PHONE_REG);

	if (!regPhone.exactMatch(phone)) {
		QMessageBox::warning(this, "警告", "电话号码格式不正确");
		ui->lineEditRegPhone->clear();
		ui->lineEditRegPhone->setFocus();
		return;
	}

	// 邮箱校验
	QRegExp regMail(EMAIL_REG);

	if (!regMail.exactMatch(email)) {
		QMessageBox::warning(this, "警告", "邮箱格式不正确");
		ui->lineEditRegEmail->clear();
		ui->lineEditRegEmail->setFocus();
		return;
	}

	// 发送HTTP请求
	QNetworkAccessManager* manager = new QNetworkAccessManager(this);
	QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
	QString url(QString("http://%1:%2/reg").arg(ip).arg(port));
	qDebug() << "URL：" << url;
	request.setUrl(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	// 将文本数据封装成json

	QJsonObject paramObj;
	paramObj.insert("email", email);
	paramObj.insert("userName", user);
	paramObj.insert("phone", phone);
	paramObj.insert("nickName", nickName);
	paramObj.insert("firstPwd", common_->getStrMD5(passwd));

	QJsonDocument document(paramObj);

	QByteArray data = document.toJson();

	QNetworkReply* reply = manager->post(request, data);

	// 读取服务器返回的数据
	connect(reply, &QNetworkReply::readyRead, this, [=](){
		// 读数据
		QByteArray data = reply->readAll();
		qDebug() << data;

		// 解析返回的数据
		QJsonParseError err;
		QJsonDocument document = QJsonDocument::fromJson(data, &err);
		if (err.error != QJsonParseError::NoError) {
			QMessageBox::critical(this, "错误", "解析服务器返回消息失败");
		} else {
			QJsonObject rootObj = document.object();
			QJsonValue codeValue = rootObj.value("code");
			if (codeValue.type() == QJsonValue::String) {
				QString msg = codeValue.toString();
				if ("002" == msg) {
					QMessageBox::information(this, "消息", "注册成功");
					ui->stackedWidget->setCurrentWidget(ui->pageLogin);
				} else if ("003" == msg) {
					QMessageBox::information(this, "消息", "用户已存在");
				} else if ("004" == msg) {
					QMessageBox::critical(this, "错误", "注册失败");
				}
			}
		}
	});
}

void LoginDialog::sltBtnServerSetClicked()
{
	QString ip = ui->lineEditIP->text();
	QString port = ui->lineEditPort->text();
	qDebug() << "port = " << port;

	// 验证IP和地址的格式

	QRegExp ipExp(IP_REG);
	if (!ipExp.exactMatch(ip)) {
		QMessageBox::warning(this, "警告", "IP地址格式错误");
		return;
	}

	QRegExp portExp(PORT_REG);
	if (!portExp.exactMatch(port)) {
		QMessageBox::warning(this, "警告", "端口格式错误");
		return;
	}

	// 将IP和端口信息保存到config文件
	ui->stackedWidget->setCurrentWidget(ui->pageLogin);
	common_->writeWebInfo(ip, port);
}

void LoginDialog::readConfigFile()
{
    QString user = common_->getConfigValue("login", "user");
    QString passwd = common_->getConfigValue("login", "pwd");
    QString remember = common_->getConfigValue("login", "remember");
	if (remember == "yes") {
		// 记住密码
		// 1. base64解密
		QByteArray pwdTemp = QByteArray::fromBase64(passwd.toLocal8Bit());
		// 2. des解密
		unsigned char pwdDec[1024];
		int pwdDecLen = 0;
		int ret = DesDec((unsigned char*)pwdTemp.data(), pwdTemp.size(), pwdDec, &pwdDecLen);
		if (ret != 0) {
			qDebug() << "解密失败";
		}
		QString pwd = QString::fromLocal8Bit((const char*)(pwdDec), pwdDecLen);
        // qDebug() << "密码：" << pwd;

		ui->checkBox->setChecked(true);
		ui->lineEditPasswd->setText(pwd);
	} else {
		ui->checkBox->setChecked(false);
		ui->lineEditPasswd->setText("");
	}

	QByteArray userTemp = QByteArray::fromBase64(user.toLocal8Bit());
	// 2. des解密
	unsigned char userDec[1024];
	int userDecLen = 0;
	int ret = DesDec((unsigned char*)userTemp.data(), userTemp.size(), userDec, &userDecLen);
	if (ret != 0) {
		qDebug() << "解密失败";
	}
	QString userName = QString::fromLocal8Bit((const char*)(userDec), userDecLen);
    // qDebug() << "用户名：" << userName;
	ui->lineEditUser->setText(userName);


    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
	if (ip != "") {
		ui->lineEditIP->setText(ip);
	}
	if (port != "") {
		ui->lineEditPort->setText(port);
	}
}

// void LoginDialog::serverSet()
// {
//     QString ip = m_common->getConfValue("web_server", "ip");
//     QString port = m_common->getConfValue("web_server", "port");
//     qDebug() << "ip:" << ip << ",port" << port;
//     ui->server_ip->setText(ip);
//     ui->server_port->setText(port);
// }

void LoginDialog::saveLoginInfoData(QString userName, QString token, QString ip, QString port)
{
    //跳转到其他页面
    //保存数据, token, user, ip, 端口
    //除了登录外：每一个请求都需要校验token,每一个请求都需要带token
    // LoginInfoInstance *loginInfo = LoginInfoInstance::getInstance();
    // qDebug() << "token:" << token;

    // loginInfo->setUser(userName);
    // loginInfo->setToken(token);
    // loginInfo->setIp(ip);
    // loginInfo->setPort(port);
}

void LoginDialog::sltBtnRegFontClicked() 
{
	ui->stackedWidget->setCurrentWidget(ui->pageRegister);
}
