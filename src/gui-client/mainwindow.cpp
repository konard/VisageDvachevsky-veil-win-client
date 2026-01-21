#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QApplication>

#include "common/gui/theme.h"
#include "connection_widget.h"
#include "diagnostics_widget.h"
#include "ipc_client_manager.h"
#include "settings_widget.h"

namespace veil::gui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      stackedWidget_(new QStackedWidget(this)),
      connectionWidget_(new ConnectionWidget(this)),
      settingsWidget_(new SettingsWidget(this)),
      diagnosticsWidget_(new DiagnosticsWidget(this)),
      ipcManager_(std::make_unique<IpcClientManager>()) {
  setupUi();
  setupMenuBar();
  setupStatusBar();
  applyDarkTheme();

  // Connect to daemon
  ipcManager_->connectToDaemon();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
  setWindowTitle("VEIL VPN Client");
  setMinimumSize(480, 720);
  resize(480, 720);

  // Set window flags for modern appearance
  setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

  // Add widgets to stacked widget
  stackedWidget_->addWidget(connectionWidget_);
  stackedWidget_->addWidget(settingsWidget_);
  stackedWidget_->addWidget(diagnosticsWidget_);

  // Set central widget
  setCentralWidget(stackedWidget_);

  // Connect signals
  connect(connectionWidget_, &ConnectionWidget::settingsRequested,
          this, &MainWindow::showSettingsView);
  connect(settingsWidget_, &SettingsWidget::backRequested,
          this, &MainWindow::showConnectionView);
  connect(diagnosticsWidget_, &DiagnosticsWidget::backRequested,
          this, &MainWindow::showConnectionView);
}

void MainWindow::setupMenuBar() {
  // Set menu bar style
  menuBar()->setStyleSheet(R"(
    QMenuBar {
      background-color: #1a1d23;
      color: #eceff4;
      border-bottom: 1px solid rgba(255, 255, 255, 0.1);
      padding: 4px 8px;
    }
    QMenuBar::item {
      padding: 6px 12px;
      border-radius: 4px;
      margin: 2px;
    }
    QMenuBar::item:selected {
      background-color: #252932;
    }
    QMenu {
      background-color: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 8px;
      padding: 4px;
    }
    QMenu::item {
      padding: 8px 24px;
      border-radius: 4px;
    }
    QMenu::item:selected {
      background-color: #3aafff;
      color: white;
    }
    QMenu::separator {
      height: 1px;
      background-color: rgba(255, 255, 255, 0.1);
      margin: 4px 8px;
    }
  )");

  auto* viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(tr("&Connection"), this, &MainWindow::showConnectionView);
  viewMenu->addAction(tr("&Settings"), this, &MainWindow::showSettingsView);
  viewMenu->addAction(tr("&Diagnostics"), this, &MainWindow::showDiagnosticsView);

  auto* helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(tr("&About VEIL"), this, &MainWindow::showAboutDialog);
  helpMenu->addAction(tr("Check for &Updates"), []() {
    // TODO: Implement update check
  });
}

void MainWindow::setupStatusBar() {
  statusBar()->setStyleSheet(R"(
    QStatusBar {
      background-color: #1a1d23;
      color: #8fa1b3;
      border-top: 1px solid rgba(255, 255, 255, 0.1);
      padding: 4px 8px;
      font-size: 12px;
    }
    QStatusBar::item {
      border: none;
    }
  )");

  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::applyDarkTheme() {
  // Apply comprehensive dark theme stylesheet from theme.h
  setStyleSheet(getDarkThemeStylesheet());

  // Additional window-specific styles
  QString windowStyle = R"(
    QMainWindow {
      background-color: #1a1d23;
    }
    QStackedWidget {
      background-color: #1a1d23;
    }
  )";

  // Append to existing stylesheet
  setStyleSheet(styleSheet() + windowStyle);
}

void MainWindow::showConnectionView() {
  stackedWidget_->setCurrentWidget(connectionWidget_);
  statusBar()->showMessage(tr("Connection"));
}

void MainWindow::showSettingsView() {
  stackedWidget_->setCurrentWidget(settingsWidget_);
  statusBar()->showMessage(tr("Settings"));
}

void MainWindow::showDiagnosticsView() {
  stackedWidget_->setCurrentWidget(diagnosticsWidget_);
  statusBar()->showMessage(tr("Diagnostics"));
}

void MainWindow::showAboutDialog() {
  // Create a simple about dialog
  auto* dialog = new QDialog(this);
  dialog->setWindowTitle(tr("About VEIL"));
  dialog->setModal(true);
  dialog->setFixedSize(400, 300);

  dialog->setStyleSheet(R"(
    QDialog {
      background-color: #1a1d23;
      color: #eceff4;
    }
    QLabel {
      color: #eceff4;
    }
    QPushButton {
      background: #3aafff;
      border: none;
      border-radius: 8px;
      padding: 10px 24px;
      color: white;
      font-weight: 600;
    }
    QPushButton:hover {
      background: #4abfff;
    }
  )");

  auto* layout = new QVBoxLayout(dialog);
  layout->setSpacing(16);
  layout->setContentsMargins(32, 32, 32, 32);

  auto* titleLabel = new QLabel("VEIL VPN Client", dialog);
  titleLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #3aafff;");
  titleLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(titleLabel);

  auto* versionLabel = new QLabel("Version 0.1.0", dialog);
  versionLabel->setStyleSheet("color: #8fa1b3; font-size: 14px;");
  versionLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(versionLabel);

  auto* descLabel = new QLabel(
      "A secure UDP-based VPN client with DPI evasion capabilities.\n\n"
      "Built with modern cryptography (X25519, ChaCha20-Poly1305)\n"
      "and advanced traffic morphing techniques.",
      dialog);
  descLabel->setWordWrap(true);
  descLabel->setStyleSheet("color: #8fa1b3; font-size: 13px;");
  descLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(descLabel);

  layout->addStretch();

  auto* closeButton = new QPushButton(tr("Close"), dialog);
  connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
  layout->addWidget(closeButton, 0, Qt::AlignCenter);

  dialog->exec();
  dialog->deleteLater();
}

}  // namespace veil::gui
