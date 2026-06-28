# Spek-Xtra

[![CI](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml/badge.svg?branch=main&event=push)](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/ChefSourOG/spek-Xtra)](https://github.com/ChefSourOG/spek-Xtra/releases/latest)
[![License](https://img.shields.io/github/license/ChefSourOG/spek-Xtra)](./LICENSE.md)
[![Platforms](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-blue)](./INSTALL-zh_CN.md)
[![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](./src)
[![Downloads](https://img.shields.io/github/downloads/ChefSourOG/spek-Xtra/total)](https://github.com/ChefSourOG/spek-Xtra/releases)

[[英文 English]](./README.md)

**Spek-Xtra** 是一款现代的跨平台声学频谱分析器，用于可视化音频文件的频率内容。它最初源自 [Spek](https://github.com/alexkay/spek)，并在 [Spek-X](https://github.com/MikeWang000000/spek-X/) 的基础上增加了丰富的功能与体验改进。

无论你是音乐人、音频工程师、播客创作者还是爱好者，Spek-Xtra 都能帮助你快速查看音频的编码信息、对比不同文件、导出高清频谱图，并通过直观的缩放和平移浏览大型音频文件。

## Spek-Xtra 能做什么

Spek-Xtra 使用 FFmpeg 读取几乎所有常见音频格式，并渲染出细节丰富、色彩分明的频谱图。你可以：

- 一目了然地查看音频元数据（编码格式、比特率、采样率、声道数、时长）。

- 在八种配色方案之间切换，以适应个人习惯或工作流。
- 使用鼠标滚轮缩放、拖拽平移频谱图。
- 在分屏对比模式下同时查看两个音频文件。
- 将高清频谱图导出为 PNG 图片。
- 通过侧边栏文件队列和最近文件列表管理多个文件。
- 在线性与对数频率刻度之间切换。

## Spek-Xtra 1.0.0 新增功能与改进

Spek-Xtra 1.0.0 带来了大量新功能和品质提升：

- **音频文件信息面板** — 显示编码格式、比特率、采样率、声道数和时长。
- **八种频谱配色方案** — Spectrum、SoX、Green、Rainbow、Teal、Heat、Ice、Grayscale。
- **导出频谱图为 PNG** — 支持自定义分辨率，方便报告和分享。
- **最近文件** — 快速重新打开最近 10 个音频文件。
- **侧边栏文件队列** — 无需离开应用即可加载和管理多个文件。
- **分屏对比模式** — 并排查看两个频谱图。
- **鼠标滚轮缩放与拖拽平移** — 精确检查细节。
- **交互式状态栏** — 实时显示鼠标位置对应的频率、时间和分贝值。
- **线性与对数频率刻度切换**。
- **重新设计的应用图标和工具栏图标**，支持高 DPI。
- **Debian 软件包重命名**为 `spek-xtra`。
- 修复启动时隐藏音频信息面板导致频谱图为空的问题。
- 修复频谱图右上角版本文字被截断的问题。

[详见完整更新日志](#新增功能和改进)。

## Spek-Xtra 1.0.0 - 2026/6/20

### 源码与安装包

类别                                 | 下载链接
-------------------------------------|----------------
Windows (x64) 便携版                 | [spek-xtra-1.0.0-windows-x86_64.zip](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.zip)
Windows (x64) 可执行文件             | [spek-xtra-1.0.0-windows-x86_64.exe](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.exe)
Debian 软件包 (x86_64)               | [spek-xtra_1.0.0_amd64.deb](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra_1.0.0_amd64.deb)
macOS (Universal)                    | [spek-xtra-1.0.0-macos-universal.tgz](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-macos-universal.tgz)
源代码包                             | [v1.0.0.tar.gz](https://github.com/ChefSourOG/spek-Xtra/archive/v1.0.0.tar.gz)

<img src="./data/example_image_1.png" height="500">
<img src="./data/example_image_2.png" height="500">
<img src="./data/example_image_3.png" height="500">

### 新增功能和改进

Spek-Xtra 1.0.0 更新：
 * 新增音频文件信息面板，显示编码格式、比特率、采样率、声道数和时长。
 * 新增频谱配色方案选择，提供八种预设（Spectrum、SoX、Green、Rainbow、Teal、Heat、Ice、Grayscale）。
 * 支持以自定义分辨率将频谱图导出为 PNG。
 * 新增最近文件子菜单，记录最近 10 个打开的音频文件。
 * 新增侧边栏文件队列，支持打开和管理多个音频文件。
 * 新增双音频分屏对比模式。
 * 支持鼠标滚轮缩放和拖拽平移频谱图。
 * 状态栏实时显示鼠标位置对应的频率、时间和分贝值。
 * 新增线性与对数频率刻度切换。
 * 重新设计应用图标，去除绿色背景并支持透明。
 * 工具栏图标更换为精选 PNG 素材，并支持高 DPI。
 * Debian 软件包重命名为 `spek-xtra`。
 * 修复启动时隐藏音频信息面板导致频谱图为空的问题。
 * 修复频谱图右上角版本文字被截断的问题。

Spek-X 1.0.0 更新：
 * FFmpeg 更新至 7.1。
 * 替换了 FFmpeg 不推荐的 API。
 * macOS 程序现在通过 Universal Binary 的形式发布。
 * Windows Arm64 程序现在编译启用了 FFmpeg 的汇编代码优化。

Spek-X 0.9.3 更新：
 * FFmpeg 更新至 6.0。
 * 支持 32-bit FLAC 音频。

Spek-X 0.9.2 更新：
 * 支持切换声道。
 * 移除了 “检查更新” 功能。
 * 修复了潜在的内存泄漏。

Spek-X 0.9.1 更新：
 * 修复了 m4a 和 ogg 的解码问题。
 * 修复了一些崩溃问题。

Spek-X 0.9.0 更新（自 0.8.2 以来）：
 * 适用于 Apple Silicon 的程序现在可用了。
 * 适用于 Windows Arm64 的程序现在可用了。
 * FFmpeg 更新至 5.0+
 * wxWidgets 更新至 3.0+
 * 新增用于保存频谱图的命令行支持。
 * 新增高 DPI 屏幕支持，修复文字显示不全问题。
 * 新增 Windows MSYS2 的构建方法。
 * Windows 上现在使用单一 exe 文件。
 * 优化了简体中文、繁体中文和法语的翻译。
 * 抑制了 wxWidgets 的警告弹窗。
 * 优化了 Linux 上的文件关联。
 * 修复了一些导致崩溃的错误。

### 构建指南

[Windows](./dist/win/README-zh_CN.md) | [macOS](./dist/osx/README-zh_CN.md) | [Linux 和其他类 Unix 系统](./INSTALL-zh_CN.md#Linux-和其他类-Unix-系统)

### 依赖

 * wxWidgets >= 3.0
 * FFmpeg >= 5.0
