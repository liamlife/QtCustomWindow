
#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"
#include "cmake_config.h"
#include <QGuiApplication>
#include <QScreen>

#ifdef Q_OS_WIN
#include "dwmapi.h" // Dwmapi.lib
#endif

namespace nametls {
	namespace view {

		MainWindow::MainWindow()
		{
			/*QPalette palette(palette());
			palette.setColor(QPalette::Background, Qt::red);
			setPalette(palette);*/

			initUI();

			updateText();

			auto rect = QGuiApplication::primaryScreen()->availableGeometry();
			qDebug() << "#________________________primary Screen rect:" << rect;

			auto screens = QGuiApplication::screens();
			for each (auto screen in screens)
			{
				qDebug() << "#____1____________________screen rect:" << screen->availableGeometry();
				qDebug() << "#____2____________________screen l:" << screen->logicalDotsPerInch();
				qDebug() << "#____3____________________screen p:" << screen->physicalDotsPerInch();
				qDebug() << "#____4____________________screen d:" << screen->devicePixelRatio();
			}

		}

		void MainWindow::initUI()
		{
			setAttribute(Qt::WA_StyledBackground, true);

			QScreen *screen = qApp->primaryScreen();
			connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onLogicalDotsPerInchChanged);

			setWidgetBorderless();


			m_iconButton = new QPushButton();
			m_iconButton->setObjectName("icon-button");

			m_titleLabel = new QLabel("Qt Custom Window");
			m_titleLabel->setObjectName("win-title-label");

			m_fullButton = new QPushButton();
			m_minButton = new QPushButton();
			m_maxButton = new QPushButton();
			m_closeButton = new QPushButton();

			m_fullButton->setObjectName("full-button");
			m_minButton->setObjectName("min-button");
			m_maxButton->setObjectName("max-button");
			m_closeButton->setObjectName("win-close-button");


			connect(m_fullButton, &QAbstractButton::clicked, this, &MainWindow::onFullscreen);
			connect(m_minButton, &QAbstractButton::clicked, this, &MainWindow::onMin);
			connect(m_maxButton, &QAbstractButton::clicked, this, &MainWindow::onMaxOrNormal);
			connect(m_closeButton, &QAbstractButton::clicked, this, &MainWindow::onClose);

			auto w = QApplication::desktop()->width();
			auto h = QApplication::desktop()->height();
			int width = 320;
			int height = 198;
			if (w < width) {
				width = w;
			}
			if (h < height) {
				height = h;
			}
			resize(width, height);
			m_testLabel = new QLabel("Hello Liam!");
			m_testLabel->setObjectName("test-label");
			m_testLabel->setAlignment(Qt::AlignCenter);

			// layout.
			auto spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

			m_titleLayout = new QHBoxLayout();
			m_titleLayout->setContentsMargins(0, 0, 0, 0);
			m_titleLayout->setSpacing(1);
			m_titleLayout->addWidget(m_iconButton);
			m_titleLayout->addWidget(m_titleLabel);
			m_titleLayout->addItem(spacer);
			m_titleLayout->addWidget(m_fullButton);
			m_titleLayout->addWidget(m_minButton);
			m_titleLayout->addWidget(m_maxButton);
			m_titleLayout->addWidget(m_closeButton);

			m_mainLayout = new QVBoxLayout(this);
			m_mainLayout->setContentsMargins(1, 1, 1, 1);
			m_mainLayout->setSpacing(0);
			m_mainLayout->addLayout(m_titleLayout);

			m_mainLayout->addWidget(m_testLabel);

		}

		void MainWindow::updateText()
		{
			auto title = tr("Qt Custom Window");
			setWindowTitle(title);
			m_titleLabel->setText(title);

			m_fullButton->setToolTip(tr("Fullscreen"));
			m_minButton->setToolTip(tr("Minimize"));
			if (windowState() == Qt::WindowMaximized) {
				m_maxButton->setToolTip(tr("Restore"));
			}
			else {
				m_maxButton->setToolTip(tr("Maximize"));
			}

			m_closeButton->setToolTip(tr("Close"));
		}

