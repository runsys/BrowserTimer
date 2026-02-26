/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "browser.h"
#include "browserwindow.h"
#include "downloadmanagerwidget.h"
#include "tabwidget.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QProcess>
#include <QMessageBox>
#include <QProgressBar>
#include <QDateTime>
#include <QTimeZone>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QNetworkReply>
#include <QHttpMultiPart>
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#include <QWebEngineFindTextResult>
#endif
#include <QWebEngineProfile>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QSoundEffect>
#include <windows.h>
#include <QWebEngineSettings>
#include <QRegularExpression>
#include <cstdlib>
#include <qrandom.h>
#include <QTime>

extern     QString lin1,lin2,lin3,lin4,lin5,lin6,txt;
QString jspreout;
BrowserWindow::BrowserWindow(Browser *browser, QWebEngineProfile *profile, bool forDevTools)
    : m_browser(browser)
    , m_profile(profile)
    , m_tabWidget(new TabWidget(profile, this))
    , m_progressBar(nullptr)
    , m_historyBackAction(nullptr)
    , m_historyForwardAction(nullptr)
    , m_stopAction(nullptr)
    , m_reloadAction(nullptr)
    , m_stopReloadAction(nullptr)
    , m_urlLineEdit(nullptr)
    , m_favAction(nullptr)
{

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    auto ggfl=(24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600));
    qDebug()<<ggfl<<"  "<<GetKeyboardLayout(0)->unused;
    QFile ff4("JSout.txt");
    ff4.open(QFile::ReadOnly);
    jspreout=QString(ff4.readAll());
    ff4.close();

    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);
    if (!forDevTools) {
        m_progressBar = new QProgressBar(this);

        QToolBar *toolbar = createToolBar();
        addToolBar(toolbar);
        menuBar()->addMenu(createFileMenu(m_tabWidget));
        menuBar()->addMenu(createEditMenu());
        menuBar()->addMenu(createViewMenu(toolbar));
        menuBar()->addMenu(createWindowMenu(m_tabWidget));
        menuBar()->addMenu(createHelpMenu());





        QToolBar *toolBar2 = new QToolBar(tr("Navigation"));
        l1=new QLabel("user agent:");
        le1=new QLineEdit(lin1);
        toolBar2->addWidget(l1);
        toolBar2->addWidget(le1);
        l2=new QLabel("seconds interval to refresh page:");
        QString lin2_2;
        if(lin2.indexOf('`')!=-1){
            lin2_2=lin2.mid(lin2.indexOf('`')+1);
            lin2=lin2.mid(0,lin2.indexOf('`'));
        }
        le2=new QLineEdit(lin2);
        toolBar2->addWidget(l2);
        toolBar2->addWidget(le2);
        l3=new QLabel("the js code run after refresh page:");
        QPushButton *runjsbtn=new QPushButton("run javascript");
        connect(runjsbtn,SIGNAL(clicked(bool)),SLOT(RunJSBtnClicked()));
        auto l3layout=new QHBoxLayout;
        l3layout->addWidget(l3);
        l3layout->addWidget(runjsbtn);
        le3=new QTextEdit();
        le3->setPlainText(txt);
        toolBar2->addWidget(l3);
        toolBar2->addWidget(le3);

        l6=new QLabel("run js after load finish milisecond:");
        le6=new QLineEdit(lin5);
        toolBar2->addWidget(l6);
        toolBar2->addWidget(le6);


        toolBar2->addWidget(runjsbtn);
        l4=new QLabel("js run result text post to the url:");
        le4=new QLineEdit(lin3);
        toolBar2->addWidget(l4);
        toolBar2->addWidget(le4);
        btn=new QPushButton("start monitor");
        connect(btn,SIGNAL(clicked(bool)),SLOT(runTimer()));
        toolBar2->addWidget(btn);
        l5=new QLabel("post result:");
        le5=new QTextEdit();
        toolBar2->addWidget(l5);
        toolBar2->addWidget(le5);
        btn2=new QPushButton("manual post once");
        connect(btn2,SIGNAL(clicked(bool)),SLOT(ManualPostOnce()));
        toolBar2->addWidget(btn2);

        ck1=new QCheckBox("JS output zero length play sound");
        ck1->setChecked(true);
        ck2=new QCheckBox("JS output change play sound");
        ck3=new QCheckBox("JS output change play sound loop");
        connect(ck3,SIGNAL(clicked(bool)),SLOT(ck3clicked()));
        toolBar2->addWidget(ck1);
        toolBar2->addWidget(ck2);
        toolBar2->addWidget(ck3);

        l7=new QLabel("play sound same time run bat once:");
        le7=new QLineEdit(lin6);
        toolBar2->addWidget(l7);
        toolBar2->addWidget(le7);

        l8=new QLabel("Run EveryIntervalRunJS.txt every(must>=10s) or like 23:50week0:");
        le8=new QLineEdit();
        le8->setText(lin2_2);
        toolBar2->addWidget(l8);
        toolBar2->addWidget(le8);

        auto *teststepsbtn=new QPushButton("test steps");
        connect(teststepsbtn,SIGNAL(clicked(bool)),SLOT(TestSteps()));
        toolBar2->addWidget(teststepsbtn);

        addToolBar(Qt::LeftToolBarArea, toolBar2);


        managerPost = new QNetworkAccessManager(this);
        connect(managerPost, &QNetworkAccessManager::finished, this, &BrowserWindow::receiveReplyPost);

        setWindowTitle("Browser Timer V2 0.1");
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    if (!forDevTools) {
        addToolBarBreak();

        m_progressBar->setMaximumHeight(1);
        m_progressBar->setTextVisible(false);
        m_progressBar->setStyleSheet(QStringLiteral("QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"));

        layout->addWidget(m_progressBar);
    }

    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    if (!forDevTools) {
        connect(m_tabWidget, &TabWidget::linkHovered, [this](const QString& url) {
            statusBar()->showMessage(url);
        });
        connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
        connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);
        connect(m_tabWidget, &TabWidget::urlChanged, [this](const QUrl &url) {
            m_urlLineEdit->setText(url.toDisplayString());
        });
        connect(m_tabWidget, &TabWidget::favIconChanged, m_favAction, &QAction::setIcon);
        connect(m_tabWidget, &TabWidget::devToolsRequested, this, &BrowserWindow::handleDevToolsRequested);
        connect(m_urlLineEdit, &QLineEdit::returnPressed, [this]() {
            m_tabWidget->setUrl(QUrl::fromUserInput(m_urlLineEdit->text()));
        });
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        connect(m_tabWidget, &TabWidget::findTextFinished, this, &BrowserWindow::handleFindTextFinished);
#endif

        QAction *focusUrlLineEditAction = new QAction(this);
        addAction(focusUrlLineEditAction);
        focusUrlLineEditAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
        connect(focusUrlLineEditAction, &QAction::triggered, this, [this] () {
            m_urlLineEdit->setFocus(Qt::ShortcutFocusReason);
        });
    }

    handleWebViewTitleChanged(QString());
    m_tabWidget->createTab();
    connect(m_tabWidget,SIGNAL(loadFinished()),this,SLOT(loadFinished()));
    is_timeout_load=false;


    m_FileWatcher = new QFileSystemWatcher();
    //m_FileWatcher->addPath("d:/");
    QFile ff("MonFileForReload");
    ff.open(QFile::WriteOnly);
    ff.close();
    m_FileWatcher->addPath("MonFileForReload");
    connect(m_FileWatcher, &QFileSystemWatcher::fileChanged, this, &BrowserWindow::on_fileChanged);
    connect(m_FileWatcher, &QFileSystemWatcher::directoryChanged, this, &BrowserWindow::on_directoryChanged);
    m_urlLineEdit->setText(lin4);
    tim2=new QTimer();
    connect(tim2,SIGNAL(timeout()),this,SLOT(TimOut2()));
    tim3=new QTimer();
    connect(tim3,SIGNAL(timeout()),this,SLOT(TimOut3()));
    stepstimer=new QTimer();
    connect(stepstimer,SIGNAL(timeout()),this,SLOT(RunSteps()));

    runrelogintimer=new QTimer();
    connect(runrelogintimer,SIGNAL(timeout()),this,SLOT(RunLoginTimer()));

    keypretim=new QTimer();
    connect(keypretim,SIGNAL(timeout()),this,SLOT(keypretimRun()));

    getcolortim=new QTimer();
    connect(getcolortim,SIGNAL(timeout()),this,SLOT(GetColorTim()));


    QFile ff5("JSoutEmptyRunJS.Sample.txt");
    ff5.open(QFile::WriteOnly);
    QFile ff7(":/../JSoutEmptyRunJS.sample.txt");
    ff7.open(QFile::ReadOnly);
    auto ctt=ff7.readAll();
    ff5.write(ctt);
    ff5.close();

    auto webview=this->m_tabWidget->currentWebView();
    webview->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    webview->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    webview->page()->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
}
int keypretimg=0;
void BrowserWindow::keypretimRun(){
    if(keypretimg==2){
        keypretim->stop();
        stepstimer->start(1000);
    }
}

