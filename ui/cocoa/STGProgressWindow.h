//  Created by Dmitry Khrykin on 8/17/19.
//  Copyright © 2019 Dmitry Khrykin. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface STGProgressWindow : NSWindow
@property(nonatomic, readonly, weak) NSProgressIndicator *progressIndicatior;
@end