		void MainWindow::setWindowMode(int mode)
		{
			if (mode == 0) {
				exitFullscreen();
			}
			else if (mode == 1) {
				onFullscreen();
			}
		}

		void MainWindow::setWidgetBorderless()
		{
			this->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#ifdef Q_OS_WIN
			DwmEnableComposition(DWM_EC_ENABLECOMPOSITION); // windows7 need disable.
			HWND hwnd = reinterpret_cast<HWND>(winId());
			DWORD style = GetWindowLong(hwnd, GWL_STYLE);
			SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);

			// add window shadow.
			const MARGINS shadow = { 1, 1, 1, 1 };
			DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);
#endif
		}

		void MainWindow::onLogicalDotsPerInchChanged(qreal dpi)
		{
			resize(size() - QSize(1, 1));
			resize(size() + QSize(1, 1));
		}

		bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
		{
#ifdef Q_OS_WIN
			if (eventType != "windows_generic_MSG")
				return false;

			MSG* msg = static_cast<MSG*>(message);
			QWidget* widget = QWidget::find(reinterpret_cast<WId>(msg->hwnd));
			if (!widget)
				return false;

			switch (msg->message) {

			case WM_NCCALCSIZE: {
				*result = 0;
				return true;
			}

			case WM_NCHITTEST:
			{
				RECT winrect;
				GetWindowRect(HWND(winId()), &winrect);

				int x = GET_X_LPARAM(msg->lParam);
				int y = GET_Y_LPARAM(msg->lParam);

				QScreen *screen = windowHandle()->screen();

				m_dpiScale = screen->devicePixelRatio();//screen->logicalDotsPerInch() / 96.0f;


				//QPoint pt = mapFromGlobal(QPoint(int(x / m_dpiScale), int(y / m_dpiScale)));

				if (windowState() == Qt::WindowFullScreen) {
					*result = HTCLIENT;
					return true;
				}

				const LONG border_width = 5;
				bool m_bResizeable = true;
				if (m_bResizeable)
				{
					bool resizeWidth = minimumWidth() != maximumWidth();
					bool resizeHeight = minimumHeight() != maximumHeight();

					if (resizeWidth)
					{
						//left border
						if (x >= winrect.left && x < winrect.left + border_width)
						{
							*result = HTLEFT;
						}
						//right border
						if (x < winrect.right && x >= winrect.right - border_width)
						{
							*result = HTRIGHT;
						}
					}
					if (resizeHeight)
					{
						//bottom border
						if (y < winrect.bottom && y >= winrect.bottom - border_width)
						{
							*result = HTBOTTOM;
						}
						//top border
						if (y >= winrect.top && y < winrect.top + border_width)
						{
							*result = HTTOP;
						}
					}
					if (resizeWidth && resizeHeight)
					{
						//bottom left corner
						if (x >= winrect.left && x < winrect.left + border_width &&
							y < winrect.bottom && y >= winrect.bottom - border_width)
						{
							*result = HTBOTTOMLEFT;
						}
						//bottom right corner
						if (x < winrect.right && x >= winrect.right - border_width &&
							y < winrect.bottom && y >= winrect.bottom - border_width)
						{
							*result = HTBOTTOMRIGHT;
						}
						//top left corner
						if (x >= winrect.left && x < winrect.left + border_width &&
							y >= winrect.top && y < winrect.top + border_width)
						{
							*result = HTTOPLEFT;
						}
						//top right corner
						if (x < winrect.right && x >= winrect.right - border_width &&
							y >= winrect.top && y < winrect.top + border_width)
						{
							*result = HTTOPRIGHT;
						}
					}
				}
				if (0 != *result) return true;
				*result = HTCLIENT;
				//if (*result == HTCLIENT)
				//{
				double dpr = m_dpiScale; // this->devicePixelRatioF();
				auto ax = (x - winrect.left) / dpr;
				auto ay = (y - winrect.top) / dpr;
				QPoint pos(ax, ay);

				QWidget* tempWidget = this->childAt(pos);

				if (tempWidget == NULL)
				{
					*result = HTCAPTION;
					return true;
				}
				else {
					if (ay < 30) {
						QString className = tempWidget->metaObject()->className();
						if (tempWidget == m_testLabel || className == "QLabel") {
							*result = HTCAPTION;
							return true;
						}
					}
				}
				return true;
			}

			case WM_GETMINMAXINFO: {
				if (::IsZoomed(msg->hwnd)) {
					RECT frame = { 0, 0, 0, 0 };
					AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);
					frame.left = abs(frame.left);
					frame.top = abs(frame.bottom);
					widget->setContentsMargins(int(frame.left / m_dpiScale), int(frame.top / m_dpiScale), int(frame.right / m_dpiScale), int(frame.bottom / m_dpiScale));
				}
				else {
					widget->setContentsMargins(0, 0, 0, 0);
				}

				*result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
				return true;
			}
				break;

			default:
				break;
			}

