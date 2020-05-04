//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import "SGCalendarImporter.h"
#import "SGCalendarManager.h"

#include "strategy.h"

@interface SGCalendarImporter ()

@property(nonatomic) stg::strategy *strategy;
@property(nonatomic, strong) EKEventStore *eventStore;
@property(nonatomic, strong) SGCalendarManager *calendarManager;

@end

@implementation SGCalendarImporter

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarImporterSettings *)settings {
    if (self = [super init]) {
        self.strategy = reinterpret_cast<stg::strategy *>(strategyPtr);
        self.eventStore = store;
        self.settings = settings;

        self.calendarManager = [[SGCalendarManager alloc] initWithStore:store];
    }

    return self;
}

- (void)import:(SGCalendarImportCompletionHandler)completionHandler {

}

@end