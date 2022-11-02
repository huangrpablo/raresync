//
// Created by 黄保罗 on 29.10.22.
//

#ifndef RARESYNC_DEFINE_H
#define RARESYNC_DEFINE_H

#include <shared_mutex>

enum rs_errno {
    CONF_ERR = -1,
    GOOD
};

typedef std::lock_guard<std::mutex> auto_lock;
typedef std::shared_lock<std::shared_mutex> read_lock;
typedef std::lock_guard<std::shared_mutex> write_lock;


#endif //RARESYNC_DEFINE_H
