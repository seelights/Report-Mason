# ReportMason - 实验报告自动填充系统

ReportMason是一个基于Qt的C++应用程序，旨在帮助学生自动化处理实验报告模板，实现从各种格式文档（Word、PDF等）到结构化XML格式的转换，并提供智能字段抽取和模板管理功能。

## 功能特性

### 核心功能
- **多格式文档支持**：支持Word文档(.docx)和PDF文档(.pdf)的解析和转换
- **智能字段抽取**：基于正则表达式、关键词匹配和模板匹配的智能字段识别
- **模板管理系统**：完整的模板导入、存储、检索和管理功能
- **XML输出**：标准化的XML格式输出，便于后续处理和集成
- **数据库持久化**：使用SQLite数据库存储模板信息和配置

### 技术特性
- **模块化设计**：清晰的类层次结构，易于扩展和维护
- **配置驱动**：支持JSON格式的配置文件和抽取规则
- **错误处理**：完善的错误处理和状态管理机制
- **跨平台支持**：基于Qt框架，支持Windows、Linux、macOS

## 项目结构

```
ReportMason/
├── include/                 # 头文件目录
│   ├── FileConverter.h     # 文件转换器基类
│   ├── DocToXmlConverter.h # Word文档转换器
│   ├── PdfToXmlConverter.h # PDF文档转换器
│   └── TemplateManager.h   # 模板管理器
├── src/                    # 源文件目录
│   ├── FileConverter.cpp
│   ├── DocToXmlConverter.cpp
│   ├── PdfToXmlConverter.cpp
│   ├── TemplateManager.cpp
│   └── FieldExtractor.h    # 字段抽取器
├── examples/               # 示例代码
│   └── ExampleUsage.cpp    # 使用示例
├── templates/              # 模板存储目录
├── ReportMason.pro         # Qt项目文件
├── main.cpp               # 主程序入口
├── Widget.h/.cpp/.ui      # 主界面
└── README.md              # 项目说明文档
```

## 系统要求

### 开发环境
- **Qt 5.12+** 或 **Qt 6.0+**
- **C++17** 或更高版本
- **CMake 3.16+** 或 **qmake**
- **SQLite 3.0+**

### 依赖库
- **Qt Core, GUI, Widgets, SQL, Network, Archive** 模块
- **KArchive**（Qt内置ZIP处理模块，用于处理Office文档）
- **Poppler-Qt**（可选，用于PDF处理）

### 编译环境
- **Windows**: Visual Studio 2019+ 或 MinGW
- **Linux**: GCC 7+ 或 Clang 6+
- **macOS**: Xcode 10+ 或 Clang 6+

## 安装和编译

### 1. 获取源码
```bash
git clone <repository-url>
cd ReportMason
```

### 2. 安装依赖
```bash
# Ubuntu/Debian (基础依赖)
sudo apt-get install qtbase5-dev libsqlite3-dev

# Ubuntu/Debian (可选：完整功能)
sudo apt-get install libquazip5-dev libpoppler-qt5-dev

# Windows (使用vcpkg)
vcpkg install qt5-base

# Windows (可选：完整功能)
vcpkg install quazip poppler

# macOS (使用Homebrew)
brew install qt5

# macOS (可选：完整功能)
brew install quazip poppler
```

**注意**: QuaZip库是可选的。未安装时项目仍可正常编译，但DOCX处理功能会被禁用。详细说明请参考`QUAZIP_SETUP.md`。

### 3. 编译项目
在Qt Creator中打开`ReportMason.pro`文件，然后点击"构建"按钮即可编译项目。

或者使用命令行：
```bash
# 使用qmake
qmake ReportMason.pro
make
```

## 使用方法

### 基本用法

#### 1. Word文档转换
```cpp
#include "include/DocToXmlConverter.h"

DocToXmlConverter converter;
QMap<QString, FileConverter::FieldInfo> fields;

// 提取字段
FileConverter::ConvertStatus status = converter.extractFields("report.docx", fields);

if (status == FileConverter::ConvertStatus::SUCCESS) {
    // 转换为XML
    QByteArray xmlOutput;
    converter.convertToXml(fields, xmlOutput);
    
    // 保存XML文件
    QFile file("output.xml");
    file.open(QIODevice::WriteOnly);
    file.write(xmlOutput);
}
```

#### 2. PDF文档转换
```cpp
#include "include/PdfToXmlConverter.h"

PdfToXmlConverter converter;
QMap<QString, FileConverter::FieldInfo> fields;

// 提取字段
FileConverter::ConvertStatus status = converter.extractFields("report.pdf", fields);

if (status == FileConverter::ConvertStatus::SUCCESS) {
    // 处理提取的字段...
}
```

#### 3. 模板管理
```cpp
#include "include/TemplateManager.h"

TemplateManager manager;
manager.initialize();

// 导入模板
int templateId = manager.importTemplate(
    "template.docx",
    "实验报告模板",
    "用于实验报告的Word模板",
    QStringList{"实验", "报告"}
);

// 获取模板信息
TemplateInfo info = manager.getTemplate(templateId);

// 设置字段映射
QJsonObject fieldMapping = {
    {"Title", QJsonObject{{"type", "text"}, {"required", true}}},
    {"StudentName", QJsonObject{{"type", "text"}, {"required", true}}}
};
manager.setTemplateFieldMapping(templateId, fieldMapping);
```

#### 4. 智能字段抽取
```cpp
#include "src/FieldExtractor.h"

FieldExtractor extractor;
QMap<QString, FileConverter::FieldInfo> fields;

// 从文本中抽取字段
QString text = "实验报告题目：算法分析实验\n姓名：张三\n学号：2023001001";
extractor.extractFieldsWithTemplate(text, "experiment_report", fields);

// 显示抽取结果
for (auto it = fields.begin(); it != fields.end(); ++it) {
    qDebug() << it.key() << ":" << it.value().content;
}
```

