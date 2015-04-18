#pragma once

#include "DynamicWebBackendInterface_p.h"

Q_FORWARD_DECLARE_OBJC_CLASS(WebView);

class DynamicWebOSXBackend : public DynamicWebBackendInterface
{
public:
	explicit DynamicWebOSXBackend();
	~DynamicWebOSXBackend();

	QString id() const override { return "OSX"; }

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
	WebView *m_web;
	QWidget *m_widget;
	QWindow *m_window;

public:
	int m_requestFrameCount;

	void notifyLoadStart();
	void notifyLoadEnd();
	void notifyLoadFail(const QString &error);
};
