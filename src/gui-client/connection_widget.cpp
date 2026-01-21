#include "connection_widget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QRandomGenerator>

#include "common/gui/theme.h"

namespace veil::gui {

ConnectionWidget::ConnectionWidget(QWidget* parent) : QWidget(parent) {
  setupUi();
  setupAnimations();
  setServerAddress("vpn.example.com", 4433);
}

void ConnectionWidget::setupUi() {
  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(24);
  mainLayout->setContentsMargins(spacing::kPaddingXLarge, spacing::kPaddingXLarge,
                                  spacing::kPaddingXLarge, spacing::kPaddingXLarge);

  // === Header with logo ===
  auto* headerLayout = new QHBoxLayout();
  auto* logoLabel = new QLabel("VEIL", this);
  logoLabel->setStyleSheet(QString("font-size: %1px; font-weight: 700; color: %2;")
                               .arg(fonts::kFontSizeHeadline)
                               .arg(colors::dark::kAccentPrimary));
  headerLayout->addWidget(logoLabel);
  headerLayout->addStretch();
  mainLayout->addLayout(headerLayout);

  // === Status Card ===
  statusCard_ = new QWidget(this);
  statusCard_->setObjectName("statusCard");
  statusCard_->setStyleSheet(R"(
    #statusCard {
      background-color: rgba(255, 255, 255, 0.05);
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 16px;
      padding: 24px;
    }
  )");

  auto* statusCardLayout = new QVBoxLayout(statusCard_);
  statusCardLayout->setAlignment(Qt::AlignCenter);
  statusCardLayout->setSpacing(12);

  // Status indicator and text in a horizontal layout
  auto* statusRowLayout = new QHBoxLayout();
  statusRowLayout->setAlignment(Qt::AlignCenter);
  statusRowLayout->setSpacing(12);

  statusIndicator_ = new QLabel(this);
  statusIndicator_->setFixedSize(16, 16);
  statusIndicator_->setStyleSheet(QString("background-color: %1; border-radius: 8px;")
                                      .arg(colors::dark::kTextSecondary));
  statusRowLayout->addWidget(statusIndicator_);

  statusLabel_ = new QLabel("Disconnected", this);
  statusLabel_->setStyleSheet(QString("font-size: %1px; font-weight: 600; color: %2;")
                                  .arg(fonts::kFontSizeTitle)
                                  .arg(colors::dark::kTextSecondary));
  statusRowLayout->addWidget(statusLabel_);

  statusCardLayout->addLayout(statusRowLayout);

  // Error label (hidden by default)
  errorLabel_ = new QLabel(this);
  errorLabel_->setWordWrap(true);
  errorLabel_->setAlignment(Qt::AlignCenter);
  errorLabel_->setStyleSheet(QString("color: %1; font-size: %2px; padding: 8px;")
                                 .arg(colors::dark::kAccentError)
                                 .arg(fonts::kFontSizeCaption));
  errorLabel_->hide();
  statusCardLayout->addWidget(errorLabel_);

  statusCard_->setMinimumHeight(120);
  mainLayout->addWidget(statusCard_);

  // === Connect Button ===
  connectButton_ = new QPushButton("Connect", this);
  connectButton_->setMinimumHeight(56);
  connectButton_->setCursor(Qt::PointingHandCursor);
  connect(connectButton_, &QPushButton::clicked, this, &ConnectionWidget::onConnectClicked);
  mainLayout->addWidget(connectButton_);

  // === Session Info Card ===
  auto* sessionGroup = new QGroupBox("Session Info", this);
  auto* sessionLayout = new QVBoxLayout(sessionGroup);
  sessionLayout->setSpacing(12);

  // Session ID row
  auto* sessionIdRow = new QHBoxLayout();
  auto* sessionIdLabelTitle = new QLabel("Session ID", sessionGroup);
  sessionIdLabelTitle->setProperty("textStyle", "secondary");
  sessionIdRow->addWidget(sessionIdLabelTitle);
  sessionIdRow->addStretch();
  sessionIdLabel_ = new QLabel("\u2014", sessionGroup);  // em-dash
  sessionIdLabel_->setProperty("textStyle", "mono");
  sessionIdRow->addWidget(sessionIdLabel_);
  sessionLayout->addLayout(sessionIdRow);

  // Separator
  auto* sep1 = new QFrame(sessionGroup);
  sep1->setFrameShape(QFrame::HLine);
  sep1->setStyleSheet("background-color: rgba(255, 255, 255, 0.05);");
  sep1->setFixedHeight(1);
  sessionLayout->addWidget(sep1);

