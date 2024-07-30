//
//  LockExample.cpp
//
//  Created by jacketzhong on 14-9-10.
//  Copyright (c) 2014年 jacketzhong. All rights reserved.
//

#include <stdlib.h>
#include <unistd.h>
#include "redlock-cpp/redlock.h"


/*
g++ -o bin/LockExample bin/LockExample.o bin/sds.o bin/redlock.o -L./bin -lredlock -L./hiredis -lhiredis
g++ -o bin/CLockExample bin/CLockExample.o bin/sds.o bin/redlock.o -L./bin -lredlock -L./hiredis -lhiredis
*/
int main (int argc, char **argv) {
    CRedLock * dlm = new CRedLock();
    bool connect = false;
    while (!connect)
    {
        connect |= dlm->AddServerUrl("127.0.0.1", 5005, "123456");
        connect |= dlm->AddServerUrl("127.0.0.1", 5006, "123456");
        connect |= dlm->AddServerUrl("127.0.0.1", 5007, "123456");
        if (!connect)
        {
            // 如果连接失败则1s试一次
            sleep(1);
        }
    } 
    
    // 分布式锁的使用案例
    while (1) {
        CLock my_lock;
        bool flag = dlm->Lock("cmd:trainId-Route", 1000, my_lock);
        if (flag) {
            printf("获取成功, Acquired by client name:%s, res:%s, vttl:%d\n",
                   my_lock.m_val, my_lock.m_resource, my_lock.m_validityTime);
            // do resource job
            try
            {
                if (rand() % 3 != 0)
                {
                    throw std::runtime_error("抛出异常.");
                }
                
                sleep(2);
                dlm->Unlock(my_lock);
            }
            catch(const std::exception& e)
            {
                dlm->Unlock(my_lock);
                std::cerr << e.what() << '\n';
            }
        } else {
            printf("获取失败, lock not acquired, name:%s\n", my_lock.m_val);
            sleep(rand() % 3);
        }
    }

    return 0;
}
