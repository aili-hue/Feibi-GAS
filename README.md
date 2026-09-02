# Feibi-GAS（菲比）——基于 Unreal Engine C++ + GAS 的战斗原型（fan-made）

基于 Unreal Engine C++ 与 Gameplay Ability System (GAS) 的“菲比”战斗复刻与示例，包含：3 段普攻、重击、跳跃轻/重攻击与空地衔接逻辑的实现参考（fan-made）。本仓库提供一个小型、可阅读的代码基，用于演示如何使用 GAS 构建动作/连段与动画通知交互。

## 关键特性
- 使用 Unreal Engine C++ + GAS 实现战斗核心（能力、效果、属性集）。
- 示例角色实现：包含普攻三段、重击、跳跃轻/重攻击以及空中/着地衔接逻辑。
- 动画通知状态（AnimNotifyState）用于在动画区间启用/禁用物理与标签逻辑。
- 模块化目录结构，便于把各个子系统提取到现有项目中作为参考或复用。

## 技术栈
- 语言：C++
- 引擎 / 运行时：Unreal Engine（本项目为 UE 模块/插件式的 C++ 源码）
- 主要理念：Gameplay Ability System (GAS) 的能力（Ability）、效果（GameplayEffect）、属性集（AttributeSet）与动画通知结合

## 项目结构（片段）
Source/              — Unreal 模块源码
  GAS.Target.cs
  GASEditor.Target.cs
  GAS/               — 主模块代码
    GAS.Build.cs
    GAS.cpp
    GAS.h
    AnimNotifyState/
      GameplayTagAnimNotifyState.{h,cpp}   — 使用标签控制动画区间状态
      AnimNotifyState_PhysicalMode.{h,cpp}  — 物理模式控制（启用/禁用碰撞或受力）
    AttributeSet/
      AttributeSet/General/
        AttributeSet_General.{h,cpp}        — 示例属性集（生命、能量等）
    Character/
      MyCharacter.{h,cpp}                   — 示例角色类：连段与能力触发逻辑
    DataAsset/                              — 存放能力/数据资产（配置驱动）
    GameplayAbility/                        — 能力（Ability）实现目录
    GameplayEffect/                         — GameplayEffect 实现目录

（仓库还包含 LICENSE 与本 README.md）

## 核心模块说明
- MyCharacter.h / MyCharacter.cpp
  - 角色输入、连段状态机以及对 GAS Ability 的调用点。示例实现了普攻三段、重击、跳跃轻/重攻击与空中/着陆衔接判定的控制逻辑。
- AttributeSet_General.*
  - 定义角色的属性集（Attributes），通过 GAS 的属性系统驱动伤害、体力/能量等数值。
- GameplayTagAnimNotifyState & AnimNotifyState_PhysicalMode
  - 将动画区间与 GameplayTag 或物理/碰撞状态结合：在指定动画帧段添加/移除标签或切换物理交互，便于按帧控制命中判定和受力。
- GAS.Build.cs / GAS.cpp / GAS.h
  - 模块构建与入口配置，用于集成到 Unreal 的构建系统（.Build.cs 定义依赖、模块名等）。

## 快速上手（把模块集成到你的项目）
注意：本仓库没有显式的 .uproject 文件；按示例将模块作为参考集成到你已有的 Unreal 项目里。

1. 克隆仓库或下载源码：
   git clone https://github.com/aili-hue/Feibi-GAS.git

2. 将 Source/GAS 目录作为模块拷贝到你的 Unreal 项目的 `Source/` 下，或把整个仓库放在你的 workspace 中根据需要移动。

3. 生成项目文件（平台相关）：
   - Windows（示例）：在项目根目录运行 GenerateProjectFiles.bat（或右键 .uproject -> Generate Visual Studio project files）。
   - macOS/Linux：运行对应的 GenerateProjectFiles 脚本或使用 Unreal Editor 生成。

4. 用 IDE（Visual Studio / Rider / Xcode）打开项目并编译，或直接在 Unreal Editor 中编译模块。

5. 在编辑器中添加/配置 Ability、Effect、DataAsset（若需要），将示例角色 MyCharacter 作为 Pawn/Character 使用以测试连段与动画区间行为。

## Demo（视频 / 演示）

[![演示缩略图](https://i0.hdslb.com/bfs/archive/47bc8748fe6d732b735ab6bfc35c5705.jpg)](https://www.bilibili.com/video/BV1TVtS6REE5/)

演示视频（Bilibili）：[使用UE5 C++ GAS 简单复刻鸣潮菲比角色的攻击](https://www.bilibili.com/video/BV1TVtS6REE5/)


## 调试与测试建议
- 在动画蒙太奇中添加相应的 AnimNotifyState（参考代码中的实现）以验证命中窗口或物理状态切换。
- 在 Ability 的执行点打印日志或使用断点（IDE）跟踪 Ability 激活链路与属性变化。
- 确保项目中启用了 GameplayTags 与 GAS 相关插件与模块。

## 已知/可改进点
- 引擎版本兼容性：请确保与你的 Unreal Engine 版本兼容（GAS API 随引擎版本可能有差异）。
- 样例数据资产（DataAsset）可以补充更多配置（当前为代码结构演示为主）。
- 更完整的示例关卡与输入绑定可以帮助快速验证连段逻辑。

## 联系
- 邮箱：13229323363@163.com

## 许可证
本仓库包含 LICENSE 文件，请参考仓库中的 LICENSE 以确定使用与再分发条款。

## 如果你想继续
- 想查看角色连段的具体实现请打开：Source/GAS/Character/MyCharacter.cpp
- 想查看动画区间如何控制命中/物理请查看：Source/GAS/AnimNotifyState/*
- 想把属性扩展为更多属性（如防御、格挡）请编辑：Source/GAS/AttributeSet/AttributeSet/General/AttributeSet_General.h/.cpp

---
