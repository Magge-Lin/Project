//编译 dataRelaySrs.cpp

mkdir build & cd build

cmake ..

make

// 编译动态库 hook.so

gcc -shared -fPIC -o hook.so hook.c -ldl

//启动 tcp 服务器并监听8888端口
./build/dataRelaySrs 8888

发送数据：hello world，截图打印结果

// 通过 LD_PRELOAD 优先加载hook.so动态库中，捕获系统socket API函数recv、send.
LD_PRELOAD=./hook.so ./build/dataRelaySrs 8888
发送数据：hello world，截图打印结果

比较两次打印结果和通信返回数据，发现 hello world 数据后会被添加 --hookXX 数据.