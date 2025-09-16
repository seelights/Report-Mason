# ReportMason - 文档处理工具

ReportMason是一个强大的文档处理工具，支持将DOCX和PDF文件转换为标准化的XML格式，并提取其中的字段、图片、表格和图表内容。

## ✨ 功能特性

- **文档转换**：支持DOCX和PDF文件转换为XML格式
- **内容提取**：提取文档中的图片、表格、图表
- **字段识别**：智能识别文档中的关键字段
- **模板管理**：支持模板加载、应用和保存
- **多格式导出**：支持XML和标准格式导出

## 🚀 快速开始

### 系统要求

- Windows 10/11
- Qt 6.9.1 或更高版本
- MinGW 编译器
- Poppler 24.08.0 (用于PDF处理)

### 安装依赖

#### 1. 安装Poppler库

ReportMason使用Poppler库进行PDF处理。请按照以下步骤安装：

1. **下载Poppler预编译版本**
   - 访问：https://github.com/oschwartz10612/poppler-windows/releases
   - 下载最新版本的Release包（推荐：Release-24.08.0-0.zip）

2. **解压Poppler**
   ```bash
   # 解压到系统目录
   # 建议解压到：D:\Program Files (x86)\Release-24.08.0-0\poppler-24.08.0
   ```

3. **配置环境变量**
   - 将Poppler的bin目录添加到系统PATH
   - 例如：`D:\Program Files (x86)\Release-24.08.0-0\poppler-24.08.0\Library\bin`

#### 2. 配置项目

项目已经预配置了Poppler路径，如果您的Poppler安装路径不同，请修改`ReportMason.pro`文件：

```pro
# Poppler库配置
INCLUDEPATH += "您的Poppler路径/Library/include"
INCLUDEPATH += "您的Poppler路径/Library/include/poppler"
INCLUDEPATH += "您的Poppler路径/Library/include/poppler/cpp"

# Poppler库文件
LIBS += -L"您的Poppler路径/Library/lib"
LIBS += -lpoppler-cpp
LIBS += -lpoppler
```

### 编译项目

1. **打开Qt Creator**
2. **打开项目**：选择`ReportMason.pro`文件
3. **配置构建**：选择MinGW编译器
4. **构建项目**：点击构建按钮

或者使用命令行：

```bash
qmake
mingw32-make
```

## 📖 使用说明

### 主界面

程序启动后，您将看到包含以下标签页的界面：

1. **XML测试工具**：测试文档转换和字段提取
2. **测试工具**：运行各种功能测试
3. **DOCX内容测试**：专门测试DOCX文件的内容提取
4. **PDF内容测试**：专门测试PDF文件的内容提取

### 使用步骤

1. **选择文件**：点击"选择文件"按钮选择DOCX或PDF文件
2. **转换文档**：点击"转换为XML"按钮进行转换
3. **提取内容**：使用相应的测试标签页提取图片、表格、图表
4. **导出结果**：选择XML或标准格式导出提取的内容

### 支持的文件格式

- **输入格式**：
  - DOCX文件（.docx）
  - PDF文件（.pdf）

- **输出格式**：
  - XML格式（结构化数据）
  - PNG格式（图片）
  - CSV格式（表格和图表数据）

## 🔧 技术架构

### 核心组件

- **FileConverter**：文件转换基类
- **DocToXmlConverter**：DOCX转XML转换器
- **PdfToXmlConverter**：PDF转XML转换器
- **FieldExtractor**：字段提取器
- **TemplateManager**：模板管理器

### 内容提取模块

- **ImageExtractor**：图片提取基类
- **TableExtractor**：表格提取基类
- **ChartExtractor**：图表提取基类
- **DocxImageExtractor**：DOCX图片提取器
- **PdfImageExtractor**：PDF图片提取器（支持Poppler）

### 工具模块

- **KZipUtils**：ZIP文件处理工具
- **ContentUtils**：内容处理工具
- **LogSystem**：日志系统

## 🐛 故障排除

### 常见问题

1. **编译错误：找不到Poppler头文件**
   - 检查Poppler安装路径是否正确
   - 确认`ReportMason.pro`中的路径配置

2. **运行时错误：找不到Poppler DLL**
   - 将Poppler的bin目录添加到系统PATH
   - 或将DLL文件复制到程序目录

3. **PDF处理失败**
   - 确认PDF文件未加密
   - 检查PDF文件是否损坏

4. **DOCX处理失败**
   - 确认DOCX文件格式正确
   - 检查文件是否被其他程序占用

### 调试模式

启用调试输出：

```cpp
// 在代码中添加
qDebug() << "调试信息";
```

## 📝 开发说明

### 添加新的内容提取器

1. 继承相应的基类（ImageExtractor、TableExtractor、ChartExtractor）
2. 实现纯虚函数
3. 在项目文件中添加新的源文件

### 扩展支持的文件格式

1. 创建新的转换器类继承FileConverter
2. 实现convertToXml方法
3. 在FileConverterFactory中注册新格式

## 📄 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 🤝 贡献

欢迎提交Issue和Pull Request来改进项目。

## 📞 支持

如果您遇到问题或有建议，请：

1. 查看本文档的故障排除部分
2. 在GitHub上提交Issue
3. 联系开发团队

---

**注意**：本项目需要Poppler库支持PDF处理。请确保正确安装和配置Poppler库。