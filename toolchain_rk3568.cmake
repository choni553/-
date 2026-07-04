# RK3568 交叉编译工具链配置
# 使用前请确认交叉编译器路径

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 编译器（Ubuntu 安装: sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu）
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# 如果你的 SDK 提供了 sysroot，取消下面注释并修改路径
# set(CMAKE_SYSROOT /path/to/rk3568/sysroot)
# set(CMAKE_FIND_ROOT_PATH /path/to/rk3568/sysroot)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
