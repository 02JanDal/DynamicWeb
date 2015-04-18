#include "DynamicWebBackendInterface_p.h"

#include <QDebug>
#include <QUrl>

QUrl DynamicWebNullBackend::url() const
{
	qCritical() << "DynamicWebNullBackend::url: Null backend";
	return QUrl();
}
void DynamicWebNullBackend::setUrl(const QUrl &url)
{
	qCritical() << "DynamicWebNullBackend::setUrl: Null backend";
}

bool DynamicWebNullBackend::canGoBack() const
{
	qCritical() << "DynamicWebNullBackend::canGoBack: Null backend";
	return false;
}
bool DynamicWebNullBackend::canGoForward() const
{
	qCritical() << "DynamicWebNullBackend::canGoForward: Null backend";
	return false;
}

QString DynamicWebNullBackend::title() const
{
	qCritical() << "DynamicWebNullBackend::title: Null backend";
	return QString();
}
int DynamicWebNullBackend::loadProgress() const
{
	qCritical() << "DynamicWebNullBackend::loadProgress: Null backend";
	return 0;
}
bool DynamicWebNullBackend::isLoading() const
{
	qCritical() << "DynamicWebNullBackend::isLoading: Null backend";
	return false;
}

void DynamicWebNullBackend::goBack()
{
	qCritical() << "DynamicWebNullBackend::goBack: Null backend";
}
void DynamicWebNullBackend::goForward()
{
	qCritical() << "DynamicWebNullBackend::goForward: Null backend";
}
void DynamicWebNullBackend::stop()
{
	qCritical() << "DynamicWebNullBackend::stop: Null backend";
}
void DynamicWebNullBackend::reload()
{
	qCritical() << "DynamicWebNullBackend::reload: Null backend";
}
