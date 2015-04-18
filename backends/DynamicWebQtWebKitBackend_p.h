#pragma once

#include "DynamicWebBackendInterface_p.h"

class QWebView;

class DynamicWebQtWebKitBackend : public DynamicWebBackendInterface
{
public:
	explicit DynamicWebQtWebKitBackend();

	QString id() const override { return "QtWebKit"; }

	QUrl url() const override;
	void setUrl(const QUrl &url) override;
	bool canGoBack() const override;
	bool canGoForward() const override;
	QString title() const override;
	int loadProgress() const override;
	bool isLoading() const override;

	QWidget *widget() override;

public slots:
	void goBack() override;
	void goForward() override;
	void stop() override;
	void reload() override;

private:
	QWebView *m_web;
	int m_progress = -1;
};
