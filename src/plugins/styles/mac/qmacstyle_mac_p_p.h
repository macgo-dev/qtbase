/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QMACSTYLE_MAC_P_P_H
#define QMACSTYLE_MAC_P_P_H

#include <Carbon/Carbon.h>
#undef check

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include <QtWidgets/private/qcommonstyle_p.h>
#include "qmacstyle_mac_p.h"
#include <private/qapplication_p.h>
#if QT_CONFIG(combobox)
#include <private/qcombobox_p.h>
#endif
#include <private/qpainter_p.h>
#include <private/qstylehelper_p.h>
#include <qapplication.h>
#include <qbitmap.h>
#if QT_CONFIG(checkbox)
#include <qcheckbox.h>
#endif
#include <qcombobox.h>
#if QT_CONFIG(dialogbuttonbox)
#include <qdialogbuttonbox.h>
#endif
#if QT_CONFIG(dockwidget)
#include <qdockwidget.h>
#endif
#include <qevent.h>
#include <qfocusframe.h>
#include <qformlayout.h>
#if QT_CONFIG(groupbox)
#include <qgroupbox.h>
#endif
#include <qhash.h>
#include <qheaderview.h>
#include <qlayout.h>
#if QT_CONFIG(lineedit)
#include <qlineedit.h>
#endif
#if QT_CONFIG(listview)
#include <qlistview.h>
#endif
#if QT_CONFIG(mainwindow)
#include <qmainwindow.h>
#endif
#include <qmap.h>
#if QT_CONFIG(menubar)
#include <qmenubar.h>
#endif
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qpixmapcache.h>
#include <qpointer.h>
#if QT_CONFIG(progressbar)
#include <qprogressbar.h>
#endif
#if QT_CONFIG(pushbutton)
#include <qpushbutton.h>
#endif
#include <qradiobutton.h>
#if QT_CONFIG(rubberband)
#include <qrubberband.h>
#endif
#if QT_CONFIG(sizegrip)
#include <qsizegrip.h>
#endif
#if QT_CONFIG(spinbox)
#include <qspinbox.h>
#endif
#if QT_CONFIG(splitter)
#include <qsplitter.h>
#endif
#include <qstyleoption.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qtoolbar.h>
#if QT_CONFIG(toolbutton)
#include <qtoolbutton.h>
#endif
#if QT_CONFIG(treeview)
#include <qtreeview.h>
#endif
#if QT_CONFIG(tableview)
#include <qtableview.h>
#endif
#include <qdebug.h>
#if QT_CONFIG(datetimeedit)
#include <qdatetimeedit.h>
#endif
#include <qmath.h>
#include <qpair.h>
#include <qvector.h>
#include <QtWidgets/qgraphicsproxywidget.h>
#include <QtWidgets/qgraphicsview.h>



//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

Q_FORWARD_DECLARE_MUTABLE_CG_TYPE(CGContext);

Q_FORWARD_DECLARE_OBJC_CLASS(NSView);
Q_FORWARD_DECLARE_OBJC_CLASS(NSCell);
Q_FORWARD_DECLARE_OBJC_CLASS(QT_MANGLE_NAMESPACE(NotificationReceiver));

QT_BEGIN_NAMESPACE

/*
    AHIG:
        macOS Human Interface Guidelines
        https://developer.apple.com/macos/human-interface-guidelines/overview/themes/

    Builder:
        Interface Builder in Xcode 8 or later
*/

// this works as long as we have at most 16 different control types
#define CT1(c) CT2(c, c)
#define CT2(c1, c2) ((uint(c1) << 16) | uint(c2))

#define SIZE(large, small, mini) \
    (controlSize == QStyleHelper::SizeLarge ? (large) : controlSize == QStyleHelper::SizeSmall ? (small) : (mini))

// same as return SIZE(...) but optimized
#define return_SIZE(large, small, mini) \
    do { \
        static const int sizes[] = { (large), (small), (mini) }; \
        return sizes[controlSize]; \
    } while (false)

