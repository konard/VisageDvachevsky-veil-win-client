#pragma once

#include <QString>
#include <QColor>

namespace veil::gui {

/// Color palette as defined in client_ui_design.md
namespace colors {
namespace dark {
// Background colors
constexpr const char* kBackgroundPrimary = "#1a1d23";
constexpr const char* kBackgroundSecondary = "#252932";
constexpr const char* kBackgroundTertiary = "#2e3440";
constexpr const char* kBackgroundCard = "rgba(255, 255, 255, 0.05)";

// Text colors
constexpr const char* kTextPrimary = "#eceff4";
constexpr const char* kTextSecondary = "#8fa1b3";
constexpr const char* kTextTertiary = "#5c687a";

// Accent colors
constexpr const char* kAccentPrimary = "#3aafff";
constexpr const char* kAccentSuccess = "#38e2c7";
constexpr const char* kAccentWarning = "#ffb347";
constexpr const char* kAccentError = "#ff6b6b";

// Glassmorphism
constexpr const char* kGlassOverlay = "rgba(255, 255, 255, 0.05)";
constexpr const char* kGlassBorder = "rgba(255, 255, 255, 0.1)";
constexpr const char* kShadow = "rgba(0, 0, 0, 0.3)";
}  // namespace dark

namespace light {
// Background colors
constexpr const char* kBackgroundPrimary = "#f8f9fa";
constexpr const char* kBackgroundSecondary = "#e9ecef";
constexpr const char* kBackgroundTertiary = "#ffffff";

// Text colors
constexpr const char* kTextPrimary = "#212529";
constexpr const char* kTextSecondary = "#6c757d";
constexpr const char* kTextTertiary = "#adb5bd";

// Accent colors
constexpr const char* kAccentPrimary = "#0d6efd";
constexpr const char* kAccentSuccess = "#20c997";
constexpr const char* kAccentWarning = "#fd7e14";
constexpr const char* kAccentError = "#dc3545";
}  // namespace light
}  // namespace colors

/// Font settings
namespace fonts {
constexpr const char* kFontFamily = "-apple-system, BlinkMacSystemFont, 'Inter', 'Segoe UI', system-ui, sans-serif";
constexpr const char* kFontFamilyMono = "'JetBrains Mono', 'Consolas', 'Monaco', monospace";

constexpr int kFontSizeHeadline = 28;
constexpr int kFontSizeTitle = 20;
constexpr int kFontSizeBody = 15;
constexpr int kFontSizeCaption = 13;
constexpr int kFontSizeMono = 13;
}  // namespace fonts

/// Animation durations in milliseconds
namespace animations {
constexpr int kDurationFast = 150;
constexpr int kDurationNormal = 200;
constexpr int kDurationSlow = 300;
constexpr int kDurationPulse = 1500;
}  // namespace animations

/// Spacing and sizing
namespace spacing {
constexpr int kPaddingSmall = 8;
constexpr int kPaddingMedium = 16;
constexpr int kPaddingLarge = 24;
constexpr int kPaddingXLarge = 40;

constexpr int kBorderRadiusSmall = 8;
constexpr int kBorderRadiusMedium = 12;
constexpr int kBorderRadiusLarge = 16;
}  // namespace spacing

/// Returns the complete dark theme stylesheet
inline QString getDarkThemeStylesheet() {
  return R"(
    /* === Main Window & Containers === */
    QMainWindow, QWidget {
      background-color: #1a1d23;
      color: #eceff4;
      font-family: -apple-system, BlinkMacSystemFont, 'Inter', 'Segoe UI', system-ui, sans-serif;
      font-size: 15px;
    }

    /* === Menu Bar === */
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

    /* === Primary Buttons (Gradient) === */
    QPushButton {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                  stop:0 #3aafff, stop:1 #38e2c7);
      border: none;
      border-radius: 12px;
      padding: 16px 32px;
      color: white;
      font-size: 16px;
      font-weight: 600;
    }

    QPushButton:hover {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                  stop:0 #4abfff, stop:1 #48f2d7);
    }

    QPushButton:pressed {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                  stop:0 #2a9fef, stop:1 #28d2b7);
    }

    QPushButton:disabled {
      background: #3d4451;
      color: #5c687a;
    }

    /* Secondary Buttons */
    QPushButton[buttonStyle="secondary"] {
      background: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      color: #eceff4;
    }

    QPushButton[buttonStyle="secondary"]:hover {
      background: #2e3440;
      border-color: rgba(255, 255, 255, 0.2);
    }

    /* Danger Buttons */
    QPushButton[buttonStyle="danger"] {
      background: #ff6b6b;
    }

    QPushButton[buttonStyle="danger"]:hover {
      background: #ff8080;
    }

    QPushButton[buttonStyle="danger"]:pressed {
      background: #e55656;
    }

    /* === Group Boxes (Glassmorphism Cards) === */
    QGroupBox {
      background-color: rgba(255, 255, 255, 0.05);
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 16px;
      margin-top: 16px;
      padding: 20px 16px 16px 16px;
      font-size: 15px;
      font-weight: 500;
    }

    QGroupBox::title {
      subcontrol-origin: margin;
      subcontrol-position: top left;
      left: 16px;
      padding: 0 8px;
      color: #8fa1b3;
      font-size: 13px;
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 1px;
    }

    /* === Form Inputs === */
    QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
      background-color: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 8px;
      padding: 10px 12px;
      color: #eceff4;
      font-size: 14px;
      selection-background-color: #3aafff;
    }

    QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
      border-color: #3aafff;
      outline: none;
    }

    QLineEdit:disabled, QSpinBox:disabled, QComboBox:disabled {
      background-color: #1a1d23;
      color: #5c687a;
    }

    QComboBox::drop-down {
      border: none;
      width: 32px;
    }

    QComboBox::down-arrow {
      image: none;
      border-left: 5px solid transparent;
      border-right: 5px solid transparent;
      border-top: 6px solid #8fa1b3;
      margin-right: 8px;
    }

    QComboBox QAbstractItemView {
      background-color: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 8px;
      padding: 4px;
      selection-background-color: #3aafff;
    }

    /* === Checkboxes === */
    QCheckBox {
      spacing: 10px;
      color: #eceff4;
      font-size: 14px;
    }

    QCheckBox::indicator {
      width: 20px;
      height: 20px;
      border: 2px solid rgba(255, 255, 255, 0.2);
      border-radius: 4px;
      background-color: #252932;
    }

    QCheckBox::indicator:checked {
      background-color: #3aafff;
      border-color: #3aafff;
    }

    QCheckBox::indicator:hover {
      border-color: #3aafff;
    }

    /* === Labels === */
    QLabel {
      color: #eceff4;
    }

    QLabel[textStyle="secondary"] {
      color: #8fa1b3;
    }

    QLabel[textStyle="caption"] {
      color: #5c687a;
      font-size: 13px;
    }

    QLabel[textStyle="mono"] {
      font-family: 'JetBrains Mono', 'Consolas', 'Monaco', monospace;
      font-size: 13px;
    }

    /* === Tables === */
    QTableWidget {
      background-color: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 8px;
      gridline-color: rgba(255, 255, 255, 0.05);
    }

    QTableWidget::item {
      padding: 8px;
      color: #eceff4;
    }

    QTableWidget::item:selected {
      background-color: rgba(58, 175, 255, 0.3);
    }

    QHeaderView::section {
      background-color: #1a1d23;
      color: #8fa1b3;
      padding: 12px 8px;
      border: none;
      border-bottom: 1px solid rgba(255, 255, 255, 0.1);
      font-weight: 600;
      text-transform: uppercase;
      font-size: 12px;
      letter-spacing: 1px;
    }

    /* === Text Edit / Log === */
    QTextEdit, QPlainTextEdit {
      background-color: #252932;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 8px;
      padding: 12px;
      color: #eceff4;
      font-family: 'JetBrains Mono', 'Consolas', 'Monaco', monospace;
      font-size: 13px;
      selection-background-color: #3aafff;
    }

    /* === Scroll Bars === */
    QScrollBar:vertical {
      background-color: #1a1d23;
      width: 12px;
      margin: 0;
      border-radius: 6px;
    }

    QScrollBar::handle:vertical {
      background-color: #3d4451;
      border-radius: 6px;
      min-height: 30px;
      margin: 2px;
    }

    QScrollBar::handle:vertical:hover {
      background-color: #4d5461;
    }

    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
      background: none;
      height: 0;
    }

    QScrollBar:horizontal {
      background-color: #1a1d23;
      height: 12px;
      margin: 0;
      border-radius: 6px;
    }

    QScrollBar::handle:horizontal {
      background-color: #3d4451;
      border-radius: 6px;
      min-width: 30px;
      margin: 2px;
    }

    QScrollBar::handle:horizontal:hover {
      background-color: #4d5461;
    }

    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal,
    QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
      background: none;
      width: 0;
    }

    /* === Tooltips === */
    QToolTip {
      background-color: #2e3440;
      color: #eceff4;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 4px;
      padding: 8px 12px;
      font-size: 13px;
    }

    /* === Progress Bar === */
    QProgressBar {
      background-color: #252932;
      border: none;
      border-radius: 4px;
      height: 8px;
      text-align: center;
    }

    QProgressBar::chunk {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                  stop:0 #3aafff, stop:1 #38e2c7);
      border-radius: 4px;
    }

    /* === Sliders === */
    QSlider::groove:horizontal {
      background-color: #252932;
      height: 6px;
      border-radius: 3px;
    }

    QSlider::handle:horizontal {
      background-color: #3aafff;
      width: 18px;
      height: 18px;
      margin: -6px 0;
      border-radius: 9px;
    }

    QSlider::handle:horizontal:hover {
      background-color: #4abfff;
    }

    QSlider::sub-page:horizontal {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                  stop:0 #3aafff, stop:1 #38e2c7);
      border-radius: 3px;
    }

    /* === Tab Widget === */
    QTabWidget::pane {
      background-color: #1a1d23;
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 8px;
      padding: 8px;
    }

    QTabBar::tab {
      background-color: #252932;
      color: #8fa1b3;
      padding: 10px 20px;
      margin-right: 4px;
      border-top-left-radius: 8px;
      border-top-right-radius: 8px;
    }

    QTabBar::tab:selected {
      background-color: #3aafff;
      color: white;
    }

    QTabBar::tab:hover:!selected {
      background-color: #2e3440;
    }
  )";
}

/// Returns the light theme stylesheet
inline QString getLightThemeStylesheet() {
  return R"(
    QMainWindow, QWidget {
      background-color: #f8f9fa;
      color: #212529;
      font-family: -apple-system, BlinkMacSystemFont, 'Inter', 'Segoe UI', system-ui, sans-serif;
      font-size: 15px;
    }

    QMenuBar {
      background-color: #ffffff;
      color: #212529;
      border-bottom: 1px solid #e9ecef;
    }

    QMenuBar::item:selected {
      background-color: #e9ecef;
    }

    QPushButton {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                  stop:0 #0d6efd, stop:1 #20c997);
      border: none;
      border-radius: 12px;
      padding: 16px 32px;
      color: white;
      font-size: 16px;
      font-weight: 600;
    }

    QGroupBox {
      background-color: #ffffff;
      border: 1px solid #e9ecef;
      border-radius: 16px;
    }

    QGroupBox::title {
      color: #6c757d;
    }

    QLineEdit, QSpinBox, QComboBox {
      background-color: #ffffff;
      border: 1px solid #ced4da;
      border-radius: 8px;
      padding: 10px 12px;
      color: #212529;
    }

    QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
      border-color: #0d6efd;
    }
  )";
}

}  // namespace veil::gui
