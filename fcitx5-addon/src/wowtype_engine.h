#ifndef WOWTYPE_FCITX5_ENGINE_H
#define WOWTYPE_FCITX5_ENGINE_H

#include <chrono>
#include <cstdint>
#include <utility>
#include <cstddef>
#include <future>
#include <memory>
#include <optional>
#include <cstdlib>
#include <string>
#include <vector>

#include <fcitx-config/configuration.h>
#include <fcitx-config/enum.h>
#include <fcitx-config/option.h>
#include <fcitx-config/rawconfig.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx-utils/dbus/bus.h>
#include <fcitx-utils/dbus/objectvtable.h>
#include <fcitx-utils/event.h>
#include <fcitx-utils/key.h>
#include <fcitx-utils/keysym.h>
#include <fcitx-utils/trackableobject.h>

#include "wowtype_core.h"

namespace fcitx {
class Instance;
class InputContext;
class InputContextEvent;
class InputPanel;
class Text;
class InputMethodEntry;
class KeyEvent;
} // namespace fcitx

class WowtypeState;

/// v + 空格打开的工具栏里现在是哪一页（符号、表情直接用表情面板）。

enum class WowtypeLayout { Horizontal, Vertical };
FCITX_CONFIG_ENUM_NAME(WowtypeLayout, "Horizontal", "Vertical");

/// 选项里显示中文（存盘仍是英文名）。
struct WowtypeLayoutAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "横排");
        config.setValueByPath("EnumI18n/1", "竖排");
    }
};

/// 打字途中方向键做什么（#129）。
enum class WowtypeArrowKeys { Candidates, Caret };
FCITX_CONFIG_ENUM_NAME(WowtypeArrowKeys, "Candidates", "Caret");

struct WowtypeArrowKeysAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "都用来选词");
        config.setValueByPath("EnumI18n/1", "左右键移动光标");
    }
};

/// 输入方案：全拼、双拼、五笔 86。
enum class WowtypeInputScheme { Quanpin, Shuangpin, Wubi86 };
FCITX_CONFIG_ENUM_NAME(WowtypeInputScheme, "Quanpin", "Shuangpin", "Wubi86");

struct WowtypeInputSchemeAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "拼音输入");
        config.setValueByPath("EnumI18n/1", "双拼输入");
        config.setValueByPath("EnumI18n/2", "五笔输入");
    }
};

enum class WowtypeShuangpinScheme { Ziranma, Xiaohe, Microsoft, Sogou, ABC, Ziguang, PinyinJiajia };
FCITX_CONFIG_ENUM_NAME(WowtypeShuangpinScheme, "Ziranma", "Xiaohe", "Microsoft", "Sogou", "ABC",
                       "Ziguang", "PinyinJiajia");

struct WowtypeShuangpinAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        const char *names[] = {"自然码", "小鹤双拼", "微软双拼", "搜狗双拼",
                               "智能 ABC", "紫光双拼", "拼音加加"};
        for (int i = 0; i < 7; ++i) {
            config.setValueByPath("EnumI18n/" + std::to_string(i), names[i]);
        }
    }
};

/// 分号、引号选第 2、3 个候选。
enum class WowtypeSelectKeys { Off, Semicolon, SemicolonQuote };
FCITX_CONFIG_ENUM_NAME(WowtypeSelectKeys, "Off", "Semicolon", "SemicolonQuote");

struct WowtypeSelectKeysAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "不用");
        config.setValueByPath("EnumI18n/1", "分号选第 2 个");
        config.setValueByPath("EnumI18n/2", "分号选第 2 个、引号选第 3 个（引号不再当隔音符）");
    }
};

/// 标点样式：中文标点 / ASCII 标点。
enum class WowtypePunctStyle { Fullwidth, Halfwidth };
FCITX_CONFIG_ENUM_NAME(WowtypePunctStyle, "Fullwidth", "Halfwidth");

struct WowtypePunctStyleAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "全角（中文标点）");
        config.setValueByPath("EnumI18n/1", "半角（英文标点）");
    }
};

