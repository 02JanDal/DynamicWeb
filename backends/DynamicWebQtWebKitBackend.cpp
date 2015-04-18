#include "DynamicWebQtWebKitBackend_p.h"

#include <QUrl>
#include <QWebView>
#include <QWebHistory>

void *DynamicWeb_createBackend()
{
	return new DynamicWebQtWebKitBackend();
}


DynamicWebQtWebKitBackend::DynamicWebQtWebKitBackend()
	: DynamicWebBackendInterface(), m_web(new QWebView)
{
	connect(m_web, &QWebView::loadStarted, this, [this]()
	{
		m_progress = 0;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &QWebView::loadProgress, this, [this](const int progress)
	{
		m_progress = progress;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &QWebView::loadFinished, this, [this]()
	{
		m_progress = -1;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &QWebView::titleChanged, this, &DynamicWebQtWebKitBackend::titleChanged);
	connect(m_web, &QWebView::urlChanged, this, &DynamicWebQtWebKitBackend::urlChanged);
}

QUrl DynamicWebQtWebKitBackend::url() const
{
	return m_web->url();
}
void DynamicWebQtWebKitBackend::setUrl(const QUrl &url)
{
	m_web->setUrl(url);
}

bool DynamicWebQtWebKitBackend::canGoBack() const
{
	return m_web->history()->canGoBack();
}
bool DynamicWebQtWebKitBackend::canGoForward() const
{
	return m_web->history()->canGoForward();
}

QString DynamicWebQtWebKitBackend::title() const
{
	return m_web->title();
}
int DynamicWebQtWebKitBackend::loadProgress() const
{
	return m_progress;
}
bool DynamicWebQtWebKitBackend::isLoading() const
{
	return m_progress != -1;
}

QWidget *DynamicWebQtWebKitBackend::widget()
{
	return m_web;
}

void DynamicWebQtWebKitBackend::goBack()
{
	m_web->back();
}
void DynamicWebQtWebKitBackend::goForward()
{
	m_web->forward();
}

void DynamicWebQtWebKitBackend::stop()
{
	m_web->stop();
}
void DynamicWebQtWebKitBackend::reload()
{
	m_web->reload();
}
