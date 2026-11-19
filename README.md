<div align="center">

<img src="icons/hicolor/scalable/apps/wowtype.svg" width="112" alt="WowType 图标">

# WowType 文字版

**面向 Linux 的个人 fork，保留文字输入，精简语音和翻译。**

[本仓库](https://github.com/legnAray/WowType) · [上游项目](https://github.com/Gloridust/WowType)

![fcitx5](https://img.shields.io/badge/fcitx5-%E2%89%A5%205.0-blue)
![License](https://img.shields.io/badge/license-MIT%20%2F%20Apache--2.0-green)

[新机安装](#ubuntu-2404-新机器安装) · [使用](#使用) · [更新](#更新本-fork) · [常见问题](#常见问题) · [开发与文档](#开发与文档)

</div>

本仓库基于 [Gloridust/WowType](https://github.com/Gloridust/WowType) 0.5.1 修改，主要用于 Ubuntu 桌面。
下面的安装流程从 **`legnAray/WowType` 的源码**生成安装包，安装的是本 fork 的文字版。
原项目的一键安装脚本和设置程序的更新源仍指向上游；本 fork 的安装、更新以本页为准。

## 这个 fork 改了什么

- 保留拼音、简拼、双拼、五笔、整句输入、候选排序与学习、自定义词、表情、符号、计算器和主题。
- 移除 Linux 插件与设置界面的语音、翻译、相关快捷键、模型管理、API 配置和 GNOME 语音浮条。
- 默认只构建文字输入核心，Linux 插件不再依赖 ALSA；正常构建无需语音、翻译模型。
- 修复 `- =` 翻页边界：启用这组翻页键时，首页按 `-`、末页按 `=`、只有一页时都保持候选栏，不提交候选或符号。计算器中的减号照常使用。

已在 Ubuntu 22.04、系统 Fcitx5 5.0.14 上验证运行。以下新机安装流程以 **Ubuntu 24.04** 为目标，使用新电脑自己的 Fcitx5 和 GTK。
上游的可选语音实现与 macOS 源码仍保留在仓库中，本页只介绍 Linux 文字版。

<p align="center"><img src="docs/images/candidates.png" width="720" alt="WowType 横排候选栏"><br>
<sub>支持整句输入、半成品拼音、纠错和候选学习</sub></p>

## Ubuntu 24.04 新机器安装

下面所有命令都在要安装的 Ubuntu 24.04 电脑上执行。安装包使用本机的系统库编译，支持 `amd64` 和 `arm64`；生成的包按系统版本和架构区分。
除标有 `sudo` 的安装命令外，其余步骤使用普通用户运行。

### 1. 安装系统依赖

```bash
sudo apt update
sudo apt install build-essential pkg-config cmake extra-cmake-modules \
  git curl xz-utils dpkg-dev \
  libfcitx5core-dev libfcitx5config-dev libfcitx5utils-dev \
  fcitx5 fcitx5-frontend-all fcitx5-config-qt im-config \
  python3 python3-gi gir1.2-gtk-4.0 gir1.2-adw-1 \
  fonts-noto-color-emoji
```

这些依赖包括 C++ 构建工具、Fcitx5 开发文件、GTK/Qt 输入前端和设置界面运行环境。
Ubuntu 软件源中的包名可查阅 [Fcitx5 开发包](https://packages.ubuntu.com/noble/libfcitx5core-dev)与[输入前端](https://packages.ubuntu.com/noble/fcitx5-frontend-all)。

### 2. 安装 Rust

已有可用的 Rust 稳定版工具链时可以跳过。新机器按 [Rust 官方方式](https://rust-lang.org/tools/install/)安装：

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs |
  sh -s -- -y --profile minimal
. "$HOME/.cargo/env"

rustc --version
cargo --version
```

### 3. 获取本 fork 和完整词库

```bash
git clone --branch main --single-branch https://github.com/legnAray/WowType.git
cd WowType

scripts/make-data.sh "" dist/data
scripts/make-wubi.sh "" dist/data
```

两个数据脚本都需要执行：打包需要完整词库、英文词表、词级语言模型、五笔码表及许可证；首次运行需要联网。
整句增强模型下载成功时也会一起打包。`words.lm`、`nnlm.bin` 用于文字候选排序，与已移除的语音和翻译功能无关。

数据脚本继续使用开源的雾凇拼音、rime-wubi 和上游发布的语言模型；模型与五笔源文件会校验固定 SHA256。

### 4. 构建并安装 `.deb`

在刚才的 `WowType` 仓库目录执行：

```bash
cargo test --locked --no-default-features -p wowtype-core

version=$(sed -n 's/^version = "\(.*\)"/\1/p' core/Cargo.toml)
scripts/package-deb.sh engine "$version" dist/pkg

sudo apt install "./dist/pkg/wowtype_${version}_ubuntu24.04_$(dpkg --print-architecture).deb"
```

打包脚本会编译 Rust 核心和 Fcitx5 插件，并把设置界面、自检工具、词库一起装进一个 `.deb`。
当前默认构建已经关闭语音和翻译，无需额外添加关闭参数，也无需安装 ALSA 开发包。

### 5. 启用 Fcitx5 并重新登录

```bash
im-config -n fcitx5
```

执行后**注销桌面并重新登录**，让输入法框架和应用使用新的会话配置。

登录后运行：

```bash
wowtype-config --setup
fcitx5-configtool
```

`--setup` 会把 WowType 加入输入法列表。也可以在「Fcitx 5 配置」中取消勾选「仅显示当前语言」，搜索并添加 WowType。

### 6. 配置 `Ctrl+Space` 和更新来源

在 **Fcitx 5 配置**中：

1. 输入法列表保留你的键盘布局和 **WowType**。如果有系统「拼音（pinyin）」，将它从启用列表移除；无需卸载软件包。
2. 在「全局选项」中关闭重复按触发键轮换输入法的选项，对应配置键 `EnumerateWithTriggerKeys=False`。
3. 从输入法菜单选择 WowType，再测试 `Ctrl+Space`，应只在英文键盘和 WowType 之间切换。

然后打开本 fork 的设置：

```bash
wowtype-config --page update
```

**关闭「自动检查更新」。当前更新页仍连接原作者的仓库，界面的「下载并安装」获取的是上游版本。**
本 fork 后续使用下方的[源码更新流程](#更新本-fork)，保留这里的定制修改。

### 7. 确认安装结果

- 打开「WowType 设置」，应只有 **常规、输入、表情、自定义词、更新**五个页面。
- 切到 WowType，输入 `nihao`；首页连续按 `-` 应保持候选栏，按空格后正常上屏。
- 按 `Ctrl+Space` 往返切换，确认没有再进入系统「拼音」。

还可以运行自检：

```bash
wowtype-config --self-test
wowtype-check --lexicon /usr/share/wowtype/lexicon.tsv
wowtype-show --lexicon /usr/share/wowtype/lexicon.tsv nihao
```

查看当前实际启用的输入法，在使用 WowType 的终端中执行：

```bash
gdbus call --session --dest org.fcitx.Fcitx5 \
  --object-path /controller \
  --method org.fcitx.Fcitx.Controller1.CurrentInputMethod
```

预期返回 `('wowtype',)`；返回 `('pinyin',)` 表示当前仍在使用系统拼音。

### 已有安装包时

同为 **Ubuntu 24.04、同 CPU 架构**的其他电脑可以复用生成的 `.deb`，无需每台电脑安装 Rust 和编译工具。
先安装运行依赖：

```bash
sudo apt update
sudo apt install fcitx5 fcitx5-frontend-all fcitx5-config-qt im-config \
  fonts-noto-color-emoji
```

再用 `sudo apt install ./实际文件名.deb` 安装复制过来的包；GTK 等程序依赖由 apt 补齐。
随后执行上面的第 5–7 步，完成登录会话、输入法列表和更新来源设置。
安装包需在与目标电脑相同的系统版本和架构上构建。

### Ubuntu 22.04 的差异

其余构建方法相同，调整两处：

- 第 1 步的 `fcitx5-frontend-all` 改为 `fcitx5-frontend-gtk3 fcitx5-frontend-gtk4 fcitx5-frontend-qt5`。
- 打包脚本自动生成带 `ubuntu22.04` 的文件名，安装命令中的 `ubuntu24.04` 也相应改成 `ubuntu22.04`。

Ubuntu 22.04 的 GNOME 42 无法使用要求 GNOME 45+ 的配色扩展，文字输入和 Fcitx5 经典界面主题仍可使用。

## 使用

| 按键 | 作用 |
|---|---|
| `Ctrl+Space` | 按上述配置切换英文键盘和 WowType |
| `Shift` | 在 WowType 内部切换中英文，具体按键可在设置中调整 |
| 空格、`1`–`9` | 选择当前候选或对应编号的候选 |
| `←` `→` | 默认横排时移动候选光标 |
| `↓` | 默认横排时展开候选面板 |
| `-` `=`、`PgUp` `PgDn` | 翻页；启用的 `- =` 在边界或只有一页时保持候选栏 |
| 回车 | 上屏原始字母 |
| `Esc` | 清空当前输入 |
| 退格 | 先撤销已选的词，再删除拼音字母 |
| `Ctrl+Delete` | 忘掉光标所在候选的学习记录 |
| `Super+.` | 打开表情与符号面板 |
| `v` + 空格 | 打开工具栏 |

常用输入示例：`zhongg` → 中国、`bjdx` → 北京大学、`riqi` → 日期、`v1+2*3` → 7。
双拼方案中计算器入口使用大写 `V`。

设置通过应用菜单的「WowType 设置」或 `wowtype-config` 打开。
自定义词和学习记录保存在用户目录；普通的 `.deb` 更新会保留这些数据。

## 更新本 fork

在克隆的仓库目录中执行：

```bash
git pull --ff-only origin main

cargo test --locked --no-default-features -p wowtype-core
version=$(sed -n 's/^version = "\(.*\)"/\1/p' core/Cargo.toml)
scripts/package-deb.sh engine "$version" dist/pkg

sudo apt install --reinstall "./dist/pkg/wowtype_${version}_ubuntu24.04_$(dpkg --print-architecture).deb"
wowtype-config --restart-fcitx
```

数据脚本有更新、需要刷新词库或 `dist/data` 不存在时，先重新执行第 3 步的两个数据脚本，再打包。
源码变化但版本号相同时，`--reinstall` 仍会替换已安装程序。
重启输入法后，部分 Chromium / Electron 应用可能需要完全退出并重新打开；也可以注销后重新登录。

`wowtype-update-local` 是维护者现有电脑上的辅助脚本，**没有随仓库分发**，新电脑使用本页的打包流程。
仓库保留的 `install.sh` 默认下载上游安装包，本 fork 的安装与升级使用上述命令。

## 常见问题

| 现象 | 处理 |
|---|---|
| `Ctrl+Space` 又切到系统拼音 | 在 Fcitx5 启用列表移除 `pinyin`，保留键盘和 WowType，并关闭 `EnumerateWithTriggerKeys` |
| 首页按 `-` 仍提交候选 | 用上面的 D-Bus 命令确认当前输入法是 `wowtype`；确认已安装本 fork 并重启输入法 |
| 输入法列表里找不到 WowType | 取消勾选「仅显示当前语言」；确认 `/usr/share/fcitx5/inputmethod/wowtype.conf` 存在；注销后重新登录 |
| 候选很少、常用词缺失 | 检查 `/usr/share/wowtype/lexicon.tsv`；重新生成 `dist/data`，重新打包并安装 |
| 某个应用无法输入中文 | 先确认已执行 `im-config -n fcitx5` 并重新登录；检查前端依赖；输入法重启后重新打开该应用 |
| 「WowType 设置」打不开 | 在终端执行 `wowtype-config` 查看错误；确认 `python3-gi`、`gir1.2-gtk-4.0`、`gir1.2-adw-1` 已安装 |
| emoji 显示为方块 | 安装 `fonts-noto-color-emoji` |
| GNOME Wayland 中系统搜索框等位置没有候选栏 | 按 [Fcitx 官方 Wayland 指南](https://fcitx-im.org/wiki/Using_Fcitx_5_on_Wayland/en)配置；GNOME 可使用 [Input Method Panel](https://extensions.gnome.org/extension/261/kimpanel/) 扩展 |

排查运行环境和导出日志：

```bash
fcitx5-diagnose
wowtype-config --export-log
```

按本页安装的程序可用 `sudo apt remove wowtype` 卸载。个人配置和词库保留在 `~/.config/fcitx5/`、`~/.local/share/wowtype/`。

## 开发与文档

文字版核心和 ABI 检查：

```bash
cargo build --release --locked --no-default-features -p wowtype-core
cargo test --locked --no-default-features -p wowtype-core
scripts/abi-smoke.sh
python3 scripts/test-config-contract.py
```

插件按键检查使用独立输入法实例：

```bash
WOWTYPE_LM="$PWD/dist/data/words.lm" \
  scripts/addon-test.sh "$PWD/dist/data/lexicon.tsv" "$PWD/dist/data/english.txt"
```

- 本 fork 的实现范围与兼容接口：[docs/LOCAL-TEXT-ONLY.md](docs/LOCAL-TEXT-ONLY.md)。其中的本机更新脚本说明只适用于维护者现有电脑。
- 引擎设计：[docs/ENGINE.md](docs/ENGINE.md)。
- 数据来源和格式：[docs/DATA.md](docs/DATA.md)。
- 设置项与数据文件位置：[docs/CONFIG.md](docs/CONFIG.md)。
- 更多输入示例：[docs/USAGE.md](docs/USAGE.md)。

部分详细文档、安装脚本与发布配置仍沿用上游内容，其中的语音、翻译及上游二进制安装说明不代表本 Linux fork 的功能；新机安装和更新使用本 README 的流程。

## 致谢

- [WowType 上游项目](https://github.com/Gloridust/WowType)：本 fork 的代码、引擎与界面基础。
- [雾凇拼音 rime-ice](https://github.com/iDvel/rime-ice)：词库和英文词表（GPL-3.0，随安装包一起分发，许可证见 `/usr/share/doc/wowtype/copyright`）
- [Rime 五笔字型 rime-wubi](https://github.com/rime/rime-wubi)：五笔 86 码表（LGPL-3.0，随安装包一起分发；作者见 `WUBI-AUTHORS`，来源和钉死的提交见 `WUBI-SOURCE.txt`）
- [Tatoeba](https://tatoeba.org)：中文句子统计出的词频（`data/tatoeba-freq.tsv`，只有词频、不含句子），融进安装包的词库；句子许可证 CC BY 2.0 FR，Tatoeba contributors
- [万象拼音 rime_wanxiang](https://github.com/amzxyz/rime_wanxiang)：可选词库（CC BY 4.0，不在安装包里，在设置里下载）
- [OpenCC](https://github.com/BYVoid/OpenCC)：简繁对照表（Apache-2.0）
- [Unicode CLDR](https://cldr.unicode.org/)：emoji 的中英文关键词（Unicode License v3）
- [fcitx5](https://github.com/fcitx/fcitx5)：输入法框架
- [voicekey](https://github.com/J3n5en/voicekey)：上游保留的可选语音代码；本 Linux 文字版不构建该功能
- 微信输入法：体验和设计上的参照

## 许可证

代码按 MIT 或 Apache-2.0 双许可证发布，任选其一。安装包里的词库数据遵循 rime-ice 的 GPL-3.0（词频另融进了 Tatoeba 句子统计出的词频，Tatoeba 句子是 CC BY 2.0 FR，署名见 DATA-SOURCE.txt），五笔码表遵循 rime-wubi 的 LGPL-3.0，可选的万象词库包遵循 CC BY 4.0，词级语言模型 `words.lm` 和整句增强模型 `nnlm.bin`（都用 LCCC 和中文维基训练）是 CC BY-SA 4.0，编进程序里的 emoji 关键词来自 Unicode CLDR（Unicode License v3）。