void BrowserWindow::ck3clicked(){
    tim3->stop();
}
void BrowserWindow::on_directoryChanged(const QString &path)
{
    qDebug() << QStringLiteral("目录发生改变--") << path;
}

bool reloadpage=false;
QString starturl;
void BrowserWindow::on_fileChanged(const QString &path)
{
    qDebug() << QStringLiteral("file发生改变--") << path;
    auto webview=this->m_tabWidget->currentWebView();
    if(webview==nullptr){
        return;
    }
    is_timeout_load=true;
    if(starturl!=""){
        if(reloadpage==false && starturl.startsWith("javascript:")){
            webview->page()->runJavaScript(starturl.mid(strlen("javascript:")),[&](const QVariant &v)
                                           {

                                           });
            tim2->start(le6->text().toInt());
        }else{
            reloadpage=false;
            if(starturl.startsWith("javascript:")){
                m_urlLineEdit->setText(starturl.mid(starturl.indexOf("//http")+2));
                emit m_urlLineEdit->returnPressed();
            }else{
                m_urlLineEdit->setText(starturl);
                emit m_urlLineEdit->returnPressed();
            }
        }
    }else{
        webview->reload();
    }
}


QString getColorstr;
void BrowserWindow::GetColorTim()
{
    getcolortim->stop();
    QStringList posls=getColorstr.split(",");
    auto color=GetPixel(GetDC(GetDesktopWindow()),posls[0].toLong(),posls[1].toLong());
    const COLORREF rgbRed   =  color&0x000000FF;
    const COLORREF rgbGreen =  (color&0x0000FF00)>>8;
    const COLORREF rgbBlue  =  (color&0x00FF0000)>>16;
    le5->setPlainText(QString("red %1 green %3 blue %4").arg(rgbRed).arg(rgbGreen).arg(rgbBlue));
}

void BrowserWindow::RunJSBtnClicked()
{
    auto webview=this->m_tabWidget->currentWebView();
    if(webview==nullptr){
        return;
    }

    QString jscode=le3->toPlainText();


    if(jscode.startsWith("testColorTrueDoNext ") || jscode.startsWith("getPointColor") || jscode.startsWith("getScreenColor")){
        QString posstr;
        if(jscode.startsWith("testColorTrueDoNext "))
            posstr=jscode.mid(QString("testColorTrueDoNext ").size());
        if(jscode.startsWith("getPointColor"))
            posstr=jscode.mid(QString("getPointColor").size());
        if(jscode.startsWith("getScreenColor"))
            posstr=jscode.mid(QString("getScreenColor").size());
        getColorstr=posstr.trimmed();
        qDebug()<<"mouse move to"<<posstr;
        getcolortim->start(3000);
    }else if(jscode.startsWith("getScreenWidth") || jscode.startsWith("testScreenWidthEqualDoNext ")){
        QString posstr;
        if(jscode.startsWith("testScreenWidthEqualDoNext "))
            posstr=jscode.mid(QString("testScreenWidthEqualDoNext ").size());
        if(jscode.startsWith("getScreenWidth"))
            posstr=jscode.mid(QString("getScreenWidth").size());
        posstr=posstr.trimmed();
        qDebug()<<"mouse move to"<<posstr;
        int cx=GetSystemMetrics(SM_CXFULLSCREEN);
        le5->setPlainText(QString("screen width %1").arg(cx));
    }else if(jscode.startsWith("getScreenHeight")){
        QString posstr;
        if(jscode.startsWith("testScreenHeightEqual "))
            posstr=jscode.mid(QString("testScreenHeightEqual ").size());
        if(jscode.startsWith("getScreenHeight"))
            posstr=jscode.mid(QString("getScreenHeight").size());
        posstr=posstr.trimmed();
        qDebug()<<"mouse move to"<<posstr;
        QStringList posls=posstr.split(",");
        int cy=GetSystemMetrics(SM_CYFULLSCREEN);
        le5->setPlainText(QString("screen height %1").arg(cy));
    }else if(jscode.startsWith("cacheClear")){
        qDebug()<<"cacheClear";
        webview->page()->profile()->clearHttpCache();
    }else{
        webview->page()->runJavaScript(jscode,[&](const QVariant &v)
           {
               QString retStr = v.toString();
               le5->setPlainText(retStr);
           });

    }
}

int eee=1;


void click(QWidget* wid,QPoint pos){
    QMouseEvent *press = new QMouseEvent(QEvent::MouseButtonPress,
                                         pos,
                                         Qt::LeftButton,
                                         Qt::MouseButton::NoButton,
                                         Qt::NoModifier);
    QCoreApplication::postEvent(wid, press);
    // Some delay
    QTimer::singleShot(300, [pos, wid]() {
        QMouseEvent *release = new QMouseEvent(QEvent::MouseButtonRelease,
                                               pos,
                                               Qt::LeftButton,
                                               Qt::MouseButton::NoButton,
                                               Qt::NoModifier);
        QCoreApplication::postEvent(wid, release);
    });

}

