#pragma once

#include <windows.h>

#include "DynamicWebBackendInterface_p.h"

class IWebBrowser2;

class DynamicWebWindowsBackend : public DynamicWebBackendInterface
{
public:
	explicit DynamicWebWindowsBackend();
	~DynamicWebWindowsBackend();

	QString id() const override { return "Windows"; }

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
	QWindow *m_window;
	QWidget *m_widget;
	IWebBrowser2 *m_web;
};
