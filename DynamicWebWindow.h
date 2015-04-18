#pragma once

#include <QWidget>

//#include "DynamicWebBackendInterface_p.h"

class DynamicWebBackendInterface;

class DynamicWebWindow : public QWidget
{
	Q_OBJECT
public:
	explicit DynamicWebWindow(QWidget *parent = nullptr);
	explicit DynamicWebWindow(QSharedPointer<DynamicWebBackendInterface> backend, QWidget *parent = nullptr);
	explicit DynamicWebWindow(const QString &backend, QWidget *parent = nullptr);

	/// Returns a list of available backend, ordered by preference. The first item is prefered.
	static QStringList availableBackends();

signals:
	void loadProgress(int progress);
	void urlChanged(const QUrl &url);
	void titleChanged(const QString &title);

public slots:
	void setUrl(const QUrl &url);
	void back();
	void forward();
	void reload();
	void stop();

protected:
	bool event(QEvent *e) override;

private:
	QSharedPointer<DynamicWebBackendInterface> m_backend;
	QWidget *m_widget;
};
