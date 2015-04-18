#include "DynamicWebWindow.h"

#include <QLibrary>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QGlobalStatic>

#include "DynamicWebBackendInterface_p.h"

Q_GLOBAL_STATIC(QList<BackendInfo *>, g_backends)

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
		if (backend->id == id)
		{
			return QSharedPointer<DynamicWebBackendInterface>(backend->creator());
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
		auto tryLoad = [](const QString &filename) -> BackendInfo *
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
			using Function = BackendInfo *(*)();
			Function func = (Function)lib.resolve("DynamicWeb_entry");
			if (!func)
			{
				qWarning() << "DynamicWebWindow::availableBackends: Unable to resolve entry point for" << filename;
				return {};
			}
			return func();
		};

		// highest order: environment variable
		if (qEnvironmentVariableIsSet("DYNAMICWEB_BACKEND"))
		{
			const QString envName = qgetenv("DYNAMICWEB_BACKEND");

			BackendInfo *fromEnv = nullptr;
			if (QFile::exists(envName))
			{
				fromEnv = tryLoad(envName);
			}
			if (!fromEnv || !fromEnv->creator)
			{
				qWarning() << "DynamicWebWindow::availableBackends: Unable to load backend defined by environment variable";
			}
			else
			{
				g_backends->append(fromEnv);
			}
		}

		// second highest: list of known backends
		for (const QString name : {"OSX", "KDEWebKit", "QtWebEngine", "QtWebKit"})
		{
			BackendInfo *result = tryLoad("DynamicWeb_" + name + libSuffix());
			if (result && result->creator)
			{
				g_backends->append(result);
			}
		}

		// lowest order: a null/dummy backend to prevent hard crashes
		g_backends->append(new BackendInfo("Null", &DynamicWebNullBackend::create));
	}

	QStringList ids;
	for (const auto backend : *g_backends)
	{
		ids.append(backend->id);
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
