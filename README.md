# NJUPT2023Apollo2D
> 南京邮电大学Apollo2D-2023校赛选拔题目以及安装配置说明
## 一、题目信息
### 简述
Apollo2D组的Aryeth同学写了一个用于训练点球大战[^1]的1v1 Trainer。请尝试修改球队代码[^2]，实现对keeper守门员和taker罚球人能力的提升。  
[^1]:RoboCup2D仿真中的点球大战与今时的形式有所出入。  
[^2]:Helios-base是半官方性质的公开可供参考代码。  
### 编写要求
当前的仓库中有三个文件夹：
|文件夹|描述|
|:-|:-|
|penalty-trainer|Trainer的源码|
|script|运行所需脚本文件|
|your-team|需要修改的球队代码|

>请仅修改your-team目录中的C++源码，完成题目要求的任务。

#### 一点建议
your-team是以git submodule的方式fork了[HELIOS-BASE](https://github.com/helios-base/helios-base)，同学们可以按需修改.gitmodules文件将其与自己修改后的库链接起来。
## 二、项目安装配置与构建
### 前置配置
请安装并配置Ubuntu虚拟机（建议18以上LTS版本），安装并配置好librcsc、rcssserver、rcssmonitor及其依赖（zlib、boost等），推荐安装doxygen辅助软件，推荐安装vscode用于编码工作，详细细节可以咨询于工（Kawhi Curry）。
### git的使用
项目推荐使用git clone命令部署到本地。默认的.gitignore文件中将your-team设置为untracked模式，将该行注释即可实现本地的commit功能。
>更多git的使用请自行查阅学习。
#### commit指令示例
```sh
git commit -a -m "MEASSAGE"
```
### 编译运行
请在部署好项目后，请在项目主目录内打开终端输入:
```sh
/*提权命令*/
#防止第二行运行时出现Permission Denied！
sudo chmod 777 *.sh
/*第一次部署*/
./init.sh
/*一键编译*/
#请球队代码更改后，运行该行代码完成编译，过程中可能需要输入密码。
./build.sh
```
运行项目需要找到编译后产生的**bin**文件夹，在该文件夹内打开终端运行train.sh脚本。
示例如下：
```sh
#查看帮助
./train.sh -h
#执行N次点球训练（默认皆为HELIOS-BASE）
./train.sh -n N
#设置守门员为your-team,并执行N次
./train.sh -k -n N
#设置罚球人为your-team,并执行N次
./train.sh -t -n N
```
## 查看记录
运行时，在bin目录下会自动生成4个log文件：keeper.log、taker.log、train.log、result.log。
其中train.log和result.log是由Trainer生成的，运行结束也会输出在屏幕上。前者是每次运行的记录，后者是统计结果[^3]。  
[^3]:O表示进球，X表示守门员扑救成功，|表示球出界。
>log每次运行会被覆盖，请及时查看。
