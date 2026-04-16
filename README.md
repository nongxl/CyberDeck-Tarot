# CyberDeck-Tarot 🔮

[English](#english) | [简体中文](#简体中文)

---

## English

A mystical Tarot reading terminal for the **M5Stack Cardputer**. This project transforms a portable hacking device into a portable oracle, combining high-tech cyberdeck aesthetics with ancient divination.

### 🌟 Features
- **Mystical UI**: Fully themed terminal interface with "breathing" animations and custom ritualistic descriptions.
- **LLM-Powered Readings**: Integrated with Large Language Models (e.g., GLM-4) to provide deep, contextual interpretations of your draws.
- **Bilingual Support**: Full support for English and Simplified Chinese.
- **Multiple Spreads**: Support for Single Card, Three-card, and the complex Celtic Cross spread.
- **Hardware Optimized**: Leverages M5Cardputer's keyboard, display, and IMU (parallax effects).

### 🛠️ Hardware Requirements
- **M5Stack Cardputer** (ESP32-S3)
- **MicroSD Card**: For configuration and asset storage.

### 💻 Software Dependencies
- **PlatformIO** (VS Code extension recommended)
- **Libraries**:
  - `M5Unified` & `M5Cardputer`
  - `M5GFX`
  - `ArduinoJson`
  - `HTTPClient`

### 🚀 Running Environment
1. Clone this repository.
2. Edit `config_example.json` and move it to your SD card as `/CyberDeck-Tarot/config.json`.
3. Set your `api_key` in the config file.
4. Flash the firmware using PlatformIO.

### 📜 Acknowledgments
Special thanks to [abdul-hamid-achik/tarotagent](https://github.com/abdul-hamid-achik/tarotagent) for the inspiration and core logic behind the Tarot interpretation engine.

---

## 简体中文

基于 **M5Stack Cardputer** 的神秘学塔罗终端。本项目将一台便携式黑客设备转变为便携式神谕，将高科技的 Cyberdeck 美学与古老的占卜艺术相结合。

### 🌟 功能特性
- **神秘主义 UI**: 全主题化的终端界面，带有“呼吸灯”动画和自定义仪式化描述。
- **大模型驱动解读**: 集成大语言模型（如 GLM-4），为您的抽牌提供深度、情境化的解读。
- **双语支持**: 完整支持英文和简体中文切换。
- **多种牌阵**: 支持单牌、三牌以及复杂的凯尔特十字牌阵。
- **硬件优化**: 充分利用 M5Stack Cardputer 的键盘、显示屏及 IMU（视差特效）。

### 🛠️ 硬件需求
- **M5Stack Cardputer** (ESP32-S3)
- **MicroSD 卡**: 用于存储配置文件和素材。

### 💻 软件依赖
- **PlatformIO** (建议配合 VS Code 使用)
- **相关库**:
  - `M5Unified` & `M5Cardputer`
  - `M5GFX`
  - `ArduinoJson`
  - `HTTPClient`

### 🚀 运行环境
1. 克隆本项目。
2. 修改 `config_example.json` 并将其作为 `/CyberDeck-Tarot/config.json` 放入 SD 卡中。
3. 在配置文件中设置您的 `api_key`。
4. 使用 PlatformIO 进行烧录。

### 📜 致谢
特别鸣谢 [abdul-hamid-achik/tarotagent](https://github.com/abdul-hamid-achik/tarotagent)，本项目参考了其塔罗解读引擎的核心逻辑与灵感。

---

## 🔮 Mystical Mappings | 神秘学描述对照

| Original Meaning | 原始含义 | Mystical (EN) | 神秘学描述 (ZH) |
| :--- | :--- | :--- | :--- |
| Booting System | 系统启动 | Aligning with the Celestial Spheres... | 正在对齐天球运行... |
| NO SD CARD | 找不到 SD 卡 | SOUL MISSING | 魂龛缺失 |
| Connecting | 正在连接 | Seeking the Unseen Threads... | 正在搜寻隐秘的联结... |
| Connected | WiFi 已连接 | Resonance Harmonized! | 共鸣已和谐！ |
| Connection Failed | 连接超时失败 | The Void remains silent. | 虚空保持着沉默。 |
| Scanning WiFi | 扫描网络 | Scrutinizing Ethereal Echoes... | 正在查探以太的回响... |
| Scan Failed | 未找到网络 | The path is hidden. Praying once more... | 路径已隐匿，再次祈求... |
| System Error | 系统错误 | The Arcane path is blocked: | 奥术路径受阻： |
| Shuffling | 正在洗牌 | Stirring the Waters of Destiny... | 正在搅动命运之泉... |
| Select Spread | 选择牌阵 | Form the Ritual Circle: | 布设仪式之阵： |
| Thinking... | 正在凝视/思考 | The Infinite whisper to Aurelia... | 无尽的虚空正向 Aurelia 低语... |
| Starts Speaking | 开始解读 | Echoes from the Astral Plane: | 来自星界的回响： |
| Complete | 解读结束 | [The Stars have spoken] | [众星已启示] |
| Navigation | 操作指引 | Traverse the stars with ; and . / ENTER for a new vision. | 以 ; 和 . 键拨动星河 / 回车召唤新幻象 |