/// 单按哪个 Shift 切换中英文。
enum class WowtypeShiftToggle { Both, Left, Right, Off };
FCITX_CONFIG_ENUM_NAME(WowtypeShiftToggle, "Both", "Left", "Right", "Off");

struct WowtypeShiftAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "左右 Shift 都行");
        config.setValueByPath("EnumI18n/1", "左 Shift");
        config.setValueByPath("EnumI18n/2", "右 Shift");
        config.setValueByPath("EnumI18n/3", "不用 Shift 切换");
    }
};

/// 切到 WowType 时用中文还是英文（#158、#121）。
enum class WowtypeInitialMode { Chinese, English, Last };
FCITX_CONFIG_ENUM_NAME(WowtypeInitialMode, "Chinese", "English", "Last");

struct WowtypeInitialModeAnnotation {
    bool skipDescription() const { return false; }
    bool skipSave() const { return false; }
    void dumpDescription(fcitx::RawConfig &config) const {
        config.setValueByPath("EnumI18n/0", "中文");
        config.setValueByPath("EnumI18n/1", "英文");
        config.setValueByPath("EnumI18n/2", "和上次切走时一样");
    }
};

/// ~/.config/fcitx5/conf/wowtype.conf。fcitx5 配置工具和 wowtype-config 都改这个文件；
/// 同名环境变量（WOWTYPE_FUZZY 等）设了的话优先，方便测试。
FCITX_CONFIGURATION(
    WowtypeConfig,
    fcitx::Option<WowtypeLayout, fcitx::NoConstrain<WowtypeLayout>,
                  fcitx::DefaultMarshaller<WowtypeLayout>, WowtypeLayoutAnnotation>
        layout{this, "CandidateLayout", "候选排列", WowtypeLayout::Horizontal};
    fcitx::Option<WowtypeArrowKeys, fcitx::NoConstrain<WowtypeArrowKeys>,
                  fcitx::DefaultMarshaller<WowtypeArrowKeys>, WowtypeArrowKeysAnnotation>
        arrowKeys{this, "ArrowKeys", "方向键", WowtypeArrowKeys::Candidates};
    fcitx::Option<int, fcitx::IntConstrain> pageSize{
        this, "PageSize", "每页候选个数", 9, fcitx::IntConstrain(3, 9)};
    fcitx::Option<bool> pageKeysMinus{this, "PageKeysMinusEqual", "翻页键：- =", true};
    fcitx::Option<bool> pageKeysBracket{this, "PageKeysBracket", "翻页键：[ ]", false};
    fcitx::Option<bool> pageKeysComma{this, "PageKeysCommaPeriod", "翻页键：, .", false};
    fcitx::Option<bool> pageKeysTab{this, "PageKeysTab", "翻页键：Shift+Tab Tab", false};
    fcitx::Option<bool> fuzzy{this, "Fuzzy", "模糊音（下面几组分别开关）", false};
    fcitx::Option<bool> fuzzyZh{this, "FuzzyZh", "模糊音：z = zh", true};
    fcitx::Option<bool> fuzzyCh{this, "FuzzyCh", "模糊音：c = ch", true};
    fcitx::Option<bool> fuzzySh{this, "FuzzySh", "模糊音：s = sh", true};
    fcitx::Option<bool> fuzzyNL{this, "FuzzyNL", "模糊音：n = l", false};
    fcitx::Option<bool> fuzzyFH{this, "FuzzyFH", "模糊音：f = h", false};
    fcitx::Option<bool> fuzzyRL{this, "FuzzyRL", "模糊音：r = l", false};
    fcitx::Option<bool> fuzzyAn{this, "FuzzyAn", "模糊音：an = ang", true};
    fcitx::Option<bool> fuzzyEn{this, "FuzzyEn", "模糊音：en = eng", true};
    fcitx::Option<bool> fuzzyIn{this, "FuzzyIn", "模糊音：in = ing", true};
    fcitx::Option<bool> fuzzyIan{this, "FuzzyIan", "模糊音：ian = iang", true};
    fcitx::Option<bool> fuzzyUan{this, "FuzzyUan", "模糊音：uan = uang", true};
    fcitx::Option<bool> fuzzyUn{this, "FuzzyUn", "模糊音：un = ong", false};
    fcitx::Option<bool> fuzzyAnAi{this, "FuzzyAnAi", "模糊音：an = ai", false};
    fcitx::Option<bool> fuzzyHuangWang{this, "FuzzyHuangWang", "模糊音：huang = wang", false};
    fcitx::Option<bool> fuzzyUnIong{this, "FuzzyUnIong", "模糊音：un = iong", false};
    fcitx::Option<bool> fuzzyEngOng{this, "FuzzyEngOng", "模糊音：eng = ong", false};
    fcitx::Option<bool> typo{this, "TypoCorrection", "按错键自动纠正", true};
    fcitx::Option<bool> rareExt{this, "RareExtChars", "候选末尾列出扩展 B 区以后的生僻字", false};
    fcitx::Option<WowtypeInputScheme, fcitx::NoConstrain<WowtypeInputScheme>,
                  fcitx::DefaultMarshaller<WowtypeInputScheme>, WowtypeInputSchemeAnnotation>
        inputScheme{this, "InputScheme", "输入方案", WowtypeInputScheme::Quanpin};
    fcitx::Option<WowtypeShuangpinScheme, fcitx::NoConstrain<WowtypeShuangpinScheme>,
                  fcitx::DefaultMarshaller<WowtypeShuangpinScheme>, WowtypeShuangpinAnnotation>
        shuangpinScheme{this, "ShuangpinScheme", "双拼方案", WowtypeShuangpinScheme::Ziranma};
    fcitx::Option<bool> shuangpinShowKeys{this, "ShuangpinShowKeys",
                                          "双拼时编辑区显示打的键（不翻成全拼）", false};
    fcitx::Option<bool> wubiAutoCommit{this, "WubiAutoCommit", "五笔四码唯一自动上屏", true};
    fcitx::Option<bool> wubiHint{this, "WubiHint", "五笔编码提示（候选后面显示还要打的码）", true};
    fcitx::Option<WowtypeShiftToggle, fcitx::NoConstrain<WowtypeShiftToggle>,
                  fcitx::DefaultMarshaller<WowtypeShiftToggle>, WowtypeShiftAnnotation>
        shiftToggle{this, "ShiftToggle", "单按 Shift 切换中英文", WowtypeShiftToggle::Both};
    fcitx::Option<WowtypeInitialMode, fcitx::NoConstrain<WowtypeInitialMode>,
                  fcitx::DefaultMarshaller<WowtypeInitialMode>, WowtypeInitialModeAnnotation>
        initialMode{this, "InitialMode", "切到 WowType 时", WowtypeInitialMode::Chinese};
    fcitx::Option<bool> traditional{this, "Traditional", "输出繁体字", false};
    fcitx::Option<WowtypePunctStyle, fcitx::NoConstrain<WowtypePunctStyle>,
                  fcitx::DefaultMarshaller<WowtypePunctStyle>, WowtypePunctStyleAnnotation>
        punctStyle{this, "PunctuationStyle", "标点样式", WowtypePunctStyle::Fullwidth};
    fcitx::Option<WowtypeSelectKeys, fcitx::NoConstrain<WowtypeSelectKeys>,
                  fcitx::DefaultMarshaller<WowtypeSelectKeys>, WowtypeSelectKeysAnnotation>
        selectKeys{this, "SelectKeys", "分号、引号选词", WowtypeSelectKeys::Off};
    fcitx::Option<bool> pickChar{this, "PickChar",
                                 "以词定字：打拼音时 [ ] 上屏候选的第一个、最后一个字", true};
    fcitx::Option<bool> association{this, "Association",
                                    "联想：上屏以后给出常跟在后面的词（Tab 选，数字、空格照常）", false};
    fcitx::Option<bool> dateTime{this, "DateTime",
                                 "打 riqi、shijian、xingqi 时给出当前日期时间", true};
    fcitx::Option<bool> calculator{this, "Calculator", "v 开头算算式（v1+2*3；双拼、五笔时是大写 V）",
                                   true};
    fcitx::Option<bool> toolbar{this, "Toolbar",
                                "v + 空格打开工具栏（剪贴板、常用语、符号、表情；双拼、五笔时不开）",
                                true};
    // 默认关：fcitx5 的 clipboard 插件不告诉我们哪条是密码管理器复制的密码
    fcitx::Option<bool> clipboardHistory{this, "ClipboardHistory",
                                         "记录剪贴板历史（只在内存里，最多 20 条）", false};
    fcitx::Option<bool> autoSpace{this, "AutoSpace", "汉字和英文、数字之间自动加空格", false};
    fcitx::Option<bool> autoSpaceLetters{this, "AutoSpaceLetters", "汉字和英文字母之间", true};
    fcitx::Option<bool> autoSpaceDigits{this, "AutoSpaceDigits", "汉字和数字之间", true};
    fcitx::Option<bool> emojiSuggest{this, "EmojiSuggest", "打字时推荐 emoji", true};
    fcitx::Option<fcitx::Key, fcitx::KeyConstrain> emojiKey{
        this, "EmojiKey", "打开表情面板的快捷键", fcitx::Key("Super+period"),
        fcitx::KeyConstrain(fcitx::KeyConstrainFlags{fcitx::KeyConstrainFlag::AllowModifierLess})};
    // 另设中英文切换键，默认不用；单按 Shift 照常。fcitx5 全局热键优先。
    fcitx::Option<fcitx::Key, fcitx::KeyConstrain> toggleKey{
        this, "ToggleKey", "切换中 / 英文的快捷键", fcitx::Key(),
        fcitx::KeyConstrain(fcitx::KeyConstrainFlags{fcitx::KeyConstrainFlag::AllowModifierLess})};
    fcitx::Option<bool> checkUpdate{this, "CheckUpdate", "自动检查更新（最多一小时一次）", true};
    // 词库文件（拼音<TAB>词<TAB>频次）。WowType 设置里切换「万象」时写成
    // ~/.local/share/wowtype/dicts/wanxiang/lexicon.tsv；文件不在就用默认的
    fcitx::Option<std::string> lexicon{this, "Lexicon", "词库文件（留空用自带的雾凇词库）", ""};
    fcitx::ExternalOption settings{this, "Settings", "自定义词和更多设置", "wowtype-config"};);

