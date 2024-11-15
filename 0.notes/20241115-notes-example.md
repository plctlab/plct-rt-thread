文章标题：**笔记文档的例子模板**

- 作者: 汪辰
- 联系方式: <unicorn_wang@outlook.com>
- 原文地址: <https://github.com/plctlab/plct-rt-thread/blob/notes/0.notes/20241115-notes-example.md>。

文章大纲

<!-- TOC -->

- [1. 第一章的标题](#1-第一章的标题)
	- [1.1. 第一章的第一节的标题](#11-第一章的第一节的标题)
	- [1.2. 第一章的第二节的标题](#12-第一章的第二节的标题)
- [2. 第二章的标题](#2-第二章的标题)
- [3. markdown 语法基本编写要求](#3-markdown-语法基本编写要求)
	- [3.1. 文字的例子](#31-文字的例子)
	- [3.2. 段落的例子](#32-段落的例子)
	- [3.3. 列表的例子：](#33-列表的例子)
	- [3.4. 代码的例子：](#34-代码的例子)
	- [3.5. Shell 的例子](#35-shell-的例子)
	- [3.6. 文字中引用函数，宏，变量名的例子](#36-文字中引用函数宏变量名的例子)
	- [3.7. 图片的例子](#37-图片的例子)

<!-- /TOC -->

# 1. 第一章的标题

## 1.1. 第一章的第一节的标题

章节一般到二级就差不多了，层次深度不宜过多。

## 1.2. 第一章的第二节的标题

# 2. 第二章的标题

# 3. markdown 语法基本编写要求

这里仅给出一些简单的例子，更多的例子，可以参考 [Basic writing and formatting syntax](https://docs.github.com/zh/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)。

## 3.1. 文字的例子

中英文数字混排时英文单词或者字母以及数字和中文之间加空格分隔，方便阅读。

```
英文 sample 和中文之间用空格分隔，单个字母 a 也一样。

数字 123 和中文之间用空格分隔。

标点符号如果用半角输入也建议加空格, 来和中文分隔。如果是全角下的符号，则可以不用额外的空格分隔。
```

**效果：**

英文 sample 和中文之间用空格分隔，单个字母 a 也一样。

数字 123 和中文之间用空格分隔。

标点符号如果用半角输入也建议加空格, 来和中文分隔。如果是全角下的符号，则可以不用额外的空格分隔。


## 3.2. 段落的例子

段落和段落之间也用空行分隔，方便阅读，不要挤在一起。

```
段落 1：这是一段很长的文字，中间是不用回车换行的，直接写下去，markdown 在转换成其他格式文档时会自动换行。XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX。

段落 2
```

**效果：**

段落 1：这是一段很长的文字，中间是不用回车换行的，直接写下去，markdown 在转换成其他格式文档时会自动换行。XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX。

段落 2


## 3.3. 列表的例子：

```
- 第一点

  列表项文字缩进对齐

- 第二点

- 第三点
```

**效果：**

- 第一点

  列表项文字缩进对齐

- 第二点

- 第三点


## 3.4. 代码的例子：

```c
#include <stdio.h>

void main()
{
	printf("hello!\n");
}
```

## 3.5. Shell 的例子

```shell
$ ls
. ..
```

## 3.6. 文字中引用函数，宏，变量名的例子

文字中引用函数，宏或者变量名时建议用 "`" 括起来

这个例子中调用了一个函数 `printf()`, 这个变量名叫做 `foo`, 这个宏名叫做 `MAX_NUM`, 这个环境变量 `$HOME`。

## 3.7. 图片的例子

![图片的名字](./pictures/20241115-notes-example/rt-thread.png)