class QMacStylePrivate : public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(QMacStyle)
public:
    enum CocoaControlType {
        Box,          // QGroupBox
        Button_CheckBox,
        Button_Disclosure,  // Disclosure triangle, like in QTreeView
        Button_PopupButton,  // Non-editable QComboBox
        Button_PullDown, // QPushButton with menu
        Button_PushButton,
        Button_RadioButton,
        ComboBox,     // Editable QComboBox
        ProgressIndicator_Determinate,
        ProgressIndicator_Indeterminate,
        Scroller_Horizontal,
        Scroller_Vertical,
        Slider_Horizontal,
        Slider_Vertical,
        Stepper       // QSpinBox buttons
    };

    typedef QPair<CocoaControlType, QStyleHelper::WidgetSizePolicy> CocoaControl;

    typedef void (^DrawRectBlock)(CGContextRef, const CGRect &);

    QMacStylePrivate();
    ~QMacStylePrivate();

    // Ideally these wouldn't exist, but since they already exist we need some accessors.
    static const int PushButtonLeftOffset;
    static const int PushButtonTopOffset;
    static const int PushButtonRightOffset;
    static const int PushButtonBottomOffset;
    static const int MiniButtonH;
    static const int SmallButtonH;
    static const int BevelButtonW;
    static const int BevelButtonH;
    static const int PushButtonContentPadding;

    enum Animates { AquaPushButton, AquaProgressBar, AquaListViewItemOpen, AquaScrollBar };
    static ThemeDrawState getDrawState(QStyle::State flags);
    QStyleHelper::WidgetSizePolicy aquaSizeConstrain(const QStyleOption *option, const QWidget *widg,
                             QStyle::ContentsType ct = QStyle::CT_CustomBase,
                             QSize szHint=QSize(-1, -1), QSize *insz = 0) const;
    QStyleHelper::WidgetSizePolicy effectiveAquaSizeConstrain(const QStyleOption *option, const QWidget *widg,
                             QStyle::ContentsType ct = QStyle::CT_CustomBase,
                             QSize szHint=QSize(-1, -1), QSize *insz = 0) const;
    void getSliderInfo(QStyle::ComplexControl cc, const QStyleOptionSlider *slider,
                          HIThemeTrackDrawInfo *tdi, const QWidget *needToRemoveMe) const;
    inline int animateSpeed(Animates) const { return 33; }

    // Utility functions
    void drawColorlessButton(const CGRect &macRect, HIThemeButtonDrawInfo *bdi,
                             const CocoaControl &cw,
                             QPainter *p, const QStyleOption *opt) const;

    QSize pushButtonSizeFromContents(const QStyleOptionButton *btn) const;

    CGRect pushButtonContentBounds(const QStyleOptionButton *btn,
                                   const HIThemeButtonDrawInfo *bdi) const;

    void initComboboxBdi(const QStyleOptionComboBox *combo, HIThemeButtonDrawInfo *bdi,
                         CocoaControl *cw,
                        const QWidget *widget, const ThemeDrawState &tds) const;

    static CGRect comboboxInnerBounds(const CGRect &outerBounds, const CocoaControl &cocoaWidget);

    static QRect comboboxEditBounds(const QRect &outerBounds, const HIThemeButtonDrawInfo &bdi);

    static void drawCombobox(const CGRect &outerBounds, const HIThemeButtonDrawInfo &bdi, const CocoaControl &cw, QPainter *p);
    bool contentFitsInPushButton(const QStyleOptionButton *btn, HIThemeButtonDrawInfo *bdi,
                                 ThemeButtonKind buttonKindToCheck) const;
    void initHIThemePushButton(const QStyleOptionButton *btn, const QWidget *widget,
                               const ThemeDrawState tds,
                               HIThemeButtonDrawInfo *bdi) const;

    void setAutoDefaultButton(QObject *button) const;

    NSView *cocoaControl(CocoaControl widget) const;
    NSCell *cocoaCell(CocoaControl widget) const;

    static CocoaControl cocoaControlFromHIThemeButtonKind(ThemeButtonKind kind);

    void setupNSGraphicsContext(CGContextRef cg, bool flipped) const;
    void restoreNSGraphicsContext(CGContextRef cg) const;

    void setupVerticalInvertedXform(CGContextRef cg, bool reverse, bool vertical, const CGRect &rect) const;

    void drawNSViewInRect(CocoaControl widget, NSView *view, const QRect &rect, QPainter *p, bool isQWidget = true, DrawRectBlock drawRectBlock = nil) const;
    void resolveCurrentNSView(QWindow *window) const;

    void drawFocusRing(QPainter *p, const QRect &targetRect, int hMargin, int vMargin, qreal radius = 0) const;

#if QT_CONFIG(tabbar)
    void tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect, QRect *iconRect) const;
#endif

public:
    mutable QPointer<QObject> autoDefaultButton;
    static  QVector<QPointer<QObject> > scrollBars;

    mutable QPointer<QFocusFrame> focusWidget;
    QT_MANGLE_NAMESPACE(NotificationReceiver) *receiver;
    mutable NSView *backingStoreNSView;
    mutable QHash<CocoaControl, NSView *> cocoaControls;
    mutable QHash<CocoaControl, NSCell *> cocoaCells;

    QFont smallSystemFont;
    QFont miniSystemFont;
};

QT_END_NAMESPACE

#endif // QMACSTYLE_MAC_P_P_H