  // Server row
  auto* serverRow = new QHBoxLayout();
  auto* serverLabelTitle = new QLabel("Server", sessionGroup);
  serverLabelTitle->setProperty("textStyle", "secondary");
  serverRow->addWidget(serverLabelTitle);
  serverRow->addStretch();
  serverLabel_ = new QLabel("vpn.example.com:4433", sessionGroup);
  serverLabel_->setProperty("textStyle", "mono");
  serverRow->addWidget(serverLabel_);
  sessionLayout->addLayout(serverRow);

  // Separator
  auto* sep2 = new QFrame(sessionGroup);
  sep2->setFrameShape(QFrame::HLine);
  sep2->setStyleSheet("background-color: rgba(255, 255, 255, 0.05);");
  sep2->setFixedHeight(1);
  sessionLayout->addWidget(sep2);

  // Latency row
  auto* latencyRow = new QHBoxLayout();
  auto* latencyLabelTitle = new QLabel("Latency", sessionGroup);
  latencyLabelTitle->setProperty("textStyle", "secondary");
  latencyRow->addWidget(latencyLabelTitle);
  latencyRow->addStretch();
  latencyLabel_ = new QLabel("\u2014", sessionGroup);
  latencyRow->addWidget(latencyLabel_);
  sessionLayout->addLayout(latencyRow);

  // Separator
  auto* sep3 = new QFrame(sessionGroup);
  sep3->setFrameShape(QFrame::HLine);
  sep3->setStyleSheet("background-color: rgba(255, 255, 255, 0.05);");
  sep3->setFixedHeight(1);
  sessionLayout->addWidget(sep3);

  // Throughput row
  auto* throughputRow = new QHBoxLayout();
  auto* throughputLabelTitle = new QLabel("TX / RX", sessionGroup);
  throughputLabelTitle->setProperty("textStyle", "secondary");
  throughputRow->addWidget(throughputLabelTitle);
  throughputRow->addStretch();
  throughputLabel_ = new QLabel("0 KB/s / 0 KB/s", sessionGroup);
  throughputRow->addWidget(throughputLabel_);
  sessionLayout->addLayout(throughputRow);

  // Separator
  auto* sep4 = new QFrame(sessionGroup);
  sep4->setFrameShape(QFrame::HLine);
  sep4->setStyleSheet("background-color: rgba(255, 255, 255, 0.05);");
  sep4->setFixedHeight(1);
  sessionLayout->addWidget(sep4);

  // Uptime row
  auto* uptimeRow = new QHBoxLayout();
  auto* uptimeLabelTitle = new QLabel("Uptime", sessionGroup);
  uptimeLabelTitle->setProperty("textStyle", "secondary");
  uptimeRow->addWidget(uptimeLabelTitle);
  uptimeRow->addStretch();
  uptimeLabel_ = new QLabel("\u2014", sessionGroup);
  uptimeRow->addWidget(uptimeLabel_);
  sessionLayout->addLayout(uptimeRow);

  sessionInfoGroup_ = sessionGroup;
  mainLayout->addWidget(sessionGroup);

  // Spacer
  mainLayout->addStretch();

  // === Footer Buttons ===
  auto* footerLayout = new QHBoxLayout();
  footerLayout->setSpacing(12);

  settingsButton_ = new QPushButton("Settings", this);
  settingsButton_->setProperty("buttonStyle", "secondary");
  settingsButton_->setCursor(Qt::PointingHandCursor);
  settingsButton_->setStyleSheet(R"(
    QPushButton {
      background: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      padding: 14px 24px;
    }
    QPushButton:hover {
      background: #2e3440;
      border-color: rgba(255, 255, 255, 0.2);
    }
  )");
  connect(settingsButton_, &QPushButton::clicked, this, &ConnectionWidget::settingsRequested);
  footerLayout->addWidget(settingsButton_);

  mainLayout->addLayout(footerLayout);
}

void ConnectionWidget::setupAnimations() {
  // Setup pulse animation timer for connecting state
  pulseTimer_ = new QTimer(this);
  pulseTimer_->setInterval(750);  // 1.5s cycle = 750ms per half
  connect(pulseTimer_, &QTimer::timeout, this, &ConnectionWidget::onPulseAnimation);

  // Setup uptime timer
  uptimeTimer_ = new QTimer(this);
  uptimeTimer_->setInterval(1000);  // Update every second
  connect(uptimeTimer_, &QTimer::timeout, this, &ConnectionWidget::onUptimeUpdate);

  // Setup opacity effect for status indicator
  statusOpacity_ = new QGraphicsOpacityEffect(statusIndicator_);
  statusOpacity_->setOpacity(1.0);
  statusIndicator_->setGraphicsEffect(statusOpacity_);
}

