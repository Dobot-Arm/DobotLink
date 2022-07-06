# 说明

- smb2库使用第三方开源库[libsmb2](https://github.com/sahlberg/libsmb2)
- 本人对这个库做了一些改动，添加了 smb2_isconnect_share(...)
- 编译之前，要把CMakeLists.txt里面的2行注释掉
    - find_package(GSSAPI)
    - find_package(OpenSSL)
	
## smb库以动态库的方式编译。

- Windows上以VS2017举例，编译方式如下：
> 1. 打开cmd进入到libsmb2-master目录
> 2. `mkdir build && cd build`
> 3. `cmake -G "Visual Studio 15 2017" ..`
> 4. `cmake --build . --config Release`
- 上面默认生成的是32位的静态库，如果想要64位的，改成：
`cmake -G "Visual Studio 15 2017 Win64" ..`

----------------------------------------------------------------------
- Linux下编译方式如下
> 1. 进入到libsmb2-master目录
> 2. `mkdir build && cd build`
> 3. `cmake -DCMAKE_BUILD_TYPE=Release ..`
> 4. `make`

##### 编译完毕后，将smb2.dll/libsmb2.so 拷贝到exe所在目录。
##### 将include/smb2、include/dllconfig.h、include/libsmb2-private.h文件拷贝到自己的工程中