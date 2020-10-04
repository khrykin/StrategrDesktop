//
// Created by Dmitry Khrykin on 2019-08-05.
//

#import <AppKit/AppKit.h>

#include <QLayout>
#include <QMacNativeWidget>
#import <QPushButton>

#include "visualeffectwidget.h"

VisualEffectWidget::VisualEffectWidget(QWidget *parent) : QMacCocoaViewContainer(nil, parent) {
    @autoreleasepool {
        NSView *mainView = [[NSView alloc] init];
        mainView.autoresizesSubviews = YES;

        //        NSVisualEffectView *visualEffectView = [[[NSVisualEffectView alloc] init] autorelease];
        //        visualEffectView.blendingMode = NSVisualEffectBlendingModeBehindWindow;
        //        visualEffectView.autoresizingMask = NSViewWidthSizable |
        //                                            NSViewHeightSizable;
        //
        //        [mainView addSubview:visualEffectView];

        setCocoaView(mainView);
        [mainView release];
    }
}

void VisualEffectWidget::setChildWidget(QWidget *widget) {
    @autoreleasepool {
        //        auto *nativeWidget = new QMacNativeWidget();
        //        auto *layout = new QVBoxLayout();
        //        // Don't use the layout rect calculated from QMacStyle.
        //        widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
        ////        layout->addWidget(widget);
        //        nativeWidget->setStyleSheet("border: 5px solid blue;"
        //                                    "background: red;");
        //        nativeWidget->setLayout(layout);
        //
        ////        // Adjust Cocoa layouts
        //        auto *nativeWidgetView = nativeWidget->nativeView();
        ////        nativeWidgetView.frame = cocoaView().frame;
        ////        nativeWidgetView.autoresizingMask = NSViewWidthSizable |
        ////                                            NSViewHeightSizable;
        ////        nativeWidgetView.autoresizesSubviews = YES;
        ////
        ////        auto *widgetNSView = reinterpret_cast<NSView *>(widget->winId());
        ////        [widgetNSView setAutoresizingMask:NSViewWidthSizable |
        ////                                          NSViewHeightSizable];
        ////
        ////        widgetNSView.frame = cocoaView().frame;
        ////        widgetNSView.autoresizingMask = NSViewWidthSizable |
        ////                                        NSViewHeightSizable;
        //
        ////        NSTextField *nativeWidgetView = [NSTextField textFieldWithString:@"BITCH"];
        //        nativeWidgetView.frame = cocoaView().frame;
        //        nativeWidgetView.autoresizingMask = NSViewWidthSizable |
        //                                            NSViewHeightSizable;
        //
        //        [cocoaView() addSubview:nativeWidgetView];
        //        nativeWidget->show();
        //        widget->show();

        QMacNativeWidget *nativeWidget = new QMacNativeWidget();
        nativeWidget->move(0, 0);
        nativeWidget->setPalette(QPalette(Qt::red));
        nativeWidget->setAutoFillBackground(true);
        QVBoxLayout *layout = new QVBoxLayout();
        QPushButton *pushButton = new QPushButton("An Embedded Qt Button!", nativeWidget);
        pushButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);// Don't use the layout rect calculated from QMacStyle.
        layout->addWidget(pushButton);
        nativeWidget->setLayout(layout);

        // Adjust Cocoa layouts
        NSView *nativeWidgetView = reinterpret_cast<NSView *>(nativeWidget->winId());
        NSView *contentView = cocoaView();
        [contentView setAutoresizesSubviews:YES];
        [nativeWidgetView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [nativeWidgetView setAutoresizesSubviews:YES];
        NSView *pushButtonView = reinterpret_cast<NSView *>(pushButton->winId());
        [pushButtonView setAutoresizingMask:NSViewWidthSizable];

        // Add the nativeWidget to the window.
        [contentView addSubview:nativeWidgetView positioned:NSWindowAbove relativeTo:nil];
        nativeWidget->show();
        pushButton->show();
    }
}
