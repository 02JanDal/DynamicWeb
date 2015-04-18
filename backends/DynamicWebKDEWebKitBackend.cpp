#include "DynamicWebKDEWebKitBackend_p.h"

#include <QUrl>
#include <kwebview.h>
#include <QWebHistory>

// TODO this shares 99% of code with the QtWebKit backend. unify!

void *DynamicWeb_createBackend()
{
	return new DynamicWebKDEWebKitBackend();
}


DynamicWebKDEWebKitBackend::DynamicWebKDEWebKitBackend()
	: DynamicWebBackendInterface(), m_web(new KWebView)
{
	connect(m_web, &KWebView::loadStarted, this, [this]()
	{
		m_progress = 0;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &KWebView::loadProgress, this, [this](const int progress)
	{
		m_progress = progress;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &KWebView::loadFinished, this, [this]()
	{
		m_progress = -1;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &KWebView::titleChanged, this, &DynamicWebKDEWebKitBackend::titleChanged);
	connect(m_web, &KWebView::urlChanged, this, &DynamicWebKDEWebKitBackend::urlChanged);
}

QUrl DynamicWebKDEWebKitBackend::url() const
{
	return m_web->url();
}
void DynamicWebKDEWebKitBackend::setUrl(const QUrl &url)
{
	m_web->setUrl(url);
}

bool DynamicWebKDEWebKitBackend::canGoBack() const
{
	return m_web->history()->canGoBack();
}
bool DynamicWebKDEWebKitBackend::canGoForward() const
{
	return m_web->history()->canGoForward();
}

QString DynamicWebKDEWebKitBackend::title() const
{
	return m_web->title();
}
int DynamicWebKDEWebKitBackend::loadProgress() const
{
	return m_progress;
}
bool DynamicWebKDEWebKitBackend::isLoading() const
{
	return m_progress != -1;
}

QWidget *DynamicWebKDEWebKitBackend::widget()
{
	return m_web;
}

void DynamicWebKDEWebKitBackend::goBack()
{
	m_web->back();
}
void DynamicWebKDEWebKitBackend::goForward()
{
	m_web->forward();
}

void DynamicWebKDEWebKitBackend::stop()
{
	m_web->stop();
}
void DynamicWebKDEWebKitBackend::reload()
{
	m_web->reload();
}
