
#ifndef CUSTOMWEBENGINEVIEW_H
#define CUSTOMWEBENGINEVIEW_H

#include <QWebEngineView>



class CustomWebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    CustomWebEngineView(QWidget* parent = nullptr)
        : QWebEngineView(parent)
    {}
    QWebEngineView* createWindow(QWebEnginePage::WebWindowType type) override
    {
        return this;
    }
};

#endif // CUSTOMWEBENGINEVIEW_H