void ConnectionWidget::onConnectClicked() {
  if (state_ == ConnectionState::kConnected ||
      state_ == ConnectionState::kConnecting ||
      state_ == ConnectionState::kReconnecting) {
    // Disconnect
    emit disconnectRequested();

    // For demo purposes, immediately update state
    setConnectionState(ConnectionState::kDisconnected);
  } else {
    // Connect
    emit connectRequested();

    // For demo purposes, simulate connection flow
    setConnectionState(ConnectionState::kConnecting);

    // Simulate successful connection after 2 seconds (for demo)
    QTimer::singleShot(2000, this, [this]() {
      if (state_ == ConnectionState::kConnecting) {
        setConnectionState(ConnectionState::kConnected);
        setSessionId("0x9abc123def456789");
      }
    });
  }
}

void ConnectionWidget::setConnectionState(ConnectionState state) {
  state_ = state;

  // Handle state transitions
  if (state == ConnectionState::kConnecting || state == ConnectionState::kReconnecting) {
    startPulseAnimation();
  } else {
    stopPulseAnimation();
  }

  if (state == ConnectionState::kConnected) {
    uptimeCounter_.start();
    uptimeTimer_->start();
  } else {
    uptimeTimer_->stop();
  }

  if (state == ConnectionState::kDisconnected || state == ConnectionState::kError) {
    // Reset metrics
    latencyMs_ = 0;
    txBytes_ = 0;
    rxBytes_ = 0;
    sessionId_.clear();
  }

  updateStatusDisplay();
}