### 高级用法

#### 自定义抽取规则
```cpp
FieldExtractor::ExtractionRule rule;
rule.fieldName = "CustomField";
rule.description = "自定义字段";
rule.patterns = {"自定义[：:]\s*(.+?)(?:\n|$)"};
rule.keywords = {"自定义", "特殊"};
rule.required = false;
rule.strategy = FieldExtractor::ExtractionStrategy::REGEX_PATTERN;

extractor.addExtractionRule(rule);
```

#### 批量处理
```cpp
TemplateManager manager;
manager.initialize();

QStringList templateFiles = {"template1.docx", "template2.pdf", "template3.docx"};

for (const QString &filePath : templateFiles) {
    int templateId = manager.importTemplate(filePath);
    if (templateId > 0) {
        qDebug() << "成功导入模板:" << filePath << "ID:" << templateId;
    }
}
```

## 配置说明

### 模板配置格式
```json
{
    "version": "1.0",
    "name": "实验报告模板",
    "description": "用于实验报告的模板",
    "fields": {
        "Title": {
            "type": "text",
            "required": true,
            "description": "实验报告题目"
        },
        "StudentName": {
            "type": "text", 
            "required": true,
            "description": "学生姓名"
        }
    },
    "rules": {
        "patterns": {
            "Title": ["题目[：:]\s*(.+?)(?:\n|$)"],
            "StudentName": ["姓名[：:]\s*(.+?)(?:\n|$)"]
        },
        "keywords": {
            "Title": ["题目", "实验名称"],
            "StudentName": ["姓名", "学生"]
        }
    }
}
```

### 抽取规则配置
```json
{
    "caseSensitive": false,
    "removeWhitespace": true,
    "mergeSimilarFields": true,
    "similarityThreshold": 0.8,
    "maxRetries": 3,
    "timeout": 5000
}
```

## API文档

### FileConverter（基类）
- `isSupported(const QString &filePath)` - 检查是否支持指定文件格式
- `extractFields(const QString &filePath, QMap<QString, FieldInfo> &fields)` - 提取字段
- `convertToXml(const QMap<QString, FieldInfo> &fields, QByteArray &xmlOutput)` - 转换为XML

### DocToXmlConverter（Word转换器）
- `extractSdtFields(const QString &docxPath, QMap<QString, FieldInfo> &fields)` - 提取SDT字段
- `createFilledDocx(const QString &templatePath, const QMap<QString, FieldInfo> &fields, const QString &outputPath)` - 创建填充文档

### PdfToXmlConverter（PDF转换器）
- `extractTextContent(const QString &pdfPath, QString &textContent)` - 提取文本内容
- `extractFormFields(const QString &pdfPath, QMap<QString, FieldInfo> &formFields)` - 提取表单字段

### TemplateManager（模板管理器）
- `importTemplate(const QString &filePath, ...)` - 导入模板
- `getTemplate(int templateId)` - 获取模板信息
- `setTemplateFieldMapping(int templateId, const QJsonObject &fieldMapping)` - 设置字段映射
- `exportTemplateConfig(int templateId, const QString &exportPath)` - 导出模板配置

### FieldExtractor（字段抽取器）
- `extractFieldsWithTemplate(const QString &text, const QString &templateType, QMap<QString, FieldInfo> &fields)` - 基于模板抽取字段
- `addExtractionRule(const ExtractionRule &rule)` - 添加自定义抽取规则
- `validateExtractionResult(const QMap<QString, FieldInfo> &fields, const QList<ExtractionRule> &rules)` - 验证抽取结果

## 扩展开发

### 添加新的文件格式支持
1. 继承`FileConverter`基类
2. 实现所有纯虚函数
3. 在`TemplateManager`中注册新的转换器
4. 更新`getFileFormat`函数

### 添加新的抽取策略
1. 在`FieldExtractor::ExtractionStrategy`枚举中添加新策略
2. 实现对应的抽取方法
3. 在`extractFields`方法中添加策略处理逻辑

### 自定义模板类型
1. 在`FieldExtractor`中添加新的预定义规则
2. 更新`initializePredefinedRules`方法
3. 创建对应的模板配置文件

## 故障排除

### 常见问题

#### 1. 编译错误
- 确保安装了所有必需的依赖库
- 检查Qt版本是否满足要求
- 验证C++标准设置是否正确

#### 2. 运行时错误
- 检查文件路径是否正确
- 确保有足够的文件读写权限
- 验证输入文件格式是否支持

#### 3. 字段抽取不准确
- 调整正则表达式模式
- 增加关键词匹配规则
- 使用混合抽取策略

### 调试技巧
- 启用Qt的调试输出：`QT_LOGGING_RULES="*=true"`
- 使用`qDebug()`输出调试信息
- 检查XML输出格式是否正确

## 贡献指南

1. Fork项目仓库
2. 创建特性分支：`git checkout -b feature/new-feature`
3. 提交更改：`git commit -am 'Add new feature'`
4. 推送分支：`git push origin feature/new-feature`
5. 提交Pull Request

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 联系方式

- 项目主页：[GitHub Repository URL]
- 问题反馈：[Issues URL]
- 邮箱：[Contact Email]

## 更新日志

### v1.0.0 (2024-01-01)
- 初始版本发布
- 支持Word和PDF文档转换
- 实现模板管理系统
- 提供智能字段抽取功能
- 完整的XML输出支持

---

**注意**: 这是一个教育项目，主要用于学习和研究目的。在生产环境中使用前，请进行充分的测试和验证。
