# QuaZip库安装指南

## 问题说明

当前项目中的`DocToXmlConverter`类需要QuaZip库来处理DOCX文件（DOCX文件本质上是ZIP格式）。如果您遇到以下错误：

```
QZipReader: No such file or directory
```

说明系统中缺少QuaZip库。

## 解决方案

### 方案1：安装QuaZip库（推荐）

#### Windows
```bash
# 使用vcpkg
vcpkg install quazip

# 或下载预编译版本
# 从 https://github.com/stachenov/quazip/releases 下载
```

#### Ubuntu/Debian
```bash
sudo apt-get install libquazip5-dev
```

#### CentOS/RHEL
```bash
sudo yum install quazip-devel
```

#### macOS
```bash
# 使用Homebrew
brew install quazip
```

#### 从源码编译
```bash
git clone https://github.com/stachenov/quazip.git
cd quazip
mkdir build && cd build
cmake ..
make
sudo make install
```

### 方案2：修改项目配置

安装QuaZip后，取消注释`app.pro`中的相关行：

```pro
# 取消注释这些行
win32: LIBS += -L$$PWD/libs -lquazip1
unix: LIBS += -L$$PWD/libs -lquazip

# 添加包含路径
INCLUDEPATH += /usr/include/quazip5  # Linux
INCLUDEPATH += /usr/local/include/quazip5  # macOS
```

### 方案3：使用当前简化版本

当前项目已经修改为简化版本，可以在没有QuaZip的情况下编译，但DOCX处理功能会被禁用。

## 验证安装

安装完成后，重新编译项目：

```bash
qmake
make
```

如果没有错误，说明QuaZip库安装成功。

## 功能限制

- **有QuaZip库**: 完整支持DOCX文件处理
- **无QuaZip库**: DOCX处理功能被禁用，但仍支持PDF和文本处理

## 替代方案

如果无法安装QuaZip，可以考虑：

1. **使用PDF格式**: 项目完全支持PDF文件处理
2. **使用纯文本**: 将DOCX内容复制为文本格式
3. **在线转换**: 使用在线工具将DOCX转换为PDF

## 项目结构说明

```
ReportMason/
├── src/DocToXmlConverter.cpp  # 包含QuaZip依赖的DOCX处理
├── src/PdfToXmlConverter.cpp  # 纯Qt实现，无外部依赖
├── src/FieldExtractor.cpp     # 纯Qt实现，无外部依赖
└── app/app.pro               # 项目配置文件
```

当前实现已经做了兼容性处理，即使没有QuaZip库也能正常编译和运行基本功能。