/// 键名、默认值、范围、可选值以 spec/config.json 为准（设置契约），改这里要先改规格；
/// fcitx5-addon/test/config_dump.cpp 把上面的导出来，scripts/test-config-contract.py 对照。

/// 把 wowtype.conf 读成的 RawConfig 装进 config，文件里没写的项是默认值（和 fcitx::readAsIni 一样）。
/// 这条规矩三端一样（settings/wowtype-config 的 read_config、macOS 的 WowtypeConfig.parse）。
/// 数字项超出范围时夹到边上（三端一样，见 spec/config.json 的 readRules.outOfRange）。
/// fcitx5 自己的 IntConstrain 是不认、回到默认值；认不出的数字（abc、空的）还是默认值。
inline void clampWowtypeInt(fcitx::Option<int, fcitx::IntConstrain> &option, const fcitx::RawConfig &raw,
                            int min, int max) {
    const std::string *text = raw.valueByPath(option.path());
    if (!text || text->empty()) {
        return;
    }
    char *end = nullptr;
    const long long n = std::strtoll(text->c_str(), &end, 10);
    if (end == text->c_str() || *end != '\0') {
        return;
    }
    if (n < min) {
        option.setValue(min);
    } else if (n > max) {
        option.setValue(max);
    }
}

/// 去掉两边的空格和 Tab（fcitx5 读文件时值前面的空格会留着：「Key=  值」读成「  值」）。
inline std::string trimWowtypeValue(const std::string &text) {
    const auto begin = text.find_first_not_of(" \t");
    if (begin == std::string::npos) {
        return "";
    }
    return text.substr(begin, text.find_last_not_of(" \t") - begin + 1);
}