void BrowserWindow::ManualPostOnce()
{

//    auto webview2=this->m_tabWidget->currentWebView();
//    //document.getElementById('J-login-btn').


//    // You need to find the first child widget of QWebEngineView. It can accept user input events.
//    QWidget* eventsReciverWidget = nullptr;
//    auto cls=webview2->children();
//    for(int i=0;i<cls.size();i++){
//        auto obj=cls[i];
//        QWidget* wgt = qobject_cast<QWidget*>(obj);
//        if(wgt!=nullptr){
//            qDebug()<<wgt->objectName();
//            QPoint clickPos(1210, 177);
//            click(wgt, clickPos);
//        }
//    }


//    for(int i=0;i<cls.size();i++){
//        auto obj=cls[i];
//        QWidget* wgt = qobject_cast<QWidget*>(obj);
//        if(wgt!=nullptr){
//            qDebug()<<wgt->objectName();
//            QPoint clickPos(760, 370);
//            click(wgt, clickPos);
//        }
//    }

//    return;


    auto webview=this->m_tabWidget->currentWebView();
    if(webview==nullptr){
        return;
    }
    is_timeout_load=true;
    if(starturl!=""){
        if(reloadpage==false && starturl.startsWith("javascript:")){
            webview->page()->runJavaScript(starturl.mid(strlen("javascript:")),[&](const QVariant &v)
                                           {

                                           });
            tim2->start(le6->text().toInt());
        }else{
            reloadpage=false;
            if(starturl.startsWith("javascript:")){
                m_urlLineEdit->setText(starturl.mid(starturl.indexOf("//http")+2));
                emit m_urlLineEdit->returnPressed();
            }else{
                m_urlLineEdit->setText(starturl);
                emit m_urlLineEdit->returnPressed();
            }
        }
    }else{
        webview->reload();
    }

}

QTimer *tim=nullptr;
bool binmon=false;
void BrowserWindow::runTimer()
{
    QString txt;
    lin1=le1->text();
    txt+=le1->text()+"\n";
    lin2=le2->text();
    txt+=le2->text()+"`"+le8->text()+"\n";
    lin3=le4->text();
    txt+=le4->text()+"\n";
    if(lin4==""){
        txt+=m_urlLineEdit->text()+"\n";
        starturl=m_urlLineEdit->text();
    }else{
        txt+=lin4+"\n";
        starturl=lin4;
    }
    lin5=le6->text();
    txt+=le6->text()+"\n";
    lin6=le7->text();
    txt+=le7->text()+"\n";
    txt+=le3->toPlainText();
    QFile ff("browserTimer_config.txt");
    ff.open(QFile::WriteOnly);
    ff.write(txt.toUtf8());
    ff.close();

    QWebEngineProfile *defaultProfile = QWebEngineProfile::defaultProfile();
    if(le1->text()!=""){
        if(defaultProfile->httpUserAgent()!=le1->text())
            defaultProfile->setHttpUserAgent(le1->text());
    }else{
        defaultProfile->setHttpUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/107.0.0.0 Safari/537.36");
    }
    CreateDirectoryW(L"cache",NULL);
    CreateDirectoryW(L"data",NULL);
    defaultProfile->setCachePath("cache");
    defaultProfile->setPersistentStoragePath("data");



    tim=new QTimer();
    connect(tim,SIGNAL(timeout()),this,SLOT(TimOut()));
    tim->start(le2->text().toInt()*1000);
    if(le8->text().size()>=10){
        if(le8->text().indexOf(":")!=-1){
            runrelogintimer->start(50*1000);
        }else{
            runrelogintimer->start(le8->text().toLong()*1000);
        }
    }
    btn->setEnabled(false);
    binmon=true;
}

bool binrunsteps=false;
bool loadfinishreloadtotrue=false;
void BrowserWindow::TimOut()
{
    tim->stop();
    auto webview=this->m_tabWidget->currentWebView();
    if(webview==nullptr){
        return;
    }
    is_timeout_load=true;
    if(binrunsteps==false){
        if(reloadpage==false && starturl.startsWith("javascript:")){
            webview->page()->runJavaScript(starturl.mid(strlen("javascript:")),[&](const QVariant &v)
                                           {

                                           });
            tim2->start(le6->text().toInt());
        }else{
            reloadpage=false;
            if(starturl!=""){
                if(starturl.startsWith("javascript:")){
                    m_urlLineEdit->setText(starturl.mid(starturl.indexOf("//http")+2));
                    emit m_urlLineEdit->returnPressed();
                }else{
                    m_urlLineEdit->setText(starturl);
                    emit m_urlLineEdit->returnPressed();
                }
            }else{
                webview->reload();
            }
        }
    }
    if(loadfinishreloadtotrue){
        reloadpage=true;
    }
    QFile ff("DoNear24Hour.txt");
    if(ff.exists() && (24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600))>=8 && (24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600))<=le2->text().toInt()){
        tim->start((24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600))*1000);
        loadfinishreloadtotrue=true;
    }else if(ff.exists() && (24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600))>=8 && (24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600))<=le2->text().toInt()*2){
        tim->start((24*3600-8-(QDateTime::currentDateTime().toSecsSinceEpoch()+QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()))%(24*3600))/2*1000);
    }else{
        if(loadfinishreloadtotrue){
            loadfinishreloadtotrue=false;
            tim->start(150*1000);
        }else{
            tim->start(le2->text().toInt()*1000);
        }
    }
}

void BrowserWindow::loadFinished()
{
    if(!is_timeout_load){
        return;
    }
    is_timeout_load=false;
    tim2->start(le6->text().toInt());
}




void BrowserWindow::TimOut2()
{
    tim2->stop();
    auto webview=this->m_tabWidget->currentWebView();
    if(webview==nullptr){
        return;
    }
    //webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    webview->page()->runJavaScript(le3->toPlainText(),[&](const QVariant &v)
    {
           //webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
           QString retStr = v.toString();
           JSoutTxt=retStr;

           // 设置url
           QNetworkRequest reqPost;
           reqPost.setUrl(QUrl(le4->text()));
           // 设置请求头
           reqPost.setRawHeader("accept", "*/*");
           reqPost.setRawHeader("Accept-Language", "zh-CN");
           reqPost.setRawHeader("Content-Type", "text/plain;charset=UTF-8");
           QByteArray m_boundary;
           m_boundary.clear();
           m_boundary.append("multipart/form-data; boundary=");

           QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);


           m_boundary.append(multipart->boundary());
           reqPost.setRawHeader(QByteArray("Content-Type"), m_boundary);
           QSslConfiguration sslConfig;
           sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
           sslConfig.setProtocol(QSsl::TlsV1SslV3);
           reqPost.setSslConfiguration(sslConfig);

          // text/plain数据
           QHttpPart textPart1;
           textPart1.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
           textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"txt\""));
           textPart1.setBody(JSoutTxt.toUtf8());
           multipart->append(textPart1);

           // 发送post请求
           replyPost = managerPost->post(reqPost, multipart);
           multipart->setParent(replyPost);// 用 reply 删除 multiPart
           //auto er=replyPost->error();
           //QMessageBox::information(nullptr, "提示", QString("Error:%1").arg(er)+QString("length%1 ").arg(le4->text().length())+ " return "+er + le4->text()+" "+QString(JSoutTxt+replyPost->readAll()));
       });


}




QSoundEffect *effect=new QSoundEffect;
QStringList steps;
int steps_currentpos=-1;

QString Trim(QString str,QString tristr){
    while(str.size()>0 && tristr.indexOf(QChar(str.at(0)))!=-1){
        str=str.mid(1);
    }
    while(str.size()>0 && tristr.indexOf(QChar(str.at(str.length()-1)))!=-1){
        str=str.mid(0,str.length()-1);
    }
    return str;
}