void ConnectionWidget::updateStatusDisplay() {
  QString statusColor = getStatusColor();
  QString statusText = getStatusText();

  // Update status indicator color
  statusIndicator_->setStyleSheet(QString("background-color: %1; border-radius: 8px;").arg(statusColor));

  // Update status label
  statusLabel_->setText(statusText);
  statusLabel_->setStyleSheet(QString("font-size: %1px; font-weight: 600; color: %2;")
                                  .arg(fonts::kFontSizeTitle)
                                  .arg(statusColor));

  // Update connect button
  switch (state_) {
    case ConnectionState::kDisconnected:
    case ConnectionState::kError:
      connectButton_->setText(state_ == ConnectionState::kError ? "Retry" : "Connect");
      connectButton_->setStyleSheet("");  // Use default gradient
      break;
    case ConnectionState::kConnecting:
    case ConnectionState::kReconnecting:
      connectButton_->setText("Cancel");
      connectButton_->setStyleSheet(QString(R"(
        QPushButton {
          background: transparent;
          border: 2px solid %1;
          color: %1;
        }
        QPushButton:hover {
          background: rgba(255, 107, 107, 0.1);
        }
      )").arg(colors::dark::kAccentError));
      break;
    case ConnectionState::kConnected:
      connectButton_->setText("Disconnect");
      connectButton_->setStyleSheet(QString(R"(
        QPushButton {
          background: %1;
        }
        QPushButton:hover {
          background: #ff8080;
        }
        QPushButton:pressed {
          background: #e55656;
        }
      )").arg(colors::dark::kAccentError));
      break;
  }

  // Show/hide error label
  if (state_ == ConnectionState::kError && !errorMessage_.isEmpty()) {
    errorLabel_->setText(errorMessage_);
    errorLabel_->show();
  } else {
    errorLabel_->hide();
  }

  // Update session info display based on state
  bool showMetrics = (state_ == ConnectionState::kConnected);

  if (showMetrics) {
    if (!sessionId_.isEmpty()) {
      // Truncate long session IDs
      QString displayId = sessionId_;
      if (displayId.length() > 16) {
        displayId = displayId.left(14) + "...";
      }
      sessionIdLabel_->setText(displayId);
    }
    // Latency color coding
    QString latencyColor = colors::dark::kTextPrimary;
    if (latencyMs_ > 0) {
      if (latencyMs_ <= 50) {
        latencyColor = colors::dark::kAccentSuccess;
      } else if (latencyMs_ <= 100) {
        latencyColor = colors::dark::kAccentWarning;
      } else {
        latencyColor = colors::dark::kAccentError;
      }
      latencyLabel_->setText(QString("%1 ms").arg(latencyMs_));
      latencyLabel_->setStyleSheet(QString("color: %1;").arg(latencyColor));
    }
    throughputLabel_->setText(QString("%1 / %2").arg(formatBytes(txBytes_), formatBytes(rxBytes_)));
  } else {
    sessionIdLabel_->setText("\u2014");
    latencyLabel_->setText("\u2014");
    latencyLabel_->setStyleSheet("");
    throughputLabel_->setText("0 KB/s / 0 KB/s");
    uptimeLabel_->setText("\u2014");
  }
}

void ConnectionWidget::updateMetrics(int latencyMs, uint64_t txBytesPerSec, uint64_t rxBytesPerSec) {
  latencyMs_ = latencyMs;
  txBytes_ = txBytesPerSec;
  rxBytes_ = rxBytesPerSec;

  if (state_ == ConnectionState::kConnected) {
    updateStatusDisplay();
  }
}

void ConnectionWidget::setSessionId(const QString& sessionId) {
  sessionId_ = sessionId;
  if (state_ == ConnectionState::kConnected) {
    updateStatusDisplay();
  }
}

void ConnectionWidget::setServerAddress(const QString& server, uint16_t port) {
  serverAddress_ = server;
  serverPort_ = port;
  serverLabel_->setText(QString("%1:%2").arg(server).arg(port));
}

void ConnectionWidget::setErrorMessage(const QString& message) {
  errorMessage_ = message;
  if (state_ == ConnectionState::kError) {
    errorLabel_->setText(message);
    errorLabel_->show();
  }
}

void ConnectionWidget::onPulseAnimation() {
  pulseState_ = !pulseState_;
  statusOpacity_->setOpacity(pulseState_ ? 1.0 : 0.5);
}

void ConnectionWidget::onUptimeUpdate() {
  if (state_ == ConnectionState::kConnected && uptimeCounter_.isValid()) {
    int seconds = static_cast<int>(uptimeCounter_.elapsed() / 1000);
    uptimeLabel_->setText(formatUptime(seconds));

    // Demo: Update metrics with simulated values
    updateMetrics(
        25 + QRandomGenerator::global()->bounded(20),  // 25-45ms latency
        1200000 + static_cast<uint64_t>(QRandomGenerator::global()->bounded(800000)),  // 1.2-2.0 MB/s TX
        3400000 + static_cast<uint64_t>(QRandomGenerator::global()->bounded(1000000))  // 3.4-4.4 MB/s RX
    );
  }
}

void ConnectionWidget::startPulseAnimation() {
  pulseTimer_->start();
}

void ConnectionWidget::stopPulseAnimation() {
  pulseTimer_->stop();
  statusOpacity_->setOpacity(1.0);
}

QString ConnectionWidget::formatBytes(uint64_t bytesPerSec) const {
  if (bytesPerSec >= 1073741824) {  // >= 1 GB/s
    return QString("%1 GB/s").arg(static_cast<double>(bytesPerSec) / 1073741824.0, 0, 'f', 1);
  } else if (bytesPerSec >= 1048576) {  // >= 1 MB/s
    return QString("%1 MB/s").arg(static_cast<double>(bytesPerSec) / 1048576.0, 0, 'f', 1);
  } else if (bytesPerSec >= 1024) {  // >= 1 KB/s
    return QString("%1 KB/s").arg(static_cast<double>(bytesPerSec) / 1024.0, 0, 'f', 1);
  } else {
    return QString("%1 B/s").arg(bytesPerSec);
  }
}

QString ConnectionWidget::formatUptime(int seconds) const {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  return QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));
}

QString ConnectionWidget::getStatusColor() const {
  switch (state_) {
    case ConnectionState::kDisconnected:
      return colors::dark::kTextSecondary;
    case ConnectionState::kConnecting:
    case ConnectionState::kReconnecting:
      return colors::dark::kAccentWarning;
    case ConnectionState::kConnected:
      return colors::dark::kAccentSuccess;
    case ConnectionState::kError:
      return colors::dark::kAccentError;
  }
  return colors::dark::kTextSecondary;
}

QString ConnectionWidget::getStatusText() const {
  switch (state_) {
    case ConnectionState::kDisconnected:
      return "Disconnected";
    case ConnectionState::kConnecting:
      return "Connecting...";
    case ConnectionState::kConnected:
      return "Connected";
    case ConnectionState::kReconnecting:
      return QString("Reconnecting... (Attempt %1)").arg(reconnectAttempt_);
    case ConnectionState::kError:
      return "Connection Failed";
  }
  return "Unknown";
}

}  // namespace veil::gui