/// 最上面一层的设置项，键和值两边的空格去掉（三端一样：「PageSize = 5」读成 5，见 spec/config.json 的 readRules）。
/// [分组]（[Hotkey] 等）是带子项的，不是设置项，不要。
inline fcitx::RawConfig wowtypeTopLevel(const fcitx::RawConfig &raw) {
    fcitx::RawConfig out;
    for (const auto &key : raw.subItems()) {
        auto item = raw.get(key);
        if (!item || !item->subItems().empty()) {
            continue;
        }
        out.setValueByPath(trimWowtypeValue(key), trimWowtypeValue(item->value()));
    }
    return out;
}

inline void loadWowtypeConfig(WowtypeConfig &config, const fcitx::RawConfig &file) {
    const fcitx::RawConfig raw = wowtypeTopLevel(file);
    config.load(raw);
    // 范围和上面 IntConstrain 的一样；不一样时契约测试（PageSize=12 读成 9）会报错
    clampWowtypeInt(config.pageSize, raw, 3, 9);
    // 0.3.12 及以前的双拼开关：只在没有 InputScheme 时算。WowtypeConfig 里没有它，存盘时就没了
    const std::string *oldShuangpin = raw.valueByPath("Shuangpin");
    if (!raw.valueByPath("InputScheme") && oldShuangpin && *oldShuangpin == "True") {
        config.inputScheme.setValue(WowtypeInputScheme::Shuangpin);
    }
}

