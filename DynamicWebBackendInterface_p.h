#pragma once

#include <QWindow>

class DynamicWebBackendInterface : public QObject
{
	Q_OBJECT
public:
	explicit DynamicWebBackendInterface() {}
	virtual ~DynamicWebBackendInterface() {}

	virtual QString id() const = 0;

	// web handling
	virtual QUrl url() const = 0;
	virtual void setUrl(const QUrl &url) = 0;
	virtual bool canGoBack() const = 0;
	virtual bool canGoForward() const = 0;
	virtual QString title() const = 0;
	virtual int loadProgress() const = 0;
	virtual bool isLoading() const = 0;

	virtual QWidget *widget() = 0;

public slots:
	virtual void goBack() = 0;
	virtual void goForward() = 0;
	virtual void stop() = 0;
	virtual void reload() = 0;

signals:
	void titleChanged(const QString &title);
	void urlChanged(const QUrl &url);
	void loadProgressChanged(const int progress);

private:
	Q_DISABLE_COPY(DynamicWebBackendInterface)
};

class DynamicWebNullBackend : public DynamicWebBackendInterface
{
	Q_OBJECT
public:
	explicit DynamicWebNullBackend() {}

	QString id() const override { return "Null"; }

	QUrl url() const override;
	void setUrl(const QUrl &url) override;
	bool canGoBack() const override;
	bool canGoForward() const override;
	QString title() const override;
	int loadProgress() const override;
	bool isLoading() const override;

	QWidget *widget() override { return nullptr; }

public slots:
	void goBack() override;
	void goForward() override;
	void stop() override;
	void reload() override;
};

extern "C"
{
extern void *DynamicWeb_createBackend();
}
