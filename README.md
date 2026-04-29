# TestPrinter
# USB 小票打印测试工具

这是一个基于 Qt 开发的 USB 小票打印机测试工具，用于读取 CSV 文件并向 ESC/POS 打印机发送打印指令，仅作学习与测试用途。

---

## 功能说明
- 读取 CSV 文件第一列内容
- 发送 ESC/POS 指令控制小票打印机
- 支持中文打印（GB2312 编码）
- 极简界面，专注测试场景

## 开发环境
- Qt 6.x（MSVC2022 64bit）
- Windows 10/11
- 依赖：`QtWidgets`、`QtCore`

## 使用说明
1.  准备 CSV 文件，第一列为需要打印的内容
2.  配置打印机路径
3.  点击按钮读取 CSV 并打印

##
本软件基于 Qt 框架开发，Qt 遵循 LGPL 开源协议。
Qt 官方及源码获取地址：https://www.qt.io
Copyright (C) The Qt Company Ltd.

## 授权协议
本项目基于 **MIT License** 开源，仅供学习与测试使用，不用于任何商业用途。