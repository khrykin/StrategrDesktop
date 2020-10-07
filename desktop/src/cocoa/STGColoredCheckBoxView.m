//
//  SGColoredCheckBoxView.m
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import "STGColoredCheckBoxView.h"
#import <QuartzCore/QuartzCore.h>

@interface SGInnerShadowLayer : CAShapeLayer <CALayerDelegate>

@end

@implementation SGInnerShadowLayer

- (void)drawInContext:(CGContextRef)ctx {
    CGAffineTransform scale = CGAffineTransformMakeTranslation(0, 0);
    CGPathRef widerPath = CGPathCreateCopyByTransformingPath(self.path, &scale);

    [self drawInnerShadowInContext:ctx
                          withPath:widerPath
                       shadowColor:[NSColor.blackColor colorWithAlphaComponent:0.2].CGColor
                            offset:CGSizeMake(0, -0.5)
                        blurRadius:1.5];

    CGPathRelease(widerPath);
}

- (void)drawInnerShadowInContext:(CGContextRef)context
                        withPath:(CGPathRef)path
                     shadowColor:(CGColorRef)shadowColor
                          offset:(CGSize)offset
                      blurRadius:(CGFloat)blurRadius {
    CGContextSaveGState(context);
    CGContextAddPath(context, path);
    CGContextClip(context);

    CGColorRef opaqueShadowColor = CGColorCreateCopyWithAlpha(shadowColor, 1.0);
    CGContextSetAlpha(context, CGColorGetAlpha(shadowColor));

    CGContextBeginTransparencyLayer(context, NULL);

    CGContextSetShadowWithColor(context, offset, blurRadius, opaqueShadowColor);
    CGContextSetBlendMode(context, kCGBlendModeSourceOut);
    CGContextSetFillColorWithColor(context, opaqueShadowColor);
    CGContextAddPath(context, path);
    CGContextFillPath(context);

    CGContextEndTransparencyLayer(context);

    CGContextRestoreGState(context);
    CGColorRelease(opaqueShadowColor);
}


@end


@interface STGColoredCheckBoxView ()

@property(nonatomic, weak) CALayer *backgroundLayer;
@property(nonatomic, weak) CAShapeLayer *checkLayer;
@property(nonatomic, weak) SGInnerShadowLayer *innerShadowLayer;

@end

@implementation STGColoredCheckBoxView

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        self.state = NSControlStateValueOff;

        CALayer *layer = [[CALayer alloc] init];

        self.layer = layer;
        self.wantsLayer = YES;

        CAShapeLayer *backgroundLayer = [[CAShapeLayer alloc] init];
        self.backgroundLayer = backgroundLayer;

        CGRect backgroundFrame;
        backgroundFrame.size = self.intrinsicContentSize;

        backgroundLayer.cornerRadius = 3;
        backgroundLayer.backgroundColor = NSColor.redColor.CGColor;
        backgroundLayer.borderWidth = 0.5;

        [self.layer addSublayer:backgroundLayer];

        SGInnerShadowLayer *innerShadowLayer = [[SGInnerShadowLayer alloc] init];
        self.innerShadowLayer = innerShadowLayer;

        CGPathRef roundedRectPath = CGPathCreateWithRoundedRect(backgroundFrame, 3, 3, nil);
        innerShadowLayer.path = roundedRectPath;
        CGPathRelease(roundedRectPath);
        innerShadowLayer.fillColor = NSColor.clearColor.CGColor;
        innerShadowLayer.strokeColor = NSColor.clearColor.CGColor;
        innerShadowLayer.cornerRadius = 3;
        innerShadowLayer.masksToBounds = YES;

        [self.layer addSublayer:innerShadowLayer];

        CGMutablePathRef checkPath = CGPathCreateMutable();
        CGPathMoveToPoint(checkPath, nil, 10, 10);
        CGPathAddLineToPoint(checkPath, nil, 5.5, 3.5);
        CGPathAddLineToPoint(checkPath, nil, 3.5, 6);

        CAShapeLayer *checkLayer = [[CAShapeLayer alloc] init];
        self.checkLayer = checkLayer;
        checkLayer.path = checkPath;
        CGPathRelease(checkPath);

        checkLayer.lineCap = kCALineCapRound;
        checkLayer.lineJoin = kCALineJoinRound;
        checkLayer.lineWidth = 1.5;
        checkLayer.fillColor = NSColor.clearColor.CGColor;
        checkLayer.strokeColor = NSColor.whiteColor.CGColor;
        checkLayer.shadowOpacity = 1.0;
        checkLayer.shadowRadius = 0.5;
        checkLayer.shadowOffset = CGSizeMake(0, -1);
        checkLayer.hidden = YES;
        checkLayer.delegate = self;

        [self setColor:NSColor.linkColor];

        [self.layer addSublayer:checkLayer];
    }

    return self;
}

- (BOOL)wantsUpdateLayer {
    return YES;
}

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [self init];

    if (self) {
        self.target = target;
        self.action = action;
    }

    return self;
}

- (void)layout {
    [super layout];

    self.backgroundLayer.frame = self.backgroundFrame;
    self.innerShadowLayer.frame = self.layer.bounds;
    self.innerShadowLayer.path = CGPathCreateWithRoundedRect(self.backgroundFrame, 3, 3, nil);

    self.checkLayer.frame = self.backgroundFrame;

    [self.innerShadowLayer setNeedsDisplay];
}

- (id<CAAction>)actionForLayer:(CALayer *)layer forKey:(NSString *)event {
    if (layer == self.checkLayer) {
        CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:event];
        animation.duration = 0;

        return animation;

    } else
        return nil;
}

- (CGRect)backgroundFrame {
    CGRect backgroundFrame;
    backgroundFrame.size = self.intrinsicContentSize;

    backgroundFrame.origin = CGPointMake(CGRectGetMidX([self bounds]) - backgroundFrame.size.width / 2,
                                         CGRectGetMidY([self bounds]) - backgroundFrame.size.height / 2);

    return backgroundFrame;
}

- (NSSize)intrinsicContentSize {
    return NSMakeSize(14, 14);
}

- (void)setState:(NSControlStateValue)state {
    _state = state;

    self.checkLayer.hidden = _state != NSControlStateValueOn;
}

- (void)mouseDown:(NSEvent *)event {
    self.state = !self.state;

    [NSApplication.sharedApplication sendAction:self.action
                                             to:self.target
                                           from:self];
}

- (void)setColor:(NSColor *)color {
    _color = color;

    self.backgroundLayer.backgroundColor = self.color.CGColor;
    self.backgroundLayer.borderColor = [self.color shadowWithLevel:0.2].CGColor;
    self.checkLayer.shadowColor = [self.color shadowWithLevel:0.3].CGColor;
}

@end
