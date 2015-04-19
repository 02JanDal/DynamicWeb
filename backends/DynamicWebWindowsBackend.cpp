#include "DynamicWebWindowsBackend_p.h"

#include <QUrl>
#include <QGlobalStatic>
#include <QWidget>
#include <QDebug>
#include <QGuiApplication>
#include <5.2.0/QtGui/qpa/qplatformnativeinterface.h>

#include <windows.h>
#include <exdisp.h>
#include <shlguid.h>

DYNAMICWEB_EXPORT_BACKEND("Windows", DynamicWebWindowsBackend)

static int g_oleInitialized = 0;

static QWindow *windowForWidget(QWidget *widget)
{
	if (QWindow *window = widget->windowHandle())
	{
		return window;
	}
	else if (QWidget *nativeParent = widget->nativeParentWidget())
	{
		return nativeParent->windowHandle();
	}
	else
	{
		return nullptr;
	}
}
static HWND hwndForWidget(QWidget *widget)
{
	if (QWindow *window = windowForWidget(widget))
	{
		return static_cast<HWND>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("handle", window));
	}
	else
	{
		return nullptr;
	}
}

DynamicWebWindowsBackend::DynamicWebWindowsBackend()
	: DynamicWebBackendInterface()
{
	if (g_oleInitialized == 0)
	{
		const bool res = SUCCEEDED(OleInitialize(NULL));
		Q_ASSERT(res);
	}
	g_oleInitialized++;

	IOleClientSite *oleClientSite = NULL;

	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	m_widget = new QWidget;
	HWND hwnd = hwndForWidget(m_widget);

	LPCLASSFACTORY classFactory = NULL;
	if (!CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL, IID_IClassFactory, (void **)&classFactory) && classFactory)
	{
		Q_ASSERT(classFactory);
		IOleObject *browserObject;
		if (!classFactory->CreateInstance(0, IID_IOleObject, (void **)&browserObject))
		{
			Q_ASSERT(browserObject);
			classFactory->Release();

			if (!browserObject->SetClientSite(oleClientSite))
			{
				browserObject->SetHostNames(L"My Host Name", 0);

				RECT rect;
				GetClientRect(hwnd, &rect);

				//qDebug() << SUCCEEDED(OleSetContainedObject((IUnknown *)browserObject, TRUE))
				//		 << SUCCEEDED(browserObject->DoVerb(OLEIVERB_SHOW, NULL, oleClientSite, -1, hwnd, &rect))
				//		 << SUCCEEDED(browserObject->QueryInterface(IID_IWebBrowser2, (void **)&m_web));

				if (!OleSetContainedObject((IUnknown *)browserObject, TRUE) &&
						//!browserObject->DoVerb(OLEIVERB_SHOW, NULL, oleClientSite, -1, hwnd, &rect) &&
						!browserObject->QueryInterface(IID_IWebBrowser2, (void **)&m_web))
				{
					m_web->put_Left(0);
					m_web->put_Top(0);
					m_web->put_Width(rect.right);
					m_web->put_Height(rect.bottom);
				}
				else
				{
					Q_ASSERT(false);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////´+

	/*
	CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (void **)&m_web);
	Q_ASSERT(m_web);

	HWND hwnd = NULL;
	IServiceProvider *serviceProvider = NULL;
	if (SUCCEEDED(m_web->QueryInterface(IID_IServiceProvider, (void **)&serviceProvider)))
	{
		IOleWindow *oleWindow = NULL;
		// if (SUCCEEDED(serviceProvider->QueryInterface(IID_IOleWindow, (void **)&oleWindow)))
		if (SUCCEEDED(serviceProvider->QueryService(SID_SShellBrowser, IID_IOleWindow, (void **)&oleWindow)))
		{
			if (!SUCCEEDED(oleWindow->GetWindow(&hwnd)))
			{
				Q_ASSERT_X(false, "DynamicWebWindowsBackend::DynamicWebWindowsBackend", "Unable to get HWND");
			}
			oleWindow->Release();
		}
		else
		{
			Q_ASSERT_X(false, "DynamicWebWindowsBackend::DynamicWebWindowsBackend", "Unable to get IOleWindow");
		}
		serviceProvider->Release();
	}
	else
	{
		Q_ASSERT_X(false, "DynamicWebWindowsBackend::DynamicWebWindowsBackend", "Unable to get IServiceProvider");
	}
	Q_ASSERT(hwnd);

	m_window = QWindow::fromWinId(reinterpret_cast<WId>(hwnd));
	Q_ASSERT(m_window);
	m_widget = QWidget::createWindowContainer(m_window);
	Q_ASSERT(m_widget);*/
}
DynamicWebWindowsBackend::~DynamicWebWindowsBackend()
{
	m_web->Release();

	g_oleInitialized--;
	if (g_oleInitialized)
	{
		OleUninitialize();
	}
}

QUrl DynamicWebWindowsBackend::url() const
{
	BSTR out;
	m_web->get_LocationURL(&out);
	const QString str = QString::fromWCharArray(out, SysStringLen(out));
	SysFreeString(out);
	return str;
}
void DynamicWebWindowsBackend::setUrl(const QUrl &url)
{
	VARIANT empty;
	VariantInit(&empty);

	const std::wstring str = url.toString().toStdWString();
	m_web->Navigate(SysAllocStringLen(str.data(), str.length()), &empty, &empty, &empty, &empty);
}

bool DynamicWebWindowsBackend::canGoBack() const
{
	return true;
}
bool DynamicWebWindowsBackend::canGoForward() const
{
	return true;
}

QString DynamicWebWindowsBackend::title() const
{
	BSTR out;
	m_web->get_LocationName(&out);
	const QString str = QString::fromWCharArray(out, SysStringLen(out));
	SysFreeString(out);
	return str;
}
int DynamicWebWindowsBackend::loadProgress() const
{
	// TODO actual progress reporting
	return isLoading() ? 100 : 0;
}
bool DynamicWebWindowsBackend::isLoading() const
{
	VARIANT_BOOL res;
	m_web->get_Busy(&res);
	return res == VARIANT_TRUE;
}

QWidget *DynamicWebWindowsBackend::widget()
{
	return m_widget;
}

void DynamicWebWindowsBackend::goBack()
{
	m_web->GoBack();
}
void DynamicWebWindowsBackend::goForward()
{
	m_web->GoForward();
}

void DynamicWebWindowsBackend::stop()
{
	m_web->Stop();
}
void DynamicWebWindowsBackend::reload()
{
	m_web->Refresh();
}
