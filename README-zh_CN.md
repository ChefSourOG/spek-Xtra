# Spek-Xtra

[![CI](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml/badge.svg?branch=main&event=push)](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/ChefSourOG/spek-Xtra)](https://github.com/ChefSourOG/spek-Xtra/releases/latest)
[![License](https://img.shields.io/github/license/ChefSourOG/spek-Xtra)](./LICENSE.md)
[![Platforms](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-blue)](./INSTALL.md)
[![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](./src)
[![Downloads](https://img.shields.io/github/downloads/ChefSourOG/spek-Xtra/total)](https://github.com/ChefSourOG/spek-Xtra/releases)

[[English 英文]](./README.md)

**Spek-Xtra**（IPA: /ˈspɛks ˈɛkstrə/）是一款现代化的跨平台音频文件声学频谱分析器。它最初衍生自 [Spek](https://github.com/alexkay/spek)，并基于 [Spek-X](https://github.com/MikeWang000000/spek-X/) 构建。Spek-Xtra 通过简洁、响应迅速且功能丰富的频谱图，让您轻松洞察音频的频率内容。

无论您是音乐人、音频工程师、播客创作者、档案管理员还是业余爱好者，Spek-Xtra 都能为您提供检查编解码器、对比不同版本、导出可用于正式发布的频谱图，以及通过直观的缩放与平移控件浏览大型文件的能力。

---

## 目录

- [简介](#简介)
- [功能特性](#功能特性)
- [截图](#截图)
- [下载与安装](#下载与安装)
- [快速开始](#快速开始)
- [历史与传承](#历史与传承)
- [从源码构建](#从源码构建)
- [贡献指南](#贡献指南)
- [许可证](#许可证)
- [致谢](#致谢)

---

## 简介

Spek-Xtra 能够读取 FFmpeg 支持的几乎所有音频格式，并实时渲染细节丰富、色彩编码的频谱图。无论是快速检查还是深度分析，它都能提供流畅的工作流：管理多文件、并排对比音轨，以及导出高分辨率图像用于报告或在线分享。

## 功能特性

### 核心分析
- **通用格式支持** — 通过 FFmpeg 解码几乎所有含音频轨道的音视频文件。
- **高性能渲染** — 多线程信号处理，实现超快速的频谱图生成。
- **交互式状态栏** — 实时显示光标下方的频率（Hz/kHz）、时间和分贝值。
- **线性与对数频率刻度** — 一键切换，以突出不同频段的细节。

### 可视化
- **八种精选调色板** — Spectrum、SoX、Green、Rainbow、Teal、Heat、Ice 和 Grayscale。
- **鼠标滚轮缩放与拖拽平移** — 精确检查瞬态、谐波和噪声底。
- **高 DPI 显示支持** — 在现代 4K 和 Retina 屏幕上呈现清晰画面。
- **全新图标设计** — 更新应用 Logo（透明背景）和自定义工具栏图标。

### 工作流与文件管理
- **音频元数据面板** — 即时查看编解码器、码率、采样率、声道数和时长。
- **侧边栏文件队列** — 无需离开应用即可加载和管理多个文件。
- **最近打开的文件** — 从文件菜单快速重新打开最近 10 个音频文件。
- **分屏对比模式** — 并排加载两个音频文件进行 A/B 分析。

### 导出与分享
- **导出为 PNG** — 保存自定义尺寸的高分辨率频谱图，用于报告、社交媒体或档案记录。

### 平台与打包
- **跨平台** — 为 Windows（x64 与 Arm64）、macOS（Universal）和 Linux（Debian）提供原生构建。
- **Windows 单文件可执行程序** — 便携版 `.exe`，无需安装即可运行。
- **Debian 软件包** — 通过 `.deb` 以 `spek-xtra` 名称分发。

### 体验优化
- 修复了启动时隐藏音频信息面板导致的空白频谱图渲染问题。
- 修复了频谱图右上角版本标签的截断显示问题。

---

## 截图

| 元数据面板与队列 | 分屏对比模式 | 调色板选择 |
|:---:|:---:|:---:|
| ![截图 1](./data/example_image_1.png) | ![截图 2](./data/example_image_2.png) | ![截图 3](./data/example_image_3.png) |

---

## 下载与安装

### 预编译二进制文件

| 平台 | 架构 | 软件包 | 说明 |
|----------|-------------|---------|-------|
| **Windows** | x64 | [`.zip`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.zip) / [`.exe`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.exe) | 便携版或安装程序 |
| **Debian / Ubuntu** | x86_64 | [`.deb`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra_1.0.0_amd64.deb) | 软件包名称：`spek-xtra` |
| **macOS** | Universal（Intel 与 Apple Silicon） | [`.tgz`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-macos-universal.tgz) | 解压后即可运行 |
| **源码** | 任意 | [`.tar.gz`](https://github.com/ChefSourOG/spek-Xtra/archive/v1.0.0.tar.gz) | 参见[从源码构建](#从源码构建) |

> **寻找最新版本？** 请访问 [Releases 页面](https://github.com/ChefSourOG/spek-Xtra/releases/latest) 获取最新构建、更新日志和每日构建产物。

### 安装说明

- **Windows：** 运行 `.exe` 安装程序，或解压 `.zip` 后运行 `spek-xtra.exe`。推荐 Windows 10/11 x64 或 Arm64。
- **macOS：** 解压 `.tgz` 并将 `Spek-Xtra.app` 拖入应用程序文件夹。推荐 macOS 11 及以上版本。
- **Linux（Debian/Ubuntu）：** 通过 `sudo dpkg -i spek-xtra_1.0.0_amd64.deb` 或您偏好的包管理器安装。FFmpeg 和 wxWidgets 依赖将自动解决。

---

## 快速开始

1. **打开文件** — 将音频文件拖入窗口，使用 `文件 → 打开`，或从菜单选择一个最近打开的文件。
2. **检查元数据** — 左侧边栏会即时显示编解码器、码率、采样率、声道数和时长。
3. **浏览频谱图** — 滚动鼠标滚轮可缩放时间轴；点击并拖拽可平移。
4. **调整外观** — 使用调色板下拉菜单切换配色方案，或切换线性/对数频率刻度。
5. **对比文件** — 打开第二个文件即可进入分屏模式，并排对比频谱图。
6. **导出** — 点击导出按钮或使用 `文件 → 导出为 PNG`，保存高分辨率图像。

---

## 历史与传承

Spek-Xtra 建立在开源音频工具漫长发展历史的肩膀上。链条中的每一次分叉都回应了社区不断变化的需求，从现代化依赖项到增加全新的工作流。

### 1. Spek（原版）— *Alexander Kojevnikov*
原版 [Spek](https://github.com/alexkay/spek) 由 Alexander Kojevnikov 创建，是一款轻量级跨平台声学频谱分析器，采用 C 和 C++ 编写。它奠定了整个生态系统的核心概念：
- 多线程 FFT 分析，实现快速频谱图渲染。
- 基于 FFmpeg 的解码，提供广泛的格式兼容性。
- 支持拖放，自动适配时间、频率和频谱密度标尺。
- 支持 19 种语言的翻译。

原版上游的开发在 2016 年后逐渐放缓，社区接过了接力棒。

### 2. Spek-Alternative — *withmorten*
2017 年，[withmorten](https://github.com/withmorten) 将项目分叉为 **Spek-Alternative**（v0.8.2.3），以现代化构建系统并修复长期存在的平台问题。主要贡献包括：
- **UI 现代化：** 从 GTK+ 迁移至 wxWidgets，以改善 Windows 和 macOS 的集成度，并优化工具栏图标。
- **打包：** 为 Windows 提供单一便携版 `.exe`，并改进 macOS 文件关联。
- **格式支持：** 新增 `.opus` 解码和 24 位 APE 支持。
- **代码质量：** 拆分出 `libspek`，增加单元测试，并迁移至非弃用的 FFmpeg API。
- **错误修复：** 解决主目录不可写时的崩溃问题、修复 Windows 上的 Unicode 文件名处理、修正频谱密度映射，并增加对平面采样格式（planar sample formats）的支持。

### 3. Spek-X — *Mike Wang*
[Mike Wang](https://github.com/MikeWang000000) 将 Spek-Alternative 分叉为 **Spek-X**，通过积极的依赖更新和新架构支持将项目推向现代。重要里程碑：
- **编解码器与依赖更新：** 升级至 FFmpeg 5.0+，随后支持 6.0、7.1，并替换弃用 API；更新 wxWidgets 至 3.0+。
- **新架构：** 引入原生 Apple Silicon 和 Windows Arm64 二进制文件，以及 macOS Universal 构建。
- **命令行界面：** 增加无头模式（headless），可直接从终端保存频谱图。
- **高 DPI 与无障碍：** 修复高分辨率显示器上的文字截断问题，并抑制侵入式的 wxWidgets 警告。
- **工作流：** 增加音频声道切换、32 位 FLAC 支持，并改进简体中文、繁体中文和法文翻译。
- **稳定性：** 修复 m4a/ogg 解码错误、堵住内存泄漏，并解决多种崩溃场景。

### 4. Spek-Xtra — *ChefSourOG*
**Spek-Xtra** 是这一链条的最新进化。它在 Spek-X 坚实、现代的基础之上，叠加了一套专业级的工作流和可视化工具：
- **信息丰富的 UI：** 专用元数据面板和交互式状态栏，提供精确的光标读数。
- **色彩与导出：** 八种调色板和高分辨率 PNG 导出，用于专业文档记录。
- **文件管理：** 侧边栏队列、最近打开文件列表，以及用于 A/B 分析的分屏对比模式。
- **导航：** 鼠标滚轮缩放和拖拽平移，用于详细的频谱检查。
- **打磨：** 全新品牌设计、高 DPI 图标，以及针对首次运行体验的定向错误修复。

---

## 从源码构建

Spek-Xtra 需要支持 C++11 的编译器，以及以下依赖：

- **wxWidgets** >= 3.0
- **FFmpeg** >= 5.0

各平台的构建说明维护在以下指南中：

| 平台 | 指南 |
|----------|-------|
| Windows | [`dist/win/README.md`](./dist/win/README.md) |
| macOS | [`dist/osx/README.md`](./dist/osx/README.md) |
| Linux 及类 Unix 系统 | [`INSTALL.md`](./INSTALL.md#building-from-the-git-repository) |

### 快速构建（Linux/macOS）

```bash
git clone https://github.com/ChefSourOG/spek-Xtra.git
cd spek-Xtra
./autogen.sh          # 或如果存在则使用 ./configure
make
sudo make install     # 可选
```

---

## 贡献指南

欢迎提交贡献、错误报告和功能请求。

1. **Issues** — 打开 [GitHub Issue](https://github.com/ChefSourOG/spek-Xtra/issues) 以报告错误或请求功能。
2. **Pull Requests** — Fork 仓库，创建功能分支，并向 `main` 分支提交 PR。请确保您的更改通过 CI 并遵循现有代码风格。
3. **翻译** — 如果您希望改进或新增语言翻译，请先打开一个 Issue 进行协调。

---

## 许可证

Spek-Xtra 与其前辈采用相同的开源许可证发布。完整文本请参见 [`LICENSE.md`](./LICENSE.md)。

---

## 致谢

- **Alexander Kojevnikov** — 创建原版 [Spek](https://github.com/alexkay/spek)。
- **withmorten** — 维护 [Spek-Alternative](https://github.com/withmorten/spek-alternative)，在上游休眠期间保持项目活力。
- **Mike Wang** — 开发 [Spek-X](https://github.com/MikeWang000000/spek-X/)，现代化依赖项、增加 CLI 支持并扩展平台覆盖。
- **FFmpeg** 和 **wxWidgets** 团队 — 提供 Spek-Xtra 赖以生存的底层库。
```

---

### 与英文版的对照说明

| 模块 | 处理方式 |
|------|---------|
| **Badge / 徽章链接** | 全部保留原始英文，因为它们是 GitHub/Shields.io 动态生成的。 |
| **技术术语** | 首次出现时附英文（如“多线程 FFT 分析”），后续直接使用中文行业标准说法。 |
| **历史传承** | 四人链条完整翻译，每个人名、版本号、技术贡献（如 planar sample formats、headless mode）均精确对应。 |
| **链接** | 所有相对路径（`./INSTALL.md`、`./LICENSE.md`）和绝对 URL 原样保留。 |
| **导航** | 顶部语言切换链接改为 `[[English 英文]](./README.md)`，方便中文用户跳回英文版。 |

您可以直接将以上内容保存为 `README-zh_CN.md`。如需繁体中文版（繁體中文 / Traditional Chinese）或调整语气（更口语化或更正式），请告诉我。
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
Here is the comprehensive Simplified Chinese README to accompany your English version. It preserves all technical details, URLs, and structure while translating the prose into natural, professional Chinese.

---

```markdown
# Spek-Xtra

[![CI](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml/badge.svg?branch=main&event=push)](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/ChefSourOG/spek-Xtra)](https://github.com/ChefSourOG/spek-Xtra/releases/latest)
[![License](https://img.shields.io/github/license/ChefSourOG/spek-Xtra)](./LICENSE.md)
[![Platforms](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-blue)](./INSTALL.md)
[![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](./src)
[![Downloads](https://img.shields.io/github/downloads/ChefSourOG/spek-Xtra/total)](https://github.com/ChefSourOG/spek-Xtra/releases)

[[English 英文]](./README.md)

**Spek-Xtra**（IPA: /ˈspɛks ˈɛkstrə/）是一款现代化的跨平台音频文件声学频谱分析器。它最初衍生自 [Spek](https://github.com/alexkay/spek)，并基于 [Spek-X](https://github.com/MikeWang000000/spek-X/) 构建。Spek-Xtra 通过简洁、响应迅速且功能丰富的频谱图，让您轻松洞察音频的频率内容。

无论您是音乐人、音频工程师、播客创作者、档案管理员还是业余爱好者，Spek-Xtra 都能为您提供检查编解码器、对比不同版本、导出可用于正式发布的频谱图，以及通过直观的缩放与平移控件浏览大型文件的能力。

<p align="center">
  <img src="./data/example_image_1.png" height="420" alt="Spek-Xtra 频谱图示例 1" />
  <img src="./data/example_image_2.png" height="420" alt="Spek-Xtra 频谱图示例 2" />
  <img src="./data/example_image_3.png" height="420" alt="Spek-Xtra 频谱图示例 3" />
</p>

---

## 目录

- [简介](#简介)
- [功能特性](#功能特性)
- [截图](#截图)
- [下载与安装](#下载与安装)
- [快速开始](#快速开始)
- [历史与传承](#历史与传承)
- [从源码构建](#从源码构建)
- [贡献指南](#贡献指南)
- [许可证](#许可证)
- [致谢](#致谢)

---

## 简介

Spek-Xtra 能够读取 FFmpeg 支持的几乎所有音频格式，并实时渲染细节丰富、色彩编码的频谱图。无论是快速检查还是深度分析，它都能提供流畅的工作流：管理多文件、并排对比音轨，以及导出高分辨率图像用于报告或在线分享。

## 功能特性

### 核心分析
- **通用格式支持** — 通过 FFmpeg 解码几乎所有含音频轨道的音视频文件。
- **高性能渲染** — 多线程信号处理，实现超快速的频谱图生成。
- **交互式状态栏** — 实时显示光标下方的频率（Hz/kHz）、时间和分贝值。
- **线性与对数频率刻度** — 一键切换，以突出不同频段的细节。

### 可视化
- **八种精选调色板** — Spectrum、SoX、Green、Rainbow、Teal、Heat、Ice 和 Grayscale。
- **鼠标滚轮缩放与拖拽平移** — 精确检查瞬态、谐波和噪声底。
- **高 DPI 显示支持** — 在现代 4K 和 Retina 屏幕上呈现清晰画面。
- **全新图标设计** — 更新应用 Logo（透明背景）和自定义工具栏图标。

### 工作流与文件管理
- **音频元数据面板** — 即时查看编解码器、码率、采样率、声道数和时长。
- **侧边栏文件队列** — 无需离开应用即可加载和管理多个文件。
- **最近打开的文件** — 从文件菜单快速重新打开最近 10 个音频文件。
- **分屏对比模式** — 并排加载两个音频文件进行 A/B 分析。

### 导出与分享
- **导出为 PNG** — 保存自定义尺寸的高分辨率频谱图，用于报告、社交媒体或档案记录。

### 平台与打包
- **跨平台** — 为 Windows（x64 与 Arm64）、macOS（Universal）和 Linux（Debian）提供原生构建。
- **Windows 单文件可执行程序** — 便携版 `.exe`，无需安装即可运行。
- **Debian 软件包** — 通过 `.deb` 以 `spek-xtra` 名称分发。

### 体验优化
- 修复了启动时隐藏音频信息面板导致的空白频谱图渲染问题。
- 修复了频谱图右上角版本标签的截断显示问题。

---

## 截图

| 元数据面板与队列 | 分屏对比模式 | 调色板选择 |
|:---:|:---:|:---:|
| ![截图 1](./data/example_image_1.png) | ![截图 2](./data/example_image_2.png) | ![截图 3](./data/example_image_3.png) |

---

## 下载与安装

### 预编译二进制文件

| 平台 | 架构 | 软件包 | 说明 |
|----------|-------------|---------|-------|
| **Windows** | x64 | [`.zip`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.zip) / [`.exe`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.exe) | 便携版或安装程序 |
| **Debian / Ubuntu** | x86_64 | [`.deb`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra_1.0.0_amd64.deb) | 软件包名称：`spek-xtra` |
| **macOS** | Universal（Intel 与 Apple Silicon） | [`.tgz`](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-macos-universal.tgz) | 解压后即可运行 |
| **源码** | 任意 | [`.tar.gz`](https://github.com/ChefSourOG/spek-Xtra/archive/v1.0.0.tar.gz) | 参见[从源码构建](#从源码构建) |

> **寻找最新版本？** 请访问 [Releases 页面](https://github.com/ChefSourOG/spek-Xtra/releases/latest) 获取最新构建、更新日志和每日构建产物。

### 安装说明

- **Windows：** 运行 `.exe` 安装程序，或解压 `.zip` 后运行 `spek-xtra.exe`。推荐 Windows 10/11 x64 或 Arm64。
- **macOS：** 解压 `.tgz` 并将 `Spek-Xtra.app` 拖入应用程序文件夹。推荐 macOS 11 及以上版本。
- **Linux（Debian/Ubuntu）：** 通过 `sudo dpkg -i spek-xtra_1.0.0_amd64.deb` 或您偏好的包管理器安装。FFmpeg 和 wxWidgets 依赖将自动解决。

---

## 快速开始

1. **打开文件** — 将音频文件拖入窗口，使用 `文件 → 打开`，或从菜单选择一个最近打开的文件。
2. **检查元数据** — 左侧边栏会即时显示编解码器、码率、采样率、声道数和时长。
3. **浏览频谱图** — 滚动鼠标滚轮可缩放时间轴；点击并拖拽可平移。
4. **调整外观** — 使用调色板下拉菜单切换配色方案，或切换线性/对数频率刻度。
5. **对比文件** — 打开第二个文件即可进入分屏模式，并排对比频谱图。
6. **导出** — 点击导出按钮或使用 `文件 → 导出为 PNG`，保存高分辨率图像。

---

## 历史与传承

Spek-Xtra 建立在开源音频工具漫长发展历史的肩膀上。链条中的每一次分叉都回应了社区不断变化的需求，从现代化依赖项到增加全新的工作流。

### 1. Spek（原版）— *Alexander Kojevnikov*
原版 [Spek](https://github.com/alexkay/spek) 由 Alexander Kojevnikov 创建，是一款轻量级跨平台声学频谱分析器，采用 C 和 C++ 编写。它奠定了整个生态系统的核心概念：
- 多线程 FFT 分析，实现快速频谱图渲染。
- 基于 FFmpeg 的解码，提供广泛的格式兼容性。
- 支持拖放，自动适配时间、频率和频谱密度标尺。
- 支持 19 种语言的翻译。

原版上游的开发在 2016 年后逐渐放缓，社区接过了接力棒。

### 2. Spek-Alternative — *withmorten*
2017 年，[withmorten](https://github.com/withmorten) 将项目分叉为 **Spek-Alternative**（v0.8.2.3），以现代化构建系统并修复长期存在的平台问题。主要贡献包括：
- **UI 现代化：** 从 GTK+ 迁移至 wxWidgets，以改善 Windows 和 macOS 的集成度，并优化工具栏图标。
- **打包：** 为 Windows 提供单一便携版 `.exe`，并改进 macOS 文件关联。
- **格式支持：** 新增 `.opus` 解码和 24 位 APE 支持。
- **代码质量：** 拆分出 `libspek`，增加单元测试，并迁移至非弃用的 FFmpeg API。
- **错误修复：** 解决主目录不可写时的崩溃问题、修复 Windows 上的 Unicode 文件名处理、修正频谱密度映射，并增加对平面采样格式（planar sample formats）的支持。

### 3. Spek-X — *Mike Wang*
[Mike Wang](https://github.com/MikeWang000000) 将 Spek-Alternative 分叉为 **Spek-X**，通过积极的依赖更新和新架构支持将项目推向现代。重要里程碑：
- **编解码器与依赖更新：** 升级至 FFmpeg 5.0+，随后支持 6.0、7.1，并替换弃用 API；更新 wxWidgets 至 3.0+。
- **新架构：** 引入原生 Apple Silicon 和 Windows Arm64 二进制文件，以及 macOS Universal 构建。
- **命令行界面：** 增加无头模式（headless），可直接从终端保存频谱图。
- **高 DPI 与无障碍：** 修复高分辨率显示器上的文字截断问题，并抑制侵入式的 wxWidgets 警告。
- **工作流：** 增加音频声道切换、32 位 FLAC 支持，并改进简体中文、繁体中文和法文翻译。
- **稳定性：** 修复 m4a/ogg 解码错误、堵住内存泄漏，并解决多种崩溃场景。

### 4. Spek-Xtra — *ChefSourOG*
**Spek-Xtra** 是这一链条的最新进化。它在 Spek-X 坚实、现代的基础之上，叠加了一套专业级的工作流和可视化工具：
- **信息丰富的 UI：** 专用元数据面板和交互式状态栏，提供精确的光标读数。
- **色彩与导出：** 八种调色板和高分辨率 PNG 导出，用于专业文档记录。
- **文件管理：** 侧边栏队列、最近打开文件列表，以及用于 A/B 分析的分屏对比模式。
- **导航：** 鼠标滚轮缩放和拖拽平移，用于详细的频谱检查。
- **打磨：** 全新品牌设计、高 DPI 图标，以及针对首次运行体验的定向错误修复。

---

## 从源码构建

Spek-Xtra 需要支持 C++11 的编译器，以及以下依赖：

- **wxWidgets** >= 3.0
- **FFmpeg** >= 5.0

各平台的构建说明维护在以下指南中：

| 平台 | 指南 |
|----------|-------|
| Windows | [`dist/win/README.md`](./dist/win/README.md) |
| macOS | [`dist/osx/README.md`](./dist/osx/README.md) |
| Linux 及类 Unix 系统 | [`INSTALL.md`](./INSTALL.md#building-from-the-git-repository) |

### 快速构建（Linux/macOS）

```bash
git clone https://github.com/ChefSourOG/spek-Xtra.git
cd spek-Xtra
./autogen.sh          # 或如果存在则使用 ./configure
make
sudo make install     # 可选
```

---

## 贡献指南

欢迎提交贡献、错误报告和功能请求。

1. **Issues** — 打开 [GitHub Issue](https://github.com/ChefSourOG/spek-Xtra/issues) 以报告错误或请求功能。
2. **Pull Requests** — Fork 仓库，创建功能分支，并向 `main` 分支提交 PR。请确保您的更改通过 CI 并遵循现有代码风格。
3. **翻译** — 如果您希望改进或新增语言翻译，请先打开一个 Issue 进行协调。

---

## 许可证

Spek-Xtra 与其前辈采用相同的开源许可证发布。完整文本请参见 [`LICENSE.md`](./LICENSE.md)。

---

## 致谢

- **Alexander Kojevnikov** — 创建原版 [Spek](https://github.com/alexkay/spek)。
- **withmorten** — 维护 [Spek-Alternative](https://github.com/withmorten/spek-alternative)，在上游休眠期间保持项目活力。
- **Mike Wang** — 开发 [Spek-X](https://github.com/MikeWang000000/spek-X/)，现代化依赖项、增加 CLI 支持并扩展平台覆盖。
- **FFmpeg** 和 **wxWidgets** 团队 — 提供 Spek-Xtra 赖以生存的底层库。