void BrowserWindow::RunLoginTimer(){
    if(le8->text().indexOf(":")!=-1){
        auto cutdt=QDateTime::currentDateTime();
        auto da=cutdt.date();
        auto weeknum=da.weekNumber();
        auto tim=cutdt.time();
        auto hour=tim.hour();
        auto minute=tim.minute();
        QString ss;
        if(le8->text().indexOf("week")!=-1){
        ss=QString("%1:%2week%3")
                .arg(int(hour),2,10,QChar('0'))
                .arg(int(minute),2,10,QChar('0'))
                .arg(int(weeknum));
        if(ss!=le8->text()){
            return;
        }
        }else if(le8->text().indexOf("day")!=-1){
            ss=QString("%1:%2day%3")
                    .arg(int(hour),2,10,QChar('0'))
                    .arg(int(minute),2,10,QChar('0'))
                    .arg(int(da.dayOfYear()%le8->text().mid(le8->text().indexOf("day")+3).toInt()+1));
            if(ss!=le8->text()){
                return;
            }
        }
    }
    runrelogintimer->stop();
    QFile ff("EveryIntervalRunJS.txt");
    ff.open(QFile::ReadOnly);
    auto ctt=ff.readAll();
    ff.close();
    steps=QStringList();
    if(ctt.size()>0){
        steps=QString(ctt).split("step:");
        //qDebug()<<"start steps:"<<steps;
        for(int i=0;i<steps.size();i++)
            steps[i]=Trim(steps.at(i)," \r\n\t");
        steps.removeAll("");
        qDebug()<<"second steps:"<<steps;
    }
    if(steps.size()>0){
        steps_currentpos=0;
        binrunsteps=true;
        stepstimer->start(1000);
    }
}


void mouse_mov(int x,int y)
{
    unsigned int dx, dy;
    dx = (double)65535/GetSystemMetrics(SM_CXSCREEN)*(double)x;
    dy = (double)65535/GetSystemMetrics(SM_CYSCREEN)*(double)y;
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
void BrowserWindow::TestSteps(){
    QFile ff("EveryIntervalRunJS.txt");
    ff.open(QFile::ReadOnly);
    auto ctt=ff.readAll();
    ff.close();
    steps=QStringList();
    if(ctt.size()>0){
        steps=QString(ctt).split("step:");
        //qDebug()<<"start steps:"<<steps;
        for(int i=0;i<steps.size();i++)
            steps[i]=Trim(steps.at(i)," \r\n\t");
        steps.removeAll("");
        qDebug()<<"second steps:"<<steps;
    }
    if(steps.size()>0){
        steps_currentpos=0;
        binrunsteps=true;
        stepstimer->start(1000);
    }
}



void mouse_click2(int x,int y)
{
    //移至固定位置右击
    //获取鼠标当前位置
    POINT curpt;
    GetCursorPos(&curpt);
    //隐藏鼠标
    //ShowCursor(false);
    //将其移到指定位置
    SetCursorPos(x,y);
    Sleep(10);
    //右击
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(10);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    //将鼠标移回起始位置
    //SetCursorPos(curpt.x,curpt.y);
    //显示鼠标
    //ShowCursor(true);
}


void mouse_click(int x,int y)
{
    unsigned int dx, dy;
    dx = (double)65535/GetSystemMetrics(SM_CXSCREEN)*(double)x;
    dy = (double)65535/GetSystemMetrics(SM_CYSCREEN)*(double)y;
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, dx, dy, 0, 0);
    Sleep(10);
    mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
    Sleep(10);
    mouse_event(MOUSEEVENTF_LEFTUP,x,y,0,0);
}

void mouse_move(int x,int y)
{
    unsigned int dx, dy;
    dx = (double)65535/GetSystemMetrics(SM_CXSCREEN)*(double)x;
    dy = (double)65535/GetSystemMetrics(SM_CYSCREEN)*(double)y;
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}

