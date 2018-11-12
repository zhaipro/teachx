#!env bash
# 更新 pacman 的配置，以便使用清华大学开源软件镜像站
echo 'Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/i686' > /etc/pacman.d/mirrorlist.mingw32
echo 'Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/x86_64' > /etc/pacman.d/mirrorlist.mingw64
echo 'Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/msys/$arch' > /etc/pacman.d/mirrorlist.msys
pacman -Sy
# 安装该项目依赖的软件
pacman -S make nasm gcc --noconfirm --needed
# bochs 的安装方式
wget https://downloads.sourceforge.net/project/bochs/bochs/2.6.9/Bochs-2.6.9.exe
echo 已下载好 Bochs 了，请先自行安装
echo 安装在了哪里？
DEFAULT_PATH='/c/Program\ Files\ \(x86\)/Bochs-2.6.9'
echo -n 默认 ${DEFAULT_PATH}:
read
if [ "$REPLY" = '' ]; then
	REPLY=$DEFAULT_PATH
fi;
rm -f Bochs-2.6.9.exe
echo PATH=$PATH:$REPLY >> ~/.bashrc
source ~/.bashrc
