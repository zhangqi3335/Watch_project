# Watch Project - 嵌入式学习与作业提交

![alt text](https://img.shields.io/badge/Focus-Embedded_System-blue)
![alt text](https://img.shields.io/badge/Language-C/C++-orange)
![alt text](https://img.shields.io/badge/Tool-Keil_MDK-lightgrey)

## 📌 项目简介

本仓库用于记录我的 **嵌入式开发学习历程** 以及 **作业代码提交**。
主要涵盖了基于 STM32/单片机 的各项实验、驱动开发以及逻辑功能的实现。

- **目标**：保持代码整洁、实现版本控制、方便在不同设备间同步。
- **内容**：包含 Keil 工程文件、源代码（.c/.h）、硬件电路图（如有）以及实验笔记。

------



## 🚀 智能同步脚本 ([gitupdata.ps1](gitupdata.ps1))

为了简化 Git 命令行操作并防止误传编译产生的冗余文件（如 .o, .obj 等），仓库内置了一个 PowerShell 自动化脚本。

### ✨ 主要功能

1. **智能文件选择**：自动识别修改过的工程文件夹或单个文件，支持序号多选，避免一键 git add . 导致的混乱。
2. **自动修复报错**：V3.2 修复了多文件提交时的字符串截取报错，运行更稳定。
3. **分支自动化**：自动检测当前分支，支持快捷切换或新建分支。
4. **智能备注**：支持自定义提交备注，若回车则自动生成带日期的默认备注。
5. **远程防冲突**：推送前自动检测远程更新，提醒先拉取（Pull）再推送（Push）。

### 🛠️ 如何运行

1. **配置路径**：右键编辑脚本，修改 $RepoPath 为你本地的实际存放路径。
2. **启动脚本**：
   - 右键点击 GiteeSync_Smart_V3.2.ps1 -> **使用 PowerShell 运行**。
   - 或者在终端执行：./GiteeSync_Smart_V3.2.ps1
3. **交互操作**：
   - 根据提示输入序号选择要上传的工程。
   - 输入 a 全选所有更改。
   - 直接回车完成提交并推送至 Gitee。

------



## 📦 如何在其他电脑上运行本项目

为了确保从本仓库下载的代码能够直接在 Keil 中编译运行，请遵循以下说明：

### 1. 过滤规则 (.gitignore)

本仓库已配置 .gitignore，**只上传源码和工程结构**，不会上传编译生成的临时文件（如 Objects/, Listings/）。

> **这样做的好处**：仓库体积小，且避免了因本地路径不一致导致的编译报错。

### 2. 运行步骤

1. 从 Gitee 下载 ZIP 包或使用 git clone 到本地。
2. 进入对应的实验文件夹，找到以 .uvprojx 结尾的项目文件。
3. **双击打开 Keil MDK**。
4. **关键步骤**：点击 Keil 工具栏的 **Rebuild All** (全部重建) 按钮 🛠️。
   - Keil 会根据你的本地环境重新生成编译文件。
5. 点击 **Download** 即可烧录到开发板。

------



## 📂 目录结构说明

```Text
Watch_project/
├── .gitignore              # Git 忽略文件配置
├── gitupdata.ps1 # 自动化同步工具
├── README.md               # 项目说明文档
├── 01_LED_Blink/           # 实验1：LED闪烁
│   ├── User/               # 用户代码
│   └── Project/            # Keil 工程文件
└── 02_UART_Test/           # 实验2：串口通信
```

------



**Author:** [zhang___qi](https://gitee.com/zhang___qi/projects)
**Status:** In Progress... 🚀