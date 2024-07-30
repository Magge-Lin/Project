

# 基于redis分布式锁  -- redlock-cpp


> ​	先把遇到的问题、解决办法、redlock-cpp 使用方法写上去
>
> ​	后面再更新一下 redlock-cpp 的学习分析

- 背景

  原本客户端鉴权方案：客户端主机、登录用户、登录用户组、登录用户的控制权限（独占）.

  业主新需求：多客户端可以登录相同用户、且可以共享控制权限.

  某天两人几乎同时（间隔3ms内）对同一辆车操作相同命令，导致服务器处理数据错误.

- 分析

  命令可下发条件在客户端通过远程过程调用获取，workstation 1 和 workstation 2 登录相同用户共享权限，都可以下发，在 workstation 1 下发命令后结果还未传给 workstation 2 时， workstation 2 也下发了命令，导致临界资源的冲突。

  由于以前同时只有一个客户端可以获取控制权限，不会存在多客户端的临界资源竞争。

- 基于当前使用的redis实现分布式锁

  ```
  # redlock-cpp
  bool flag = dlm->Lock("foo", 100000, my_lock);
  bool flag = dlm->ContinueLock("foo", 14000, my_lock);
  
  这里 Lock()、ContinueLock()  两个添加分布式锁的函数，区别在于：
    同一个进程调用 Lock() 后，需要调用 Unlock() 先解锁才能再次加锁.
    而调用 ContinueLock() 后，同一个进程只需要再次调用 ContinueLock() 给当前进程的分布式锁续时间.
  ```

  