class WowtypeEngine;

/// 后台载入的结果：核心（失败是空的），还有哪个用户文件在、却没读出来。
struct WowtypeLoaded {
    WowtypeEngineHandle *core = nullptr;
    bool userDictUnread = false;
    bool historyUnread = false;
    /// 载入的整句增强模型（路径 + 大小 + 修改时间，没载入是空），reloadConfig 按它认模型换没换
    std::string nnlmStamp;
};

/// 会话总线上的 org.wowtype.Input：外部命令切中 / 英 / 译（vim 退出插入模式切英文，#98）。
///   busctl --user call org.wowtype.Input /org/wowtype/Input org.wowtype.Input1 SetMode s en
/// Version 返回正在跑的插件的版本：升级以后设置界面拿它和装好的版本比，不一样（或者没有这个方法，
/// 是 0.4.0 及以前的插件）就提醒重启输入法。
class WowtypeControl : public fcitx::dbus::ObjectVTable<WowtypeControl> {
public:
    explicit WowtypeControl(WowtypeEngine *engine) : engine_(engine) {}
    std::string mode();
    /// zh / en / toggle（只在中英之间切），返回切完以后的模式。
    std::string setMode(const std::string &mode);
    std::string version();

private:
    WowtypeEngine *engine_;
    FCITX_OBJECT_VTABLE_METHOD(mode, "Mode", "", "s");
    FCITX_OBJECT_VTABLE_METHOD(setMode, "SetMode", "s", "s");
    FCITX_OBJECT_VTABLE_METHOD(version, "Version", "", "s");
};

class WowtypeEngine final : public fcitx::InputMethodEngineV2 {
public:
    explicit WowtypeEngine(fcitx::Instance *instance);
    ~WowtypeEngine() override;

    void keyEvent(const fcitx::InputMethodEntry &entry,
                  fcitx::KeyEvent &event) override;
    void activate(const fcitx::InputMethodEntry &entry,
                  fcitx::InputContextEvent &event) override;
    void deactivate(const fcitx::InputMethodEntry &entry,
                    fcitx::InputContextEvent &event) override;
    void reset(const fcitx::InputMethodEntry &entry,
               fcitx::InputContextEvent &event) override;
    std::string subMode(const fcitx::InputMethodEntry &entry,
                        fcitx::InputContext &ic) override;
    std::string subModeLabelImpl(const fcitx::InputMethodEntry &entry,
                                 fcitx::InputContext &ic) override;

