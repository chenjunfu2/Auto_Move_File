# Auto_Move_File
在每次程序启动、从休眠或睡眠中唤醒、在程序已运行时重复启动时，从源目录下自动移动所有文件到目标目录

程序全局只能存在唯一实例

程序启动命令行: \[程序名称\] \[源目录\] \[目标目录\]

程序不会自动添加自身为启动项，可以手动在注册表、启动目录等位置设置程序为启动项，

这样就能在每次系统启动、从休眠或睡眠中唤醒的时候自动移动文件，也可以手动运行程序（此时无需命令行参数）移动一次文件

## 写这个程序的目的
我是用来偷Windows自动更新壁纸的，这样每次开机的时候都会自动帮我从Assets文件夹移动到我长期保存的目录，
实际上这个程序还能做的更好，比如给壁纸加上后缀名、使用拷贝而不是移动（但是不确定拷贝会不会延长壁纸更新周期，
似乎移动走之后确实可以让windows更新快一点？），监控Windows的更新而不是只在每次系统启动时移动文件，改成计划任务或者别的
什么方式而不是一直挂在后台等等，但是我懒，就这样，反正能用，剩下的交给有需要的人自己修改吧，这个项目我就跑路了（大概）
