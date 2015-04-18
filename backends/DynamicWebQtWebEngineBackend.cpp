#include "DynamicWebQtWebEngineBackend_p.h"

#include <QUrl>
#include <QWebEngineView>
#include <QWebEngineHistory>

void *DynamicWeb_createBackend()
{
	return new DynamicWebQtWebEngineBackend();
}

DynamicWebQtWebEngineBackend::DynamicWebQtWebEngineBackend()
	: DynamicWebBackendInterface(), m_web(new QWebEngineView)
{
	connect(m_web, &QWebEngineView::loadStarted, this, [this]()
	{
		m_progress = 0;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &QWebEngineView::loadProgress, this, [this](const int progress)
	{
		m_progress = progress;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &QWebEngineView::loadFinished, this, [this]()
	{
		m_progress = -1;
		emit loadProgressChanged(m_progress);
	});
	connect(m_web, &QWebEngineView::titleChanged, this, &DynamicWebQtWebEngineBackend::titleChanged);
	connect(m_web, &QWebEngineView::urlChanged, this, &DynamicWebQtWebEngineBackend::urlChanged);
}
DynamicWebQtWebEngineBackend::~DynamicWebQtWebEngineBackend()
{
	delete m_web;
}

QUrl DynamicWebQtWebEngineBackend::url() const
{
	return m_web->url();
}
void DynamicWebQtWebEngineBackend::setUrl(const QUrl &url)
{
	m_web->load(url);
}

bool DynamicWebQtWebEngineBackend::canGoBack() const
{
	return m_web->history()->canGoBack();
}
bool DynamicWebQtWebEngineBackend::canGoForward() const
{
	return m_web->history()->canGoForward();
}

QString DynamicWebQtWebEngineBackend::title() const
{
	return m_web->title();
}
int DynamicWebQtWebEngineBackend::loadProgress() const
{
	return m_progress;
}
bool DynamicWebQtWebEngineBackend::isLoading() const
{
	return m_progress != -1;
}

QWidget *DynamicWebQtWebEngineBackend::widget()
{
	return m_web;
}

void DynamicWebQtWebEngineBackend::goBack()
{
	m_web->back();
}
void DynamicWebQtWebEngineBackend::goForward()
{
	m_web->forward();
}

void DynamicWebQtWebEngineBackend::stop()
{
	m_web->stop();
}
void DynamicWebQtWebEngineBackend::reload()
{
	m_web->reload();
}
