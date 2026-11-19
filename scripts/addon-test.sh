#!/usr/bin/env bash
# 构建 fcitx5 插件并跑按键行为测试。
#
#   scripts/addon-test.sh [lexicon.tsv] [english.txt]
#   WOWTYPE_HARNESS_DBUS=$DBUS_SESSION_BUS_ADDRESS ...   # 使用指定的测试会话总线
#
# 起的是一个私有、无界面、无 D-Bus 的 fcitx5 实例（配置目录都在临时目录里），
# 不会影响正在用的输入法。不给词库时用仓库里的 demo 词典，部分用例会因为缺词失败。
set -euo pipefail
cd "$(dirname "$0")/.."

LEXICON=${1:-$PWD/core/data/demo_lexicon.tsv}
ENGLISH=${2:-}
BUILD=$(realpath -m "${WOWTYPE_ADDON_TEST_BUILD:-fcitx5-addon/build-test}")

cargo build --release --no-default-features -p wowtype-core
cmake -S fcitx5-addon -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DWOWTYPE_BUILD_TESTS=ON >/dev/null
cmake --build "$BUILD" -j

# 设置契约：插件的键名、默认值、范围、可选值和 spec/config.json 对得上（顺带对照设置界面）
python3 scripts/test-config-contract.py --fcitx5 "$BUILD/wowtype-config-dump"

ROOT=$(mktemp -d)
DBUS_PID=
trap '[[ -n "$DBUS_PID" ]] && kill "$DBUS_PID" 2>/dev/null; rm -rf "$ROOT"' EXIT

# 外部命令切模式的用例要会话总线：没给 WOWTYPE_HARNESS_DBUS 时起一条私有的（没有 dbus-daemon 时跳过）
if [[ -z "${WOWTYPE_HARNESS_DBUS:-}" ]] && command -v dbus-daemon >/dev/null; then
  WOWTYPE_HARNESS_DBUS=$(dbus-daemon --session --fork --print-address=1 --print-pid=3 3>"$ROOT/dbus.pid")
  DBUS_PID=$(cat "$ROOT/dbus.pid")
fi

mkdir -p "$ROOT/data/fcitx5/addon" "$ROOT/data/fcitx5/inputmethod" "$ROOT/config/fcitx5"
cp "$BUILD/wowtype-addon.conf" "$ROOT/data/fcitx5/addon/wowtype.conf"
cp "$BUILD/wowtype.conf" "$ROOT/data/fcitx5/inputmethod/wowtype.conf"
# fcitx5 自带的测试输入法，用来模拟「打到一半切换输入法」
cat > "$ROOT/data/fcitx5/addon/testim.conf" <<'ADDON'
[Addon]
Name=testim
Category=InputMethod
Library=libtestim
Type=SharedLibrary
OnDemand=False
ADDON
# 假的 clipboard 插件（测试程序里注册的静态插件），盖掉系统的那个。
# WOWTYPE_HARNESS_REAL_CLIPBOARD=1 时用系统装的 clipboard 插件，只确认调用它不出错
if [[ -z "${WOWTYPE_HARNESS_REAL_CLIPBOARD:-}" ]]; then
cat > "$ROOT/data/fcitx5/addon/clipboard.conf" <<'ADDON'
[Addon]
Name=clipboard
Category=Module
Library=clipboard
Type=StaticLibrary
OnDemand=False
ADDON
fi
cat > "$ROOT/config/fcitx5/profile" <<'PROFILE'
[Groups/0]
Name=Default
Default Layout=us
DefaultIM=wowtype

[Groups/0/Items/0]
Name=wowtype
Layout=

[Groups/0/Items/1]
Name=testim
Layout=

[GroupOrder]
0=Default
PROFILE

SYSTEM_ADDONS=$(pkg-config --variable=libdir Fcitx5Core 2>/dev/null || echo /usr/lib/x86_64-linux-gnu)/fcitx5
env -i PATH="$PATH" HOME="$ROOT" \
  XDG_DATA_HOME="$ROOT/data" XDG_CONFIG_HOME="$ROOT/config" XDG_RUNTIME_DIR="$ROOT" \
  XDG_DATA_DIRS="/usr/share" \
  FCITX_ADDON_DIRS="$BUILD:$SYSTEM_ADDONS" \
  WOWTYPE_LEXICON="$LEXICON" WOWTYPE_ENGLISH="$ENGLISH" WOWTYPE_LM="${WOWTYPE_LM:-}" WOWTYPE_LOG="${WOWTYPE_LOG:-0}" \
  WOWTYPE_USER_DICT="$ROOT/user.tsv" \
  DBUS_SESSION_BUS_ADDRESS="${WOWTYPE_HARNESS_DBUS:-}" \
  WOWTYPE_HARNESS_REAL_CLIPBOARD="${WOWTYPE_HARNESS_REAL_CLIPBOARD:-}" \
  WOWTYPE_NO_UPDATE_CHECK=1 "$BUILD/wowtype-key-harness"
