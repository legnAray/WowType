# Linux 文字输入版

本 fork 在 Ubuntu 22.04、系统 fcitx5 5.0.14 上使用。

- 保留拼音、简拼、双拼、五笔、候选排序和学习、整句增强、表情、符号、计算器、自定义词与主题。
- 移除 Linux 插件和设置界面中的语音、翻译、相关快捷键、模型下载和管理、API 配置，以及 GNOME 语音浮条。
- Cargo 默认特性为空，默认只构建 `core`。Linux 插件不再链接 ALSA；默认核心不包含麦克风采集、语音识别、翻译 HTTP 请求和 llama-server 管理器。
- 共享 C ABI 的禁用接口和会话数据结构保留，避免破坏现有调用方；禁用的语音、翻译创建函数返回空指针，不启动后台任务。上游可选实现和 macOS 源码保留，Linux 安装不使用它们。

候选栏启用了 `- =` 翻页时，只有一页或已经到边界也保持候选栏，不上屏候选字或符号。关闭该翻页选项时仍按普通符号处理；计算器中的 `-` 继续表示减法。

## 本机更新

修改工作区后运行：

```sh
wowtype-update-local
```

命令读取本地工作区，不会执行 Git 拉取或推送，安装前保存程序和配置备份。安装使用系统 fcitx5 和系统 GTK，不需要私有输入法框架。

手动构建核心使用：

```sh
cargo build --release --locked --no-default-features -p wowtype-core
cargo test --locked --no-default-features -p wowtype-core
```

Linux 插件按 `fcitx5-addon/CMakeLists.txt` 构建，指定系统 fcitx5 开发文件、核心静态库和实际安装前缀。设置和插件选项由 `spec/config.json`、`scripts/test-config-contract.py` 共同校验。

## 验证

核心会话测试覆盖单页、首页、末页的翻页边界、关闭翻页选项后的符号输入，以及计算器减法。`scripts/addon-test.sh` 在独立输入法实例中验证实际按键和候选栏；`scripts/test-settings-gui.py` 使用临时配置验证五个设置页面、控件、恢复默认和自定义词，不修改个人配置。
