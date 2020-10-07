//
// Created by Dmitry Khrykin on 25.05.2020.
//

#ifndef STRATEGR_SGTOOLBAR_H
#define STRATEGR_SGTOOLBAR_H

#import <AppKit/AppKit.h>

class MainWindow;
@interface STGToolbar : NSToolbar <NSToolbarDelegate> {
@public
    MainWindow *qWindow;
}

@property(nonatomic, readonly) unsigned int currentPage;

- (void)setPage:(unsigned int)index;

@end


#endif//STRATEGR_SGTOOLBAR_H
