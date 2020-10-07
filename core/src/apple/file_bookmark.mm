//
// Created by Dmitry Khrykin on 26.08.2020.
//

#include "file_bookmark.h"
#include "stgstring.h"

#import <Foundation/Foundation.h>

namespace stg {
    static auto make_bookmark(NSURL *url) -> raw_buffer {
        NSData *urlBookmarkData;

        if ([url startAccessingSecurityScopedResource]) {
            NSError *error;
            urlBookmarkData = [url.standardizedURL bookmarkDataWithOptions:0
                                            includingResourceValuesForKeys:nil
                                                             relativeToURL:nil
                                                                     error:&error];

            if (error) {
                NSLog(@"Can't create bookmark from NSURL with the folowing error:\n"
                      @"\tcode: %ld\n"
                      @"\tdescription:\n\t\t%@",
                      (long) error.code, error.localizedDescription);
            }
        }

        [url stopAccessingSecurityScopedResource];

        return {(uint8_t *) urlBookmarkData.bytes,
                (uint8_t *) urlBookmarkData.bytes + urlBookmarkData.length};
    }

#pragma mark - Construction

    file_bookmark::file_bookmark(NSURL *url)
        : data(make_bookmark(url)) {}

#pragma mark - Conversions

    auto file_bookmark::to_nsurl(bool *is_stale) const -> NSURL * {
        NSData *bookmarkData = [NSData dataWithBytesNoCopy:(void *) data.data()
                                                    length:data.size()
                                              freeWhenDone:NO];

        NSError *error;
        NSURL *url = [NSURL URLByResolvingBookmarkData:bookmarkData
                                               options:0
                                         relativeToURL:nil
                                   bookmarkDataIsStale:(BOOL *) is_stale
                                                 error:&error];

        if (error) {
            NSLog(@"Can't resolve NSURL bookmark with the folowing error:\n\tcode: %ld\n\ndescription: %@",
                  (long) error.code, error.localizedDescription);
        }

        return url;
    }
}