#endif

			return QWidget::nativeEvent(eventType, message, result);
		}

		void MainWindow::changeEvent(QEvent* e)
		{
			if (e->type() == QEvent::WindowStateChange)
			{
				QWindowStateChangeEvent* event = static_cast<QWindowStateChangeEvent*>(e);

				if (event->oldState() & Qt::WindowMinimized)
				{
					//qDebug() << "Window restored (to normal or maximized state)!";
				}
				if (event->oldState() == Qt::WindowMaximized && this->windowState() == Qt::WindowNoState)
				{
					m_maxButton->setStyleSheet("background-image: url(:/res/svg/maximize.svg);");
					m_maxButton->setToolTip(tr("Maximize"));
				}
				else if (event->oldState() == Qt::WindowNoState && this->windowState() == Qt::WindowMaximized)
				{
					m_maxButton->setStyleSheet("background-image: url(:/res/svg/maximize2.svg);");
					m_maxButton->setToolTip(tr("Restore"));
				}
			}
		}

		void MainWindow::keyPressEvent(QKeyEvent *event)
		{
			switch (event->key())
			{
			case Qt::Key_Escape:
				exitFullscreen();
				break;
			default:
				QWidget::keyPressEvent(event);
			}
		}

		void MainWindow::onFullscreen()
		{
			if (windowState() != Qt::WindowFullScreen) {
				if (windowState() == Qt::WindowMaximized) {
					setWindowState(Qt::WindowNoState);
				}
				setWindowState(Qt::WindowFullScreen);

				QLayoutItem *childItem;
				if ((childItem = m_mainLayout->takeAt(0)) != 0)
				{
					if (childItem->layout())
					{
						childItem->layout()->setParent(NULL);
					}
					m_mainLayout->removeItem(childItem);
				}
				m_mainLayout->setContentsMargins(0, 0, 0, 0);
			}
		}

		void MainWindow::exitFullscreen()
		{
			if (windowState() == Qt::WindowFullScreen) {
				showNormal();
				m_mainLayout->insertLayout(0, m_titleLayout);

				m_mainLayout->setContentsMargins(1, 1, 1, 1);
			}
		}

		void MainWindow::onMin()
		{
			if (windowState() != Qt::WindowMinimized) {
				setWindowState(Qt::WindowMinimized);
			}
		}

		void MainWindow::onMaxOrNormal()
		{
			if (windowState() == Qt::WindowNoState) {
				setWindowState(Qt::WindowMaximized);
			}
			else {
				setWindowState(Qt::WindowNoState);
			}
		}

		void MainWindow::onClose()
		{
			close();
		}
	}
}

