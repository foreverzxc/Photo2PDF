# Photo2PDF

A simple and easy-to-use image to PDF converter built with Qt.

[中文说明](#中文说明)

## Features

- **Import Images**: Support for PNG, JPG, JPEG, BMP formats
- **Import PDF**: Each page of PDF is parsed as an image
- **Reorder Images**: Drag and drop to adjust image order
- **Rotate Images**: Rotate images clockwise or counter-clockwise
- **Export to PDF**: Convert all images to a single PDF file
- **Multi-language**: Support for English and Simplified Chinese

## System Requirements

- Windows 10/11
- Qt 6.x

## Installation

1. Download the latest release from [Releases](https://github.com/yourusername/Photo2PDF/releases)
2. Extract the ZIP file
3. Run `Photo2PDF.exe`

## Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/Photo2PDF.git
cd Photo2PDF

# Open Photo2PDF.pro in Qt Creator
# Or build from command line:
qmake Photo2PDF.pro
make

# For Windows deployment:
windeployqt release/Photo2PDF.exe
```

## Usage

1. Click "Open Images" or "Open PDF" to add files
2. Drag rows to reorder images (optional)
3. Select an image and rotate if needed (optional)
4. Select save path and configure export settings
5. Click "Export PDF" to generate the PDF file

## Demo

https://github.com/user-attachments/assets/37b2b87c-6ec7-4fe7-b252-570056c53f6e

## License

MIT License

---

## 中文说明

Photo2PDF 是一个简单易用的图片转 PDF 工具，基于 Qt 开发。

### 功能特点

- **导入图片**: 支持 PNG、JPG、JPEG、BMP 格式
- **导入 PDF**: PDF 文件的每一页都会解析为图片
- **调整顺序**: 拖拽行来调整图片顺序
- **旋转图片**: 支持顺时针和逆时针旋转
- **导出 PDF**: 将所有图片转换为单个 PDF 文件
- **多语言**: 支持英文和简体中文

### 系统要求

- Windows 10/11
- Qt 6.x

### 安装

1. 从 [Releases](https://github.com/yourusername/Photo2PDF/releases) 下载最新版本
2. 解压 ZIP 文件
3. 运行 `Photo2PDF.exe`

### 使用方法

1. 点击"打开图片"或"打开 PDF"添加文件
2. 拖拽行来调整图片顺序（可选）
3. 选中图片并旋转（可选）
4. 选择保存路径并配置导出设置
5. 点击"导出 PDF"生成 PDF 文件

### 演示视频

https://github.com/user-attachments/assets/37b2b87c-6ec7-4fe7-b252-570056c53f6e

### 许可证

MIT License
