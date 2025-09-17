#!/usr/bin/env python3
"""
批量替换C++文件中的字符串字面量为QStringLiteral
"""

import os
import re
import glob

def fix_string_literals_in_file(file_path):
    """修复单个文件中的字符串字面量"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 备份原文件
        backup_path = file_path + '.backup'
        with open(backup_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        # 替换规则
        replacements = [
            # 替换字符串字面量在函数调用中
            (r'(\w+)\("([^"]*)"\)', r'\1(QS("\2"))'),
            # 替换字符串字面量在赋值中
            (r'(\w+)\s*=\s*"([^"]*)"', r'\1 = QS("\2")'),
            # 替换字符串字面量在构造函数中
            (r'new\s+\w+\("([^"]*)"', r'new \w+(QS("\1")'),
            # 替换字符串字面量在数组初始化中
            (r'\{[^}]*"([^"]*)"[^}]*\}', r'{\1}'),
        ]
        
        # 应用替换
        for pattern, replacement in replacements:
            content = re.sub(pattern, replacement, content)
        
        # 写回文件
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f"已处理: {file_path}")
        return True
        
    except Exception as e:
        print(f"处理文件 {file_path} 时出错: {e}")
        return False

def main():
    """主函数"""
    # 要处理的文件模式
    patterns = [
        'src/*.cpp',
        'src/*.h',
        'tools/**/*.cpp',
        'tools/**/*.h'
    ]
    
    files_processed = 0
    files_failed = 0
    
    for pattern in patterns:
        for file_path in glob.glob(pattern, recursive=True):
            if fix_string_literals_in_file(file_path):
                files_processed += 1
            else:
                files_failed += 1
    
    print(f"\n处理完成:")
    print(f"成功处理: {files_processed} 个文件")
    print(f"处理失败: {files_failed} 个文件")

if __name__ == "__main__":
    main()
