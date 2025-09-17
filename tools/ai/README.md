# AI工具模块

这个模块提供了统一的AI服务调用接口，支持多个AI服务提供商（OpenAI、DeepSeek、豆包等），并提供了流式和非流式两种调用方式。

## 功能特性

- 🚀 **多厂商支持**：支持OpenAI、DeepSeek、豆包等主流AI服务
- 🔄 **流式/非流式**：支持实时流式输出和一次性完整响应
- ⚙️ **灵活配置**：支持自定义配置和预设模板
- 💾 **配置持久化**：自动保存和加载配置
- 🛡️ **错误处理**：完善的错误处理和超时机制
- 📊 **使用统计**：提供token使用量统计

## 文件结构

```
tools/ai/
├── LlmTypes.h              # 类型定义（枚举、结构体）
├── OpenAIBase.h/.cpp       # 抽象基类
├── OpenAINonStreamClient.h/.cpp  # 非流式客户端
├── OpenAIStreamClient.h/.cpp     # 流式客户端
├── AiManager.h/.cpp        # AI管理器
├── AiExample.cpp           # 使用示例
└── README.md              # 说明文档
```

## 快速开始

### 1. 基本使用

```cpp
#include "AiManager.h"

// 获取AI管理器实例
AiManager* manager = AiManager::instance();

// 快速聊天
QList<ChatMessage> messages = {
    {"system", "你是一个有帮助的助手"},
    {"user", "请介绍一下Qt框架"}
};

AiResponse response = manager->quickChat(messages, "DeepSeek");
if (response.success) {
    qDebug() << "AI回复:" << response.content;
}
```

### 2. 非流式客户端

```cpp
#include "OpenAINonStreamClient.h"

auto* client = manager->createNonStreamClient("DeepSeek");

connect(client, &OpenAINonStreamClient::finished, [](const QString& result) {
    qDebug() << "完整结果:" << result;
});

connect(client, &OpenAINonStreamClient::error, [](const QString& error) {
    qWarning() << "错误:" << error;
});

QList<ChatMessage> messages = {{"user", "你好"}};
client->start(messages);
```

### 3. 流式客户端

```cpp
#include "OpenAIStreamClient.h"

auto* client = manager->createStreamClient("DeepSeek");

connect(client, &OpenAIStreamClient::chunk, [](const QString& chunk) {
    qDebug() << "增量:" << chunk;  // 实时显示
});

connect(client, &OpenAIStreamClient::finished, [](const QString& result) {
    qDebug() << "完成:" << result;
});

QList<ChatMessage> messages = {{"user", "写一首诗"}};
client->start(messages);
```

### 4. 自定义配置

```cpp
// 创建自定义配置
LlmContext customConfig;
customConfig.provider = Provider::DeepSeek;
customConfig.baseUrl = "https://api.deepseek.com";
customConfig.apiKey = "your-api-key";
customConfig.model = "deepseek-chat";
customConfig.temperature = 0.5;
customConfig.maxTokens = 100;

// 添加配置
manager->addProvider("我的配置", customConfig);

// 使用配置
auto* client = manager->createNonStreamClient("我的配置");
```

## 支持的AI服务

### OpenAI
- **BaseURL**: `https://api.openai.com`
- **模型**: `gpt-4o`, `gpt-4`, `gpt-3.5-turbo`
- **特点**: 功能最全面，支持工具调用

### DeepSeek
- **BaseURL**: `https://api.deepseek.com`
- **模型**: `deepseek-chat`, `deepseek-reasoner`
- **特点**: 性价比高，推理能力强

### 豆包（火山方舟）
- **BaseURL**: `https://ark.cn-beijing.volces.com/api/v3`
- **模型**: `doubao-pro-4k`, `doubao-lite-4k`
- **特点**: 国内服务，响应速度快

## 配置管理

### 预设配置
系统提供了常用AI服务的预设配置：
- `OpenAI`: OpenAI GPT服务
- `DeepSeek`: DeepSeek服务
- `豆包`: 火山方舟豆包服务

### 配置文件
配置会自动保存到 `AppConfigLocation/ai_config.ini`，包含：
- 所有自定义配置
- 默认配置选择
- 各种参数设置

### 配置参数说明

| 参数 | 类型 | 说明 | 默认值 |
|------|------|------|--------|
| provider | Provider | 服务提供商 | OpenAI |
| baseUrl | QString | API基础URL | - |
| apiKey | QString | API密钥 | - |
| model | QString | 模型名称 | - |
| temperature | double | 温度参数(0-2) | 0.7 |
| topP | double | Top-p参数(0-1) | 1.0 |
| maxTokens | int | 最大token数 | 0(无限制) |
| timeoutMs | int | 超时时间(毫秒) | 60000 |
| stream | bool | 是否流式输出 | false |

## 错误处理

系统提供完善的错误处理机制：

```cpp
connect(client, &OpenAIBase::error, [](const QString& error) {
    // 处理各种错误：
    // - 网络错误
    // - API错误
    // - 配置错误
    // - 超时错误
    qWarning() << "AI请求失败:" << error;
});
```

## 使用统计

非流式请求会返回token使用统计：

```cpp
connect(client, &OpenAINonStreamClient::usageReady, [](const Usage& usage) {
    qDebug() << "输入tokens:" << usage.promptTokens;
    qDebug() << "输出tokens:" << usage.completionTokens;
    qDebug() << "总计tokens:" << usage.totalTokens;
});
```

## 注意事项

1. **API密钥安全**：请妥善保管API密钥，不要提交到版本控制系统
2. **网络环境**：确保网络可以访问对应的AI服务API
3. **请求频率**：注意各服务商的请求频率限制
4. **成本控制**：合理设置maxTokens参数控制成本
5. **错误重试**：建议实现重试机制处理临时网络错误

## 扩展开发

### 添加新的AI服务

1. 在`Provider`枚举中添加新类型
2. 在`AiManager::getPresetConfig`中添加预设配置
3. 如需特殊处理，可继承`OpenAIBase`创建专用客户端

### 自定义功能

- 工具调用：扩展消息结构支持工具调用
- 多模态：支持图片、音频等输入
- 缓存机制：添加响应缓存减少重复请求
- 批量处理：支持批量请求提高效率

## 许可证

本模块遵循项目的整体许可证。
