# hazel_demo

![Hazel](./Resources/Branding/Hazel_Logo_Text_Light_Square.png)

Hazel主要是一个早期的交互式应用程序和Windows渲染引擎。目前还没有太多的实现。



\## Getting Started

建议使用Visual Studio 2022，Hazel尚未在其他开发环境上进行正式测试，我们专注于Windows构建。



**1.下载仓库**



可以使用git将仓库克隆到本地目标：



```
git clone --recursive https://gitee.com/ljr1998/hazel_demo.git
```



确保您执行 `--recursive` 来获取所有的子模块！



**2. Configuring the dependencies:**

1. 运行脚本文件夹中的 `Setup.bat` 文件。如果还没有项目所需的前置条件，则将下载。

2. 其中一个前置条件是 `Vulkan SDK`。如果没有安装，脚本将执行 `VulkanSDK.exe` 文件，并提示用户安装 SDK。
3. 安装完成后，再次运行 `Setup.bat` 文件。如果 Vulkan SDK 安装正确，脚本就会下载 Vulkan SDK 调试库。(这可能需要较长的时间）
4. 下载并解压文件后，`Win-GenProjects.bat` 脚本文件将自动执行，然后生成 Visual Studio 解决方案文件供用户使用。



如果进行了更改，或想要重新生成项目文件，请重新运行脚本文件夹中的 `Win-GenProjects.bat `脚本文件。



##  计划

2D与3D分开进行设计，此仓库专注于2D开发

### 主要特点：

- 快速2D渲染（UI、粒子、精灵等）
- 高保真基于物理的3D渲染（稍后将扩展，首先是2D）
- 支持Mac、Linux、Android和iOS
  - 本机渲染API支持（DirectX、Vulkan、Metal）
- 功能齐全的查看器和编辑器应用程序
- 完全脚本化的交互和行为
- 集成的第三方2D和3D物理引擎
- 程序性地形和世界生成
- 人工智能
- 音频系统

