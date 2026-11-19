// WowType 的 GNOME Shell 扩展：
//   - stylesheet.css 把 Input Method Panel（kimpanel）画的候选框换成 WowType 的绿色主题，
//     GNOME Shell 启用扩展时自动加载，停用时自动卸载。
import {Extension} from 'resource:///org/gnome/shell/extensions/extension.js';

export default class WowTypeThemeExtension extends Extension {
    enable() {}
    disable() {}
}