    const fcitx::Configuration *getConfig() const override { return &config_; }
    void setConfig(const fcitx::RawConfig &config) override;
    /// 重新读配置文件和自定义词（设置界面改完后经 D-Bus ReloadAddonConfig 调用）。
    void reloadConfig() override;

    void updateInputPanel(fcitx::InputContext *ic, WowtypeState *state);
    /// 预编辑去向：全局开关（「在程序中显示预编辑文本」）打开且应用申报
    /// Preedit 能力时送进应用输入框（on-the-spot），否则画在候选窗顶部。
    void sendPreedit(fcitx::InputContext *ic, const fcitx::Text &preedit);
    /// 编辑区画在应用的输入框里（应用支持、全局设置也开着）。
    bool inlinePreedit(fcitx::InputContext *ic) const;
    /// 选中第 index 个候选（全局下标）：整句候选直接上屏，
    /// 句首词候选先压进已选栈，剩下的拼音继续输入。
    void commitCandidate(fcitx::InputContext *ic, WowtypeState *state,
                         std::size_t index);
    /// 点了工具栏 / 表情面板的第 index 项（会话做：菜单进下一层，别的上屏）。
    void choosePanelItem(fcitx::InputContext *ic, WowtypeState *state, std::size_t index);
    /// 展开的候选面板里点了第 row 行：光标在这行就选光标处的词，否则选这行第一个。
    void selectGridRow(fcitx::InputContext *ic, WowtypeState *state, std::size_t row);
    /// 工具栏 / 表情面板 / 展开面板的候选框上点了翻页按钮、滚了滚轮：和键盘上按 sym（Page Up /
    /// Page Down、上下键）一样交给会话，照会话的页和光标重画。面板的页码、光标只在会话里有一份。
    void panelListKey(fcitx::InputContext *ic, WowtypeState *state, fcitx::KeySym sym);

private:
    /// 词库在后台线程载入；载入好之前 core_ 为空，按键照常进编辑区。
    bool coreReady();
    /// 后台载入完了（loading_ 已经就绪，或者愿意等）：收下核心和「哪个用户文件没读出来」。
    void takeLoaded();
    /// 配置里换了词库：存好用户词、清掉各输入框的编辑状态，在后台换新词库。
    /// 上一次载入还没完就先记着，载完再换（不在主线程上等）。
    void maybeSwitchLexicon();
    void applyConfig();
    /// 词库载入好以后，把开关和自定义词交给核心。
    void applyCoreSettings();
    std::string customWordsPath() const;
    bool isEmojiKey(const fcitx::Key &key) const;
    uint32_t modeKeyAction(const fcitx::Key &key) const;
    /// 能自己切中 / 英：单按 Shift 没关，或者另设了切换键。都没有时一直是中文（切过去就切不回来）。
    bool canToggleMode() const;
    /// 单按 Shift 切换中英文：处理了（切换了）返回 true。
    /// Shift 键交给会话判断单按（别的键只报一下，打断单按）。返回 true：这个键处理完了
    /// （Shift 键都是；单按切换了模式时吃掉，否则交给应用）。
    bool handleShiftKey(fcitx::InputContext *ic, WowtypeState *state, fcitx::KeyEvent &event);
    void followSessionMode(fcitx::InputContext *ic, WowtypeState *state, const WowtypeFrame *frame);
    /// 把全局的中 / 英 / 译对齐到这个输入框的会话（切英文时会话上屏打到一半的）。
    void syncMode(fcitx::InputContext *ic, WowtypeState *state);
    /// 按「切到 WowType 时」（InitialMode）定中 / 英：从别的输入法切回来、启动后第一次用时调。
    void applyInitialMode(fcitx::InputContext *ic, WowtypeState *state);
    /// notify：弹出「中 / 英 / 译」提示（外部命令切的不弹，免得 vim 每按一次 Esc 闪一下）。
    void switchMode(fcitx::InputContext *ic, WowtypeState *state, bool ascii,
                    bool notify);

