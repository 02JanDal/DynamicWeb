#include "DynamicWebOSXBackend_p.h"

#include <QUrl>
#include <QWidget>

#include <CoreFoundation/CoreFoundation.h>
#include <Webkit/WebKit.h>

DYNAMICWEB_EXPORT_BACKEND("OSX", DynamicWebOSXBackend)

// code heavily borrowed from https://github.com/qtproject/qtwebview/blob/dev/src/webview/qwebview_osx.mm

@interface DWFrameLoadDelegate : NSObject
{
	DynamicWebOSXBackend *m_backend;
}
- (DWFrameLoadDelegate *)initWithBackend:(DynamicWebOSXBackend *)backend;
- (void)pageDone;
- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame;
- (void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame;
- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame;
- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame;
@end

@implementation DWFrameLoadDelegate

- (DWFrameLoadDelegate *)initWithBackend:(DynamicWebOSXBackend *)backend
{
	m_backend = backend;
	return self;
}

- (void)pageDone
{
	emit m_backend->titleChanged(m_backend->title());
	emit m_backend->loadProgressChanged(m_backend->loadProgress());
}

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
	m_backend->m_requestFrameCount++;
	if (m_backend->m_requestFrameCount == 1)
	{
		m_backend->notifyLoadStart();
	}
}
- (void)webView:(WebView *)sender didCommitLoadForFrame:(WebFrame *)frame {}
- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
	m_backend->m_requestFrameCount--;
	if (m_backend->m_requestFrameCount == 0)
	{
		[self pageDone];
		m_backend->notifyLoadEnd();
	}
}
- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
	m_backend->m_requestFrameCount--;
	if (m_backend->m_requestFrameCount == 0)
	{
		[self pageDone];
		m_backend->notifyLoadFail(QString::fromNSString([error localizedFailureReason]));
	}
}

@end

DynamicWebOSXBackend::DynamicWebOSXBackend()
	: DynamicWebBackendInterface()
{
	NSRect frame = NSMakeRect(0.0, 0.0, 400, 400);
	m_web = [[WebView alloc] initWithFrame:frame frameName:@"DynamicWeb Frame" groupName:nil];
	[m_web setFrameLoadDelegate:[[DWFrameLoadDelegate alloc] initWithBackend:this]];
	m_window = QWindow::fromWinId(reinterpret_cast<WId>(m_web));
	m_widget = QWidget::createWindowContainer(m_window);
}
DynamicWebOSXBackend::~DynamicWebOSXBackend()
{
	[m_web.frameLoadDelegate release];
	[m_web release];
}

QWidget *DynamicWebOSXBackend::widget()
{
	return m_widget;
}

QUrl DynamicWebOSXBackend::url() const
{
	return QString::fromNSString([m_web stringByEvaluatingJavaScriptFromString:@"window.location.href"]);
}
void DynamicWebOSXBackend::setUrl(const QUrl &url)
{
	m_requestFrameCount = 0;
	[[m_web mainFrame] loadRequest:[NSURLRequest requestWithURL:url.toNSURL()]];
}

bool DynamicWebOSXBackend::canGoBack() const
{
	return m_web.canGoBack;
}
bool DynamicWebOSXBackend::canGoForward() const
{
	return m_web.canGoForward;
}
void DynamicWebOSXBackend::goBack()
{
	[m_web goBack];
}
void DynamicWebOSXBackend::goForward()
{
	[m_web goForward];
}

QString DynamicWebOSXBackend::title() const
{
	return QString::fromNSString([m_web stringByEvaluatingJavaScriptFromString:@"document.title"]);
}
int DynamicWebOSXBackend::loadProgress() const
{
	// TODO actual progress reporting
	return isLoading() ? 100 : 0;
}
bool DynamicWebOSXBackend::isLoading() const
{
	return m_web.isLoading;
}

void DynamicWebOSXBackend::stop()
{
	[m_web stopLoading:m_web];
}
void DynamicWebOSXBackend::reload()
{
	[[m_web mainFrame] reload];
}

void DynamicWebOSXBackend::notifyLoadStart()
{
	emit loadProgressChanged(loadProgress());
}
void DynamicWebOSXBackend::notifyLoadEnd()
{
	emit loadProgressChanged(loadProgress());
}
void DynamicWebOSXBackend::notifyLoadFail(const QString &error)
{
	emit loadProgressChanged(loadProgress());
}
