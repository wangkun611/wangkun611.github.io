---
title: IDA使用FAQ
date: 2023-04-12
draft: true
---

## 怎么修改二进制文件

1. 首先光标定位到要修改的代码行
2. 点击菜单选项：Edit->Patch program->Assemble...，打开Assemble instruction对话框
3. 在 Instruction中输入汇编代码后，点击OK按钮。IDA会实时的转成机器码
4. 点击OK后，IDA会根据机器码的长度，把修改位置移动到机器码的下一个字节
5. 循环3,4直到完成所有代码的修改
6. 点击菜单选项：Edit->Patch program->Apply patches to input file...，选择输入文件，是否需要备份等选项后，点击OK
7. 完成了文件修改

## jump 到任意位置

1. 定位到要跳转的位置，例如：00000001805E18D8，点击N,输入名称，最好起一个有意义的名字，也可以使用 loc_1805E18D8
2. 在 Assemble instruction对话框 中输入 jmp loc_1805E18D8
