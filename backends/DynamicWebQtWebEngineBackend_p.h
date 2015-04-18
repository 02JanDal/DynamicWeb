#pragma once

#include "DynamicWebBackendInterface_p.h"

class QWebEngineView;

class DynamicWebQtWebEngineBackend : public DynamicWebBackendInterface
{
public:
	explicit DynamicWebQtWebEngineBackend();
	~DynamicWebQtWebEngineBackend();

	QString id() const override { return "QtWebEngine"; }

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
	QWebEngineView *m_web;
	int m_progress = -1;
};