struct PressKKKK {
    QString key;
    QTimer *tim;
};
DWORD WINAPI creditbuycheckthrd(LPVOID p) {
    PressKKKK *str = (PressKKKK*)p;
    QString posstr=str->key;
    for(int i=0;i<posstr.size();i++){
        if(posstr.at(i)=='\\'){
            if(posstr.at(i+1)=='r'){
                keybd_event('\r', 0, 0, 0);
                keybd_event('\r', 0, KEYEVENTF_KEYUP, 0);
                i+=1;
            }else if(posstr.at(i+1)=='n'){
                keybd_event(VK_RETURN, 0, 0, 0);
                keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                i+=1;
            }else if(posstr.at(i+1)=='\\'){
                keybd_event(VK_OEM_5, 0, 0, 0);
                Sleep(100);
                keybd_event(VK_OEM_5, 0, KEYEVENTF_KEYUP, 0);
                Sleep(100);
                i+=1;
            }else if(posstr.at(i+1)=='t'){
                keybd_event(VK_TAB, 0, 0, 0);
                keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
                i+=1;
            }else  if(i+2<posstr.size() && posstr.at(i+1).isDigit() && posstr.at(i+2).isDigit()){
                char ch=(char)posstr.mid(i+1,2).toInt(nullptr,16);
                keybd_event(ch, 0, 0, 0);
                keybd_event(ch, 0, KEYEVENTF_KEYUP, 0);
                i+=2;
            }else{
                keybd_event((char)posstr.at(i+1).toLatin1(), 0, 0, 0);
                keybd_event((char)posstr.at(i+1).toLatin1(), 0, KEYEVENTF_KEYUP, 0);
                i+=1;
            }
            Sleep(100);
        }else if(posstr.at(i)=='@'){
            keybd_event(VK_LSHIFT, 0, 0, 0);
            keybd_event('2', 0, 0, 0);
            Sleep(100);
            keybd_event('2', 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='.'){
            keybd_event(VK_OEM_PERIOD, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='+'){
            keybd_event(VK_OEM_PLUS, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_PLUS, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='-'){
            keybd_event(VK_OEM_MINUS, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_MINUS, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)==','){
            keybd_event(VK_OEM_COMMA, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_COMMA, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='/'){
            keybd_event(VK_OEM_2, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_2, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='~'){
            keybd_event(VK_OEM_3, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_3, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='.'){
            keybd_event(VK_OEM_PERIOD, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)==';'){
            keybd_event(VK_OEM_1, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_1, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='\\'){
            keybd_event(VK_OEM_5, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_5, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='\''){
            keybd_event(VK_OEM_7, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_7, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)=='['){
            keybd_event(VK_OEM_4, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_4, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else if(posstr.at(i)==']'){
            keybd_event(VK_OEM_6, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_OEM_6, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }else{
            keybd_event((char)toupper(posstr.at(i).toLatin1()), 0, 0, 0);
            Sleep(100);
            keybd_event((char)toupper(posstr.at(i).toLatin1()), 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
        }
    }
    keypretimg=2;
    //str->tim->start(1000);
    return 0;
}


bool binJSoutNoneEmptyChangedRunJS=false;
void BrowserWindow::RunSteps(){
    stepstimer->stop();
    auto webview=this->m_tabWidget->currentWebView();
    if(webview==nullptr){
        return;
    }
    if(steps_currentpos<steps.size()){
        qDebug()<<"steps_currentpos<steps.size()"<<steps_currentpos<<steps.size()<<steps.at(steps_currentpos);//<<steps;
        if(steps.at(steps_currentpos).startsWith("activeMainWindow")){
             qDebug()<<"active winid"<<this->winId();
            //
            //SwitchToThisWindow((HWND)this->winId(),FALSE);
            //SetForegroundWindow((HWND)this->winId());
            //this->activateWindow();
            //ShowWindow((HWND)this->winId(),SW_RESTORE);
             SetWindowPos((HWND)this->winId(),HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE );
            this->showMaximized();
            SwitchToThisWindow((HWND)this->winId(),TRUE);
            SetForegroundWindow((HWND)this->winId());
            SetWindowPos((HWND)this->winId(),HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE );
            steps_currentpos+=1;
            stepstimer->start(2000);
        }else if(steps.at(steps_currentpos).startsWith("sysMouseMove ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("sysMouseMove ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"mouse move to"<<posstr;
            QStringList posls=posstr.split(" ");
            if(posls.size()>=2){
                mouse_mov(posls[0].toLong(),posls[1].toLong());
            }
            steps_currentpos+=1;
            stepstimer->start(1000);
        }else if(steps.at(steps_currentpos).startsWith("pressKey ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("pressKey ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"presskey:"<<posstr;
            auto kk=new PressKKKK;
            kk->key=posstr;
            kk->tim=stepstimer;
            keypretimg=1;
            CreateThread(0, 1024 * 1024, creditbuycheckthrd, kk, 0, 0);
            keypretim->start(100);
            /*
            for(int i=0;i<posstr.size();i++){
                if(posstr.at(i)=='\\'){
                    if(posstr.at(i+1)=='r'){
                        keybd_event('\r', 0, 0, 0);
                        keybd_event('\r', 0, KEYEVENTF_KEYUP, 0);
                        i+=1;
                    }else if(posstr.at(i+1)=='n'){
                        keybd_event(VK_RETURN, 0, 0, 0);
                        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                        i+=1;
                    }else if(posstr.at(i+1)=='\\'){
                        keybd_event(VK_OEM_5, 0, 0, 0);
                        Sleep(100);
                        keybd_event(VK_OEM_5, 0, KEYEVENTF_KEYUP, 0);
                        Sleep(100);
                        i+=1;
                    }else if(posstr.at(i+1)=='t'){
                        keybd_event(VK_TAB, 0, 0, 0);
                        keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
                        i+=1;
                    }else  if(i+2<posstr.size() && posstr.at(i+1).isDigit() && posstr.at(i+2).isDigit()){
                        char ch=(char)posstr.mid(i+1,2).toInt(nullptr,16);
                        keybd_event(ch, 0, 0, 0);
                        keybd_event(ch, 0, KEYEVENTF_KEYUP, 0);
                        i+=2;
                    }else{
                        keybd_event((char)posstr.at(i+1).toLatin1(), 0, 0, 0);
                        keybd_event((char)posstr.at(i+1).toLatin1(), 0, KEYEVENTF_KEYUP, 0);
                        i+=1;
                    }
                    Sleep(100);
                }else if(posstr.at(i)=='@'){
                    keybd_event(VK_LSHIFT, 0, 0, 0);
                    keybd_event('2', 0, 0, 0);
                    Sleep(100);
                    keybd_event('2', 0, KEYEVENTF_KEYUP, 0);
                    keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='.'){
                    keybd_event(VK_OEM_PERIOD, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='+'){
                    keybd_event(VK_OEM_PLUS, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_PLUS, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='-'){
                    keybd_event(VK_OEM_MINUS, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_MINUS, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)==','){
                    keybd_event(VK_OEM_COMMA, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_COMMA, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='/'){
                    keybd_event(VK_OEM_2, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_2, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='~'){
                    keybd_event(VK_OEM_3, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_3, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='.'){
                    keybd_event(VK_OEM_PERIOD, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)==';'){
                    keybd_event(VK_OEM_1, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_1, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='\\'){
                    keybd_event(VK_OEM_5, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_5, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='\''){
                    keybd_event(VK_OEM_7, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_7, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)=='['){
                    keybd_event(VK_OEM_4, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_4, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else if(posstr.at(i)==']'){
                    keybd_event(VK_OEM_6, 0, 0, 0);
                    Sleep(100);
                    keybd_event(VK_OEM_6, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }else{
                    keybd_event((char)toupper(posstr.at(i).toLatin1()), 0, 0, 0);
                    Sleep(100);
                    keybd_event((char)toupper(posstr.at(i).toLatin1()), 0, KEYEVENTF_KEYUP, 0);
                    Sleep(100);
                }
            }
            */
            steps_currentpos+=1;
            //stepstimer->start(1000);
        }else if(steps.at(steps_currentpos).startsWith("turnKeyboardLayout")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("turnKeyboardLayout").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"turnKeyboardLayout "<<posstr;
            QStringList posls=posstr.split(",");
            keybd_event(VK_LCONTROL, 0, 0, 0);
            keybd_event(VK_SPACE, 0, 0, 0);
            Sleep(100);
            keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
            Sleep(100);
            steps_currentpos+=1;
            stepstimer->start(1000);
        }else if(steps.at(steps_currentpos).startsWith("testColorTrueDoNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("testColorTrueDoNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"testColorTrueDoNext "<<posstr;
            QStringList posls=posstr.split(",");
            if(posls.size()>=6){
                auto color=GetPixel(GetDC(GetDesktopWindow()),posls[1].toLong(),posls[2].toLong());
                const COLORREF rgbRed   =  color&0x000000FF;
                const COLORREF rgbGreen =  (color&0x0000FF00)>>8;
                const COLORREF rgbBlue  =  (color&0x00FF0000)>>16;
                qDebug()<<"testColorTrueDoNext "<<rgbRed<<"  "<<rgbGreen<<"  "<<rgbBlue;
                if(fabs(rgbRed-posls[3].toLong())<=10 && fabs(rgbGreen-posls[4].toLong())<=10 && fabs(rgbBlue-posls[5].toLong())<=10 ){
                    qDebug()<<"testColorTrueDoNext aabbcc";
                }else{
                    qDebug()<<"testColorTrueDoNext eeffgg";
                    steps_currentpos+=posls[0].toLong();
                }
            }
            steps_currentpos+=1;
            stepstimer->start(1000);
        }else if(steps.at(steps_currentpos).startsWith("testColorTrueDontNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("testColorTrueDontNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"testColorTrueDontNext "<<posstr;
            QStringList posls=posstr.split(",");
            if(posls.size()>=6){
                auto color=GetPixel(GetDC(GetDesktopWindow()),posls[1].toLong(),posls[2].toLong());
                const COLORREF rgbRed   =  color&0x000000FF;
                const COLORREF rgbGreen =  (color&0x0000FF00)>>8;
                const COLORREF rgbBlue  =  (color&0x00FF0000)>>16;
                qDebug()<<"testColorTrueDontNext "<<rgbRed<<"  "<<rgbGreen<<"  "<<rgbBlue;
                if(fabs(rgbRed-posls[3].toLong())<=5 && fabs(rgbGreen-posls[4].toLong())<=5 && fabs(rgbBlue-posls[5].toLong())<=5 ){
                    steps_currentpos+=posls[0].toLong();
                }else{

                }
            }
            steps_currentpos+=1;
            stepstimer->start(1000);
        }else if(steps.at(steps_currentpos).startsWith("testScreenWidthEqualDoNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("testScreenWidthEqualDoNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"testScreenWidthDoNext "<<posstr;
            QStringList posls=posstr.split(",");
            int cx=GetSystemMetrics(SM_CXFULLSCREEN);
            if(cx==posls[1].toInt()){

            }else{
                steps_currentpos+=posls[0].toInt();
            }
            steps_currentpos+=1;
            stepstimer->start(100);
        }else if(steps.at(steps_currentpos).startsWith("rand01DoNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("rand01DoNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"rand01DoNext "<<posstr;
            QStringList posls=posstr.split(",");
            if(qrand()%2==0){

            }else{
                steps_currentpos+=posls[0].toInt();
            }
            steps_currentpos+=1;
            stepstimer->start(100);
        }else if(steps.at(steps_currentpos).startsWith("testScreenWidthEqualDontNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("testScreenWidthEqualDoNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"testScreenWidthDoNext "<<posstr;
            QStringList posls=posstr.split(",");
            int cx=GetSystemMetrics(SM_CXFULLSCREEN);
            if(cx==posls[1].toInt()){
                steps_currentpos+=posls[0].toInt();
            }else{

            }
            steps_currentpos+=1;
            stepstimer->start(100);
        }else if(steps.at(steps_currentpos).startsWith("testScreenHeightEqualDoNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("testScreenHeightEqualDoNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"testScreenHeightDoNext "<<posstr;
            QStringList posls=posstr.split(",");
            int cx=GetSystemMetrics(SM_CYFULLSCREEN);
            if(cx==posls[1].toInt()){

            }else{
                steps_currentpos+=posls[0].toInt();
            }
            steps_currentpos+=1;
            stepstimer->start(100);
        }else if(steps.at(steps_currentpos).startsWith("testScreenHeightEqualDontNext ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("testScreenHeightEqualDoNext ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"testScreenHeightDoNext "<<posstr;
            QStringList posls=posstr.split(",");
            int cx=GetSystemMetrics(SM_CYFULLSCREEN);
            if(cx==posls[1].toInt()){
                steps_currentpos+=posls[0].toInt();
            }else{

            }
            steps_currentpos+=1;
            stepstimer->start(100);
        }else if(steps.at(steps_currentpos).startsWith("sleep ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("sleep ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"sleep msec"<<posstr;
            //Sleep(posstr.toInt());
            steps_currentpos+=1;
            stepstimer->start(posstr.toInt());
        }else if(steps.at(steps_currentpos).startsWith("loadurl ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("loadurl ").size())," \r\n\t");
            qDebug()<<"step run url"<<posstr;
            if(posstr.startsWith("javascript:")){
                //webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
                webview->page()->runJavaScript(posstr.mid(QString("javascript:").size()),[&](const QVariant &v)
                   {
                       //webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
                       QString jsreturl=v.toString();
                       QRegularExpression regex2("https?://");
                       if(regex2.match(jsreturl).hasMatch()){
                           m_urlLineEdit->setText(jsreturl);
                           emit m_urlLineEdit->returnPressed();
                       }
                       steps_currentpos+=1;
                       stepstimer->start(8000);
                   });
            }else{
                m_urlLineEdit->setText(posstr);
                emit m_urlLineEdit->returnPressed();
                steps_currentpos+=1;
                stepstimer->start(8000);
            }
        }else if(steps.at(steps_currentpos).startsWith("mouseclick ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("mouseclick ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"step run mouseclick"<<posstr;
            QStringList posls=posstr.split(" ");
            if(posls.size()>=2){
                auto cls=webview->children();
                for(int i=0;i<cls.size();i++){
                    auto obj=cls[i];
                    QWidget* wgt = qobject_cast<QWidget*>(obj);
                    if(wgt!=nullptr){
                        qDebug()<<wgt->objectName();
                        qDebug()<<"step run mouseclick2"<<posls[0].toLong()<< posls[1].toLong();
                        mouse_click(posls[0].toLong(), posls[1].toLong());
                        break;
                    }
                }
            }
            steps_currentpos+=1;
            stepstimer->start(1000);
        }else if(steps.at(steps_currentpos).startsWith("linkclick ")){
            QString posstr=Trim(steps.at(steps_currentpos).mid(QString("linkclick ").size())," \r\n\t");
            posstr=posstr.trimmed();
            qDebug()<<"step run linkclick"<<posstr;
            QStringList posls=posstr.split(" ");
            if(posls.size()>=2){
                auto cls=webview->children();
                for(int i=0;i<cls.size();i++){
                    auto obj=cls[i];
                    QWidget* wgt = qobject_cast<QWidget*>(obj);
                    if(wgt!=nullptr){
                        qDebug()<<wgt->objectName();
                        qDebug()<<"step run mouseclick2"<<posls[0].toLong()<< posls[1].toLong();
                        mouse_click(posls[0].toLong(), posls[1].toLong());
                        break;
                    }
                }
            }
            steps_currentpos+=1;
            stepstimer->start(8000);
        }else if(steps.at(steps_currentpos).startsWith("clearCache")){
            webview->page()->profile()->clearHttpCache();
            steps_currentpos+=1;
            stepstimer->start(3000);
        }else{
           //webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
           webview->page()->runJavaScript(steps[steps_currentpos],[&](const QVariant &v)
           {
                //webview->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
           });
           steps_currentpos+=1;
           stepstimer->start(1000);
        }
    }else{
        binJSoutNoneEmptyChangedRunJS=false;
        steps_currentpos=0;
        binrunsteps=false;
        stepstimer->stop();
        if(binmon){
            tim2->start(le6->text().toInt());
        }
    }
}


// 接收http响应
void BrowserWindow::receiveReplyPost(QNetworkReply *reply)
{
    // 获取状态码
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // 获取响应数据
    QString reply_data = reply->readAll();

    le5->setPlainText("status_code:"+QString("%1").arg(status_code)+"  "+reply_data+"\n"+lin4+" JSOUT:"+JSoutTxt);
    if(JSoutTxt!="" && reply_data.indexOf("txt_seg_length 0")!=-1){
        tim2->start(100);
    }

    QFile ff2("JSout.txt");
    ff2.open(QFile::ReadOnly);
    auto oldctt=ff2.readAll();
    ff2.close();

    QFile ff3("JSoutCheckRegex.txt");
    ff3.open(QFile::ReadOnly);
    auto jsoutckrestr=ff3.readAll();
    ff3.close();
    QFile ff4("JSoutEmptyRunJSUrlRegex.txt");
    ff4.open(QFile::ReadOnly);
    auto jsurlrestr=ff4.readAll();
    ff4.close();

    QRegularExpression regex2(jsurlrestr);
    if(JSoutTxt=="" && binJSoutNoneEmptyChangedRunJS==false){
        QFile ff("JSoutEmptyRunJS.txt");
        ff.open(QFile::ReadOnly);
        auto ctt=ff.readAll();
        ff.close();
        steps=QStringList();
        if(ctt.size()>0){
            steps=QString(ctt).split("step:");
           for(int i=0;i<steps.size();i++)
                steps[i]=Trim(steps.at(i)," \r\n\t");
            steps.removeAll("");
        }
        if(steps.size()>0){
            steps_currentpos=0;
            binrunsteps=true;
            stepstimer->start(3000);
        }else{
            if(ck1->isChecked()){
                effect->setSource(QUrl::fromLocalFile("jet.wav"));
                //循环播放
                effect->setLoopCount(1);
                //设置音量，0-1
                effect->setVolume(0.8f);
                effect->play();
                if(lin6!=""){
                    QProcess process;
                    process.start(lin6, QStringList());
                    process.waitForFinished();
                }
                this->statusBar()->showMessage("最后变更时间:"+QDateTime::currentDateTime().toString()+" 变更后内容:"+JSoutTxt.mid(0,64)+" 变更前内容:"+QString(oldctt).mid(0,64));
            }
        }
    }else{
        QRegularExpression regex(jsoutckrestr);
        if(jsoutckrestr=="" && jspreout!=JSoutTxt || jsoutckrestr!="" && JSoutTxt.contains(regex) && jspreout!=JSoutTxt){
            bool be=false;
            if(ck2->isChecked()){
                effect->setSource(QUrl::fromLocalFile("jet.wav"));
                //循环播放
                effect->setLoopCount(1);
                //设置音量，0-1
                effect->setVolume(0.8f);
                effect->play();
                if(lin6!=""){
                    QProcess process;
                    process.start(lin6, QStringList());
                    process.waitForFinished();
                }
                time3execnt=1;
                be=true;
            }
            if(ck3->isChecked()){
                if(be==false)time3execnt=0;
                tim3->start(60000);
            }
            this->statusBar()->showMessage("最后变更时间:"+QDateTime::currentDateTime().toString()+" 变更后内容:"+JSoutTxt.mid(0,64)+" 变更前内容:"+QString(oldctt).mid(0,64));
            jspreout=JSoutTxt;
            QFile ff("JSout.txt");
            ff.open(QFile::WriteOnly|QFile::Truncate);
            ff.write(JSoutTxt.toUtf8());
            ff.close();
            if(lin6!=""){
                QProcess process;
                process.start(lin6, QStringList());
                process.waitForFinished();
            }
            if(QFile::exists("JSoutNoneEmptyChangedRunJS.txt")){
                QFile ff("JSoutNoneEmptyChangedRunJS.txt");
                ff.open(QFile::ReadOnly);
                auto ctt=QString(ff.readAll());
                ff.close();
                ctt=ctt.replace("__currentjsout__",JSoutTxt);
                steps=QStringList();
                if(ctt.size()>0){
                    steps=QString(ctt).split("step:");
                   for(int i=0;i<steps.size();i++)
                        steps[i]=Trim(steps.at(i)," \r\n\t");
                    steps.removeAll("");
                }
                if(steps.size()>0){
                    steps_currentpos=0;
                    binrunsteps=true;
                    binJSoutNoneEmptyChangedRunJS=true;
                    stepstimer->start(3000);
                }
            }
        }else{

        }
    }
}


void BrowserWindow::TimOut3()
{
    if(time3execnt==0){
        QProcess process;
        process.start(lin6, QStringList());
        process.waitForFinished();
    }
    time3execnt+=1;
    QFile ff("no_loop_sound_hour_range.txt");
    ff.open(QFile::ReadOnly);
    auto ffctt=ff.readAll();
    ff.close();
    QStringList rnghour=QString(ffctt).split(" ");
    if(rnghour.size()<2 || !(rnghour.size()>=2 && QTime::currentTime().hour()>=rnghour[0].toLong() && QTime::currentTime().hour()<=rnghour[1].toLong())){
        effect->setSource(QUrl::fromLocalFile("jet.wav"));
        //循环播放
        effect->setLoopCount(1);
        //设置音量，0-1
        effect->setVolume(0.8f);
        effect->play();
    }
}


QSize BrowserWindow::sizeHint() const
{
    QRect desktopRect = QApplication::primaryScreen()->geometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}

QMenu *BrowserWindow::createFileMenu(TabWidget *tabWidget)
{
    QMenu *fileMenu = new QMenu(tr("&File"));
    fileMenu->addAction(tr("&New Window"), this, &BrowserWindow::handleNewWindowTriggered, QKeySequence::New);
    fileMenu->addAction(tr("New &Incognito Window"), this, &BrowserWindow::handleNewIncognitoWindowTriggered);

    QAction *newTabAction = new QAction(tr("New &Tab"), this);
    newTabAction->setShortcuts(QKeySequence::AddTab);
    connect(newTabAction, &QAction::triggered, this, [this]() {
        m_tabWidget->createTab();
        m_urlLineEdit->setFocus();
    });
    fileMenu->addAction(newTabAction);

    fileMenu->addAction(tr("&Open File..."), this, &BrowserWindow::handleFileOpenTriggered, QKeySequence::Open);
    fileMenu->addSeparator();

    QAction *closeTabAction = new QAction(tr("&Close Tab"), this);
    closeTabAction->setShortcuts(QKeySequence::Close);
    connect(closeTabAction, &QAction::triggered, [tabWidget]() {
        tabWidget->closeTab(tabWidget->currentIndex());
    });
    fileMenu->addAction(closeTabAction);

    QAction *closeAction = new QAction(tr("&Quit"),this);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(closeAction);

    connect(fileMenu, &QMenu::aboutToShow, [this, closeAction]() {
        if (m_browser->windows().count() == 1)
            closeAction->setText(tr("&Quit"));
        else
            closeAction->setText(tr("&Close Window"));
    });
    return fileMenu;
}

QMenu *BrowserWindow::createEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"));
    QAction *findAction = editMenu->addAction(tr("&Find"));
    findAction->setShortcuts(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &BrowserWindow::handleFindActionTriggered);

    QAction *findNextAction = editMenu->addAction(tr("Find &Next"));
    findNextAction->setShortcut(QKeySequence::FindNext);
    connect(findNextAction, &QAction::triggered, [this]() {
        if (!currentTab() || m_lastSearch.isEmpty())
            return;
        currentTab()->findText(m_lastSearch);
    });

    QAction *findPreviousAction = editMenu->addAction(tr("Find &Previous"));
    findPreviousAction->setShortcut(QKeySequence::FindPrevious);
    connect(findPreviousAction, &QAction::triggered, [this]() {
        if (!currentTab() || m_lastSearch.isEmpty())
            return;
        currentTab()->findText(m_lastSearch, QWebEnginePage::FindBackward);
    });

    return editMenu;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolbar)
{
    QMenu *viewMenu = new QMenu(tr("&View"));
    m_stopAction = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stopAction->setShortcuts(shortcuts);
    connect(m_stopAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Stop);
    });

    m_reloadAction = viewMenu->addAction(tr("Reload Page"));
    m_reloadAction->setShortcuts(QKeySequence::Refresh);
    connect(m_reloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Reload);
    });

    QAction *zoomIn = viewMenu->addAction(tr("Zoom &In"));
    zoomIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    });

    QAction *zoomOut = viewMenu->addAction(tr("Zoom &Out"));
    zoomOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    });

    QAction *resetZoom = viewMenu->addAction(tr("Reset &Zoom"));
    resetZoom->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoom, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(1.0);
    });


    viewMenu->addSeparator();
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+|"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible()) {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);

    QAction *viewStatusbarAction = new QAction(tr("Hide Status Bar"), this);
    viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    connect(viewStatusbarAction, &QAction::triggered, [this, viewStatusbarAction]() {
        if (statusBar()->isVisible()) {
            viewStatusbarAction->setText(tr("Show Status Bar"));
            statusBar()->close();
        } else {
            viewStatusbarAction->setText(tr("Hide Status Bar"));
            statusBar()->show();
        }
    });
    viewMenu->addAction(viewStatusbarAction);
    return viewMenu;
}

QMenu *BrowserWindow::createWindowMenu(TabWidget *tabWidget)
{
    QMenu *menu = new QMenu(tr("&Window"));

    QAction *nextTabAction = new QAction(tr("Show Next Tab"), this);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
    nextTabAction->setShortcuts(shortcuts);
    connect(nextTabAction, &QAction::triggered, tabWidget, &TabWidget::nextTab);

    QAction *previousTabAction = new QAction(tr("Show Previous Tab"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
    previousTabAction->setShortcuts(shortcuts);
    connect(previousTabAction, &QAction::triggered, tabWidget, &TabWidget::previousTab);

    connect(menu, &QMenu::aboutToShow, [this, menu, nextTabAction, previousTabAction]() {
        menu->clear();
        menu->addAction(nextTabAction);
        menu->addAction(previousTabAction);
        menu->addSeparator();

        QVector<BrowserWindow*> windows = m_browser->windows();
        int index(-1);
        for (auto window : windows) {
            QAction *action = menu->addAction(window->windowTitle(), this, &BrowserWindow::handleShowWindowTriggered);
            action->setData(++index);
            action->setCheckable(true);
            if (window == this)
                action->setChecked(true);
        }
    });
    return menu;
}

QMenu *BrowserWindow::createHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"));
    helpMenu->addAction(tr("About &Qt"), qApp, QApplication::aboutQt);
    return helpMenu;
}

QToolBar *BrowserWindow::createToolBar()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    navigationBar->setMovable(false);
    navigationBar->toggleViewAction()->setEnabled(false);

    m_historyBackAction = new QAction(this);
    QList<QKeySequence> backShortcuts = QKeySequence::keyBindings(QKeySequence::Back);
    for (auto it = backShortcuts.begin(); it != backShortcuts.end();) {
        // Chromium already handles navigate on backspace when appropriate.
        if ((*it)[0] == Qt::Key_Backspace)
            it = backShortcuts.erase(it);
        else
            ++it;
    }
    // For some reason Qt doesn't bind the dedicated Back key to Back.
    backShortcuts.append(QKeySequence(Qt::Key_Back));
    m_historyBackAction->setShortcuts(backShortcuts);
    m_historyBackAction->setIconVisibleInMenu(false);
    m_historyBackAction->setIcon(QIcon(QStringLiteral(":go-previous.png")));
    m_historyBackAction->setToolTip(tr("Go back in history"));
    connect(m_historyBackAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Back);
    });
    navigationBar->addAction(m_historyBackAction);

    m_historyForwardAction = new QAction(this);
    QList<QKeySequence> fwdShortcuts = QKeySequence::keyBindings(QKeySequence::Forward);
    for (auto it = fwdShortcuts.begin(); it != fwdShortcuts.end();) {
        if (((*it)[0] & Qt::Key_unknown) == Qt::Key_Backspace)
            it = fwdShortcuts.erase(it);
        else
            ++it;
    }
    fwdShortcuts.append(QKeySequence(Qt::Key_Forward));
    m_historyForwardAction->setShortcuts(fwdShortcuts);
    m_historyForwardAction->setIconVisibleInMenu(false);
    m_historyForwardAction->setIcon(QIcon(QStringLiteral(":go-next.png")));
    m_historyForwardAction->setToolTip(tr("Go forward in history"));
    connect(m_historyForwardAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Forward);
    });
    navigationBar->addAction(m_historyForwardAction);

    m_stopReloadAction = new QAction(this);
    connect(m_stopReloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::WebAction(m_stopReloadAction->data().toInt()));
    });
    navigationBar->addAction(m_stopReloadAction);

    m_urlLineEdit = new QLineEdit(this);
    m_favAction = new QAction(this);
    m_urlLineEdit->addAction(m_favAction, QLineEdit::LeadingPosition);
    m_urlLineEdit->setClearButtonEnabled(true);
    navigationBar->addWidget(m_urlLineEdit);

    auto downloadsAction = new QAction(this);
    downloadsAction->setIcon(QIcon(QStringLiteral(":go-bottom.png")));
    downloadsAction->setToolTip(tr("Show downloads"));
    navigationBar->addAction(downloadsAction);
    connect(downloadsAction, &QAction::triggered, [this]() {
        m_browser->downloadManagerWidget().show();
    });

    return navigationBar;
}

void BrowserWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action) {
    case QWebEnginePage::Back:
        m_historyBackAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Forward:
        m_historyForwardAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
        m_reloadAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Stop:
        m_stopAction->setEnabled(enabled);
        break;
    default:
        qWarning("Unhandled webActionChanged signal");
    }
}

void BrowserWindow::handleWebViewTitleChanged(const QString &title)
{
    QString suffix = m_profile->isOffTheRecord()
        ? tr("Qt Simple Browser (Incognito)")
        : tr("Qt Simple Browser");

    if (title.isEmpty())
        setWindowTitle(suffix);
    else
        setWindowTitle(title + " - " + suffix);
}

void BrowserWindow::handleNewWindowTriggered()
{
    BrowserWindow *window = m_browser->createWindow();
    window->m_urlLineEdit->setFocus();
}

void BrowserWindow::handleNewIncognitoWindowTriggered()
{
    BrowserWindow *window = m_browser->createWindow(/* offTheRecord: */ true);
    window->m_urlLineEdit->setFocus();
}

void BrowserWindow::handleFileOpenTriggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QString(),
                                                tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (url.isEmpty())
        return;
    currentTab()->setUrl(url);
}

void BrowserWindow::handleFindActionTriggered()
{
    if (!currentTab())
        return;
    bool ok = false;
    QString search = QInputDialog::getText(this, tr("Find"),
                                           tr("Find:"), QLineEdit::Normal,
                                           m_lastSearch, &ok);
    if (ok && !search.isEmpty()) {
        m_lastSearch = search;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        currentTab()->findText(m_lastSearch);
#else
        currentTab()->findText(m_lastSearch, 0, [this](bool found) {
            if (!found)
                statusBar()->showMessage(tr("\"%1\" not found.").arg(m_lastSearch));
        });
#endif
    }
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("Are you sure you want to close the window ?\n"
                                          "There are %1 tabs open.").arg(m_tabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

TabWidget *BrowserWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

void BrowserWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon(QStringLiteral(":process-stop.png"));
    static QIcon reloadIcon(QStringLiteral(":view-refresh.png"));

    if (0 < progress && progress < 100) {
        m_stopReloadAction->setData(QWebEnginePage::Stop);
        m_stopReloadAction->setIcon(stopIcon);
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
        m_progressBar->setValue(progress);
    } else {
        m_stopReloadAction->setData(QWebEnginePage::Reload);
        m_stopReloadAction->setIcon(reloadIcon);
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
        m_progressBar->setValue(0);
    }
}

void BrowserWindow::handleShowWindowTriggered()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        int offset = action->data().toInt();
        QVector<BrowserWindow*> windows = m_browser->windows();
        windows.at(offset)->activateWindow();
        windows.at(offset)->currentTab()->setFocus();
    }
}

void BrowserWindow::handleDevToolsRequested(QWebEnginePage *source)
{
    source->setDevToolsPage(m_browser->createDevToolsWindow()->currentTab()->page());
    source->triggerAction(QWebEnginePage::InspectElement);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
void BrowserWindow::handleFindTextFinished(const QWebEngineFindTextResult &result)
{
    if (result.numberOfMatches() == 0) {
        statusBar()->showMessage(tr("\"%1\" not found.").arg(m_lastSearch));
    } else {
        statusBar()->showMessage(tr("\"%1\" found: %2/%3").arg(m_lastSearch,
                                                               QString::number(result.activeMatch()),
                                                               QString::number(result.numberOfMatches())));
    }
}
#endif