    const char *modeTitle() const;
    const char *modeLabel() const;
    /// 单调时钟的毫秒数（会话的 time_ms、tick 用）。
    static uint64_t nowMs();
    // 表情面板（快捷键打开，打拼音或英文搜 emoji 和符号）
    void fillToolPanel(fcitx::InputContext *ic, WowtypeState *state);
    /// 剪贴板历史：从 fcitx5 的 clipboard 插件拿最新一条交给核心（没有这个插件就什么都不做）。
    void sampleClipboard(fcitx::InputContext *ic);

public:

private:
    std::string recentEmojiPath() const;
    /// 把引擎、上文、设置同步给会话（引擎是后台载入的，设置可能刚改过）。
    void syncSession(WowtypeState *state);
    /// 同步后交给会话处理一个按下的键；action 是 WOWTYPE_KEY_ACTION_*（fcitx5 全局的翻页、
    /// 移动候选热键）。返回会话的 frame（会话没建好时为 NULL）。
    const WowtypeFrame *sessionKey(fcitx::InputContext *ic, WowtypeState *state,
                                   const fcitx::Key &key,
                                   uint32_t action = WOWTYPE_KEY_ACTION_NONE);
    /// 应用里光标前一个字（Unicode 码位）：光标前没有字是 0，应用不报告周围文字时没有值。
    std::optional<uint32_t> prevChar(fcitx::InputContext *ic) const;
    /// 这个键对应的 fcitx5 全局翻页、移动候选热键（方向键不算，照横排竖排的规则走）。
    uint32_t keyAction(const fcitx::Key &key) const;
    /// 会话处理了一个键以后：上屏、记 emoji、照会话的候选框重画（或只挪页码和光标）。
    void applySessionFrame(fcitx::InputContext *ic, WowtypeState *state,
                           const WowtypeFrame *frame);
    /// 候选由会话算，外壳抄一份画候选框（fcitx5 的候选框要整个列表），连同页码、光标。
    void refreshCandidates(WowtypeState *state);
    /// 会话的候选重算过才重新抄；返回 true 表示重抄了。页码、光标每次都抄。
    bool syncView(WowtypeState *state);
    void fillGridPanel(fcitx::InputContext *ic, WowtypeState *state);
    /// 上屏以后的联想行（会话的 WOWTYPE_PANEL_ASSOCIATION）。
    void fillAssociation(fcitx::InputPanel &panel, WowtypeState *state);
    std::string segmentedRemaining(WowtypeState *state);
    /// 会话上屏以后：上屏 frame->commit、清空外壳的状态；frame->learned 时攒着存用户词库。
    void finishCommit(fcitx::InputContext *ic, WowtypeState *state, const WowtypeFrame *frame);
    /// 会话这次调用里引擎学到了选词（frame->learned）就记一次，返回记了没有。会话给的每个 frame 都要过
    bool countLearned(const WowtypeFrame *frame);
    /// 上屏已选的词 + 还没选的原始输入（切输入法、打到一半切英文），原始输入也记一次选词。
    void commitInput(fcitx::InputContext *ic, WowtypeState *state);
    /// 选中光标所在的候选；没有候选时上屏原始输入（交给会话按一次空格）。
    void commitCurrent(fcitx::InputContext *ic, WowtypeState *state);
    /// 攒够上屏次数（force 时不管次数）就在后台存用户词库和输入习惯（按键的线程上不等磁盘）。
    /// wait：等后台写完再返回（析构、换词库前：新引擎要读这些文件）。
    void maybeSaveUserDict(bool force, bool wait = false);
    std::string userDictPath() const;

    /// 注册外部模式切换和版本查询接口。
    void setupControl();
    void maybeCheckUpdate();

