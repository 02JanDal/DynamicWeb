#include "DynamicWebWindow.h"

#include <QLibrary>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QGlobalStatic>

#include "DynamicWebBackendInterface_p.h"

Q_GLOBAL_STATIC(QList<QSharedPointer<DynamicWebBackendInterface>>, g_backends)

static QString libSuffix()
{
#if defined(Q_OS_WIN)
	return ".dll";
#elif defined(Q_OS_UNIX)
	return ".so";
#endif
}
static QSharedPointer<DynamicWebBackendInterface> backendForId(const QString &id)
{
	// ensure g_backends is populated
	DynamicWebWindow::availableBackends();

	for (auto backend : *g_backends)
	{
		if (backend->id() == id)
		{
			return backend;
		}
	}
	return {};
}

DynamicWebWindow::DynamicWebWindow(QWidget *parent)
	: DynamicWebWindow(availableBackends().first(), parent)
{
}
DynamicWebWindow::DynamicWebWindow(const QString &backend, QWidget *parent)
	: DynamicWebWindow(backendForId(backend), parent)
{
}
DynamicWebWindow::DynamicWebWindow(QSharedPointer<DynamicWebBackendInterface> backend, QWidget *parent)
	: QWidget(parent), m_backend(backend)
{
	m_widget = m_backend->widget();
	Q_ASSERT(m_widget);
	m_widget->setParent(this);

	connect(m_backend.data(), &DynamicWebBackendInterface::urlChanged, this, [this](const QUrl &url) { setWindowTitle(url.toString()); });
	connect(m_backend.data(), &DynamicWebBackendInterface::loadProgressChanged, this, &DynamicWebWindow::loadProgress);
	connect(m_backend.data(), &DynamicWebBackendInterface::urlChanged, this, &DynamicWebWindow::urlChanged);
	connect(m_backend.data(), &DynamicWebBackendInterface::titleChanged, this, &DynamicWebWindow::titleChanged);
}

QStringList DynamicWebWindow::availableBackends()
{
	if (g_backends->isEmpty())
	{
		auto tryLoad = [](const QString &filename) -> QSharedPointer<DynamicWebBackendInterface>
		{
			if (!QLibrary::isLibrary(filename) || !QFile::exists(filename))
			{
				return {};
			}
			QLibrary lib(filename);
			if (!lib.load())
			{
				qWarning() << "DynamicWebWindow::availableBackends: Unable to load" << filename << ":" << lib.errorString();
				return {};
			}
			using Function = void *(*)();
			Function func = (Function)lib.resolve("DynamicWeb_createBackend");
			if (!func)
			{
				qWarning() << "DynamicWebWindow::availableBackends: Unable to resolve entry point for" << filename;
				return {};
			}
			return QSharedPointer<DynamicWebBackendInterface>((DynamicWebBackendInterface *)func());
		};

		// highest order: environment variable
		if (qEnvironmentVariableIsSet("DYNAMICWEB_BACKEND"))
		{
			const QString envName = qgetenv("DYNAMICWEB_BACKEND");

			QSharedPointer<DynamicWebBackendInterface> fromEnv;
			if (QFile::exists(envName))
			{
				fromEnv = tryLoad(envName);
			}
			if (fromEnv.isNull())
			{
				qWarning() << "DynamicWebWindow::availableBackends: Unable to load backend defined by environment variable";
			}
			else
			{
				g_backends->append(fromEnv);
			}
		}

		// second highest: list of known backends
		for (const QString name : {"KDEWebKit", "QtWebEngine", "QtWebKit"})
		{
			QSharedPointer<DynamicWebBackendInterface> result = tryLoad("DynamicWeb_" + name + libSuffix());
			if (!result.isNull())
			{
				g_backends->append(result);
			}
		}

		// lowest order: a null/dummy backend to prevent hard crashes
		g_backends->append(QSharedPointer<DynamicWebNullBackend>::create());
	}

	QStringList ids;
	for (const auto backend : *g_backends)
	{
		ids.append(backend->id());
	}
	return ids;
}

void DynamicWebWindow::setUrl(const QUrl &url)
{
	m_backend->setUrl(url);
}

void DynamicWebWindow::back()
{
	m_backend->goBack();
}
void DynamicWebWindow::forward()
{
	m_backend->goForward();
}
void DynamicWebWindow::reload()
{
	m_backend->reload();
}
void DynamicWebWindow::stop()
{
	m_backend->stop();
}

bool DynamicWebWindow::event(QEvent *e)
{
	QWidget::event(e);
	switch (e->type())
	{
	case QEvent::Resize:
		m_widget->resize(size());
		return true;
	default:
		return false;
	}
}
