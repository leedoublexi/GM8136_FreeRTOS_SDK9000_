当前目录以 F:\SDK 为例arm-none-eabi-gcc-4.6.4_host_mingw32\bin
1. 解压 arm-none-eabi-gcc-4.6.4_host_mingw32.7z 到当前目录,  F:\SDK\arm-none-eabi-gcc-4.6.4_host_mingw32
2. 将F:\SDK\arm-none-eabi-gcc-4.6.4_host_mingw32\bin   加到环境变量的 Path
3. 双击F:\SDK\Software\start.bat
4. 编译工程：make -C ./projects/samples/room70
5. 界面图片宽度、高度须为16的倍数

6. 新网卡支持：define.h  的 #define _XTM9000_SUPPORT  //XTM9000网卡支持