    fcitx::Instance *instance_;
    WowtypeEngineHandle *core_;
    std::future<WowtypeLoaded> loading_;
    /// 用户词库（user.tsv）/ 输入习惯（history.tsv）文件在、这次载入却没读出来：存之前先把原文件改名成
    /// <原名>.bad-<时间戳> 留着（maybeSaveUserDict），不然会被只有这次运行里学到的内容盖掉。
    bool userDictUnread_ = false;
    bool historyUnread_ = false;
    /// 正在用的整句增强模型（WowtypeLoaded::nnlmStamp）
    std::string nnlmStamp_;
    /// core_ / loading_ 用的词库文件，和配置要的词库文件（不一样就该换了）。
    std::string lexicon_;
    std::string wantedLexicon_;
    /// lexicon_ 载入时的 (大小, 修改时间)：同一个路径的文件被换掉（更新了词库包）也要重载。
    std::pair<std::uintmax_t, long long> lexiconStamp_{0, 0};
    fcitx::FactoryFor<WowtypeState> factory_;
    int unsavedCommits_ = 0;

    WowtypeConfig config_;
    // 生效中的设置（配置文件 + 环境变量覆盖）
    bool horizontal_ = true;
    WowtypeArrowKeys arrowKeys_ = WowtypeArrowKeys::Candidates;
    std::size_t pageSize_ = 9;
    bool fuzzy_ = false;
    /// 打开的模糊音分组（位掩码，见 wowtype_engine_set_fuzzy_groups）。
    unsigned fuzzyGroups_ = 0;
    bool typo_ = true;
    bool rareExt_ = false;
    WowtypeSelectKeys selectKeys_ = WowtypeSelectKeys::Off;
    bool pickChar_ = true;
    bool association_ = false;
    bool dateTime_ = true;
    bool calculator_ = true;
    bool toolbar_ = true;
    bool clipboardHistory_ = false;
    /// 双拼时编辑区显示打的键（#122）
    bool shuangpinShowKeys_ = false;
    /// 自动补空格（wowtype_autospace 的 flags）：1 字母、2 数字，0 关。
    int autoSpaceFlags_ = 0;
    bool shuangpin_ = false;
    /// 五笔（设置里选了五笔；码表在 applyCoreSettings 里载入，没载入时会话照全拼打并提示）。
    bool wubi_ = false;
    /// 现在的引擎载入了五笔码表。
    bool wubiLoaded_ = false;
    /// 配置文件里的双拼方案名（传给核心）。
    std::string shuangpinScheme_ = "Ziranma";
    WowtypeShiftToggle shiftToggle_ = WowtypeShiftToggle::Both;
    WowtypeInitialMode initialMode_ = WowtypeInitialMode::Chinese;
    /// 启动后还没切到过 WowType：第一次切过来（包括 WowType 是默认输入法时第一次获得焦点）按 InitialMode 定模式
    bool initialModeApplied_ = false;
    WowtypePunctStyle punctStyle_ = WowtypePunctStyle::Fullwidth;
    /// 打开的翻页键组（Page Up/Down、方向键、fcitx 全局翻页键一直可用）
    bool pageMinus_ = true;
    bool pageBracket_ = false;
    bool pageComma_ = false;
    bool pageTab_ = false;
    /// 英文模式：按键原样交给应用（中英文切换全局生效，切到别的窗口也保持）。
    bool asciiMode_ = false;
    bool traditional_ = false;
    bool conversionLoaded_ = false;
    bool emojiSuggest_ = true;
    /// 表情面板快捷键，FcitxKey_None 表示关掉。
    fcitx::Key emojiKey_;
    fcitx::Key toggleKey_;
    /// 最近用过的 emoji（最近的在前），存在 ~/.local/share/wowtype/emoji-recent.txt。

    /// 自动检查更新的时间。
    std::optional<std::chrono::steady_clock::time_point> updateCheckedAt_;

    // 外部命令切模式：单独一条会话总线连接。control_ 要先于总线析构，所以排在后面
    std::unique_ptr<fcitx::dbus::Bus> controlBus_;
    std::unique_ptr<WowtypeControl> control_;

public:
    /// 现在的模式：zh / en。
    std::string modeName() const;
    bool setModeByName(const std::string &mode);
};

#endif /* WOWTYPE_FCITX5_ENGINE_H */
