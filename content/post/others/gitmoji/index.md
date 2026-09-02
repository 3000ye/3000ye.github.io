---
title: "Git moji"
description: "Git 提交与 Moji 使用规范"
date: 2025-01-19T23:46:43+08:00
image: 
math: true
toc: true
tags:
    - Git
---

# Git Commit Message 规范

Git Commit Message 虽然可以随意描述，但使用没有意义的描述对于后续 review 代码以及理解代码用途等方面都会造成巨大的影响。因此 Commit Message 具有意义是最基本的要求，此外，你还应该遵守一定的格式规范，这样能够让大家更快更清晰地了解该 Commit 的详情。这里我主要介绍下常规的 Git Commit 规范和 Gitmoji 规范，最后介绍下我常用的相关配置。

Commit Message 最基本的组成为：`header`、`body`、`footer`：

```shell
<type>(<scope>): <subject>
<BLANK LINE>
<body>
<BLANK LINE>
<footer>
```

根据 conventional-commit-types, type 的可选值一般来说是有下面几种

- feat: 添加新功能
- fix: 修复 bug
- docs: 仅对文档进行修改
- style: 对代码语义无影响的格式修改（如去除无用空格、格式化等等修改）
- refactor: 代码重构（既不是新增功能，也不是修改 bug 的代码变动）
- perf: 提高性能的代码修改
- test: 测试用例添加及修改
- build: 影响构建系统或外部依赖关系的更改
- ci: 更改 CI 配置文件和脚本
- chore: 其它不涉及源码以及测试的修改

> 一般情况，commit 只会包含 header 部分。

Gitmoji 规范允许你在 Commit Message 中使用 Emoji ，简单的说，你可以用生动形象的 Emoji 来表示该次 Commit 的 type，Gitmoji 推荐的 header 格式是：

```shell
<gitmoji><type>: <subject>
```

常见的 moji 如下所示：

<table><thead><tr><th>view</th> <th>code</th> <th>mean</th> <th>translate</th></tr></thead> <tbody><tr><td>🎨</td> <td><code>:art:</code></td> <td>Improve structure / format of the code.</td> <td>改良结构和代码格式</td></tr> <tr><td>⚡️</td> <td><code>:zap:</code></td> <td>Improve performance.</td> <td>优化性能</td></tr> <tr><td>🔥</td> <td><code>:fire:</code></td> <td>Remove code or files.</td> <td>移除代码或文件</td></tr> <tr><td>🐛</td> <td><code>:bug:</code></td> <td>Fix a bug.</td> <td>修复 bug</td></tr> <tr><td>🚑</td> <td><code>:ambulance:</code></td> <td>Critical hotfix.</td> <td>紧急的热修复</td></tr> <tr><td>✨</td> <td><code>:sparkles:</code></td> <td>Introduce new features.</td> <td>引入新功能</td></tr> <tr><td>📝</td> <td><code>:memo:</code></td> <td>Add or update documentation.</td> <td>添加或更新文档</td></tr> <tr><td>🚀</td> <td><code>:rocket:</code></td> <td>Deploy stuff.</td> <td>部署</td></tr> <tr><td>💄</td> <td><code>:lipstick:</code></td> <td>Add or update the UI and style files.</td> <td>添加或更新 UI 和样式文件</td></tr> <tr><td>🎉</td> <td><code>:tada:</code></td> <td>Begin a project.</td> <td>开始一个全新的项目～</td></tr> <tr><td>✅</td> <td><code>:white_check_mark:</code></td> <td>Add or update tests.</td> <td>添加或更新测试用例</td></tr> <tr><td>🔒</td> <td><code>:lock:</code></td> <td>Fix security issues.</td> <td>修复安全问题</td></tr> <tr><td>🔖</td> <td><code>:bookmark:</code></td> <td>Release / Version tags.</td> <td>发版 / 版本标签</td></tr> <tr><td>🚨</td> <td><code>:rotating_light:</code></td> <td>Fix compiler / linter warnings.</td> <td>修复编译器或者 linter 的 warning</td></tr> <tr><td>🚧</td> <td><code>:construction:</code></td> <td>Work in progress.</td> <td>工作在进行中</td></tr> <tr><td>💚</td> <td><code>:green_heart:</code></td> <td>Fix CI Build.</td> <td>修复 CI 构建问题</td></tr> <tr><td>⬇️</td> <td><code>:arrow_down:</code></td> <td>Downgrade dependencies.</td> <td>降级依赖库</td></tr> <tr><td>⬆️</td> <td><code>:arrow_up:</code></td> <td>Upgrade dependencies.</td> <td>升级依赖库</td></tr> <tr><td>📌</td> <td><code>:pushpin:</code></td> <td>Pin dependencies to specific versions.</td> <td>将依赖库固定到特定版本</td></tr> <tr><td>👷</td> <td><code>:construction_worker:</code></td> <td>Add or update CI build system.</td> <td>添加或更新 CI 构建系统</td></tr> <tr><td>📈</td> <td><code>:chart_with_upwards_trend:</code></td> <td>Add or update analytics or track code.</td> <td>添加或更新分析或跟踪代码</td></tr> <tr><td>♻️</td> <td><code>:recycle:</code></td> <td>Refactor code.</td> <td>重构代码</td></tr> <tr><td>➕</td> <td><code>:heavy_plus_sign:</code></td> <td>Add a dependency.</td> <td>添加一个依赖项</td></tr> <tr><td>➖</td> <td><code>:heavy_minus_sign:</code></td> <td>Remove a dependency.</td> <td>删除一个依赖项</td></tr> <tr><td>🔧</td> <td><code>:wrench:</code></td> <td>Add or update configuration files.</td> <td>添加或改变配置文件</td></tr> <tr><td>🔨</td> <td><code>:hammer:</code></td> <td>Add or update development scripts.</td> <td>添加或更新构开发脚本</td></tr> <tr><td>🌐</td> <td><code>:globe_with_meridians:</code></td> <td>Internationalization and localization.</td> <td>国际化和本地化（i18n）</td></tr> <tr><td>✏️</td> <td><code>:pencil2:</code></td> <td>Fix typos.</td> <td>修复拼写错误</td></tr> <tr><td>💩</td> <td><code>:poop:</code></td> <td>Write bad code that needs to be improved.</td> <td>当前代码尚需优化</td></tr> <tr><td>⏪</td> <td><code>:rewind:</code></td> <td>Revert changes.</td> <td>撤销之前的修改</td></tr> <tr><td>🔀</td> <td><code>:twisted_rightwards_arrows:</code></td> <td>Merge branches.</td> <td>合并分支</td></tr> <tr><td>📦</td> <td><code>:package:</code></td> <td>Add or update compiled files or packages.</td> <td>添加或更新编译的文件或包</td></tr> <tr><td>👽</td> <td><code>:alien:</code></td> <td>Update code due to external API changes.</td> <td>由于外部 API 更改而更新代码</td></tr> <tr><td>🚚</td> <td><code>:truck:</code></td> <td>Move or rename resources (e.g.: files, paths, routes).</td> <td>移动或重命名资源（诸如：文件、路径、路由）</td></tr> <tr><td>📄</td> <td><code>:page_facing_up:</code></td> <td>Add or update license.</td> <td>添加或更新 LICENSE</td></tr> <tr><td>💥</td> <td><code>:boom:</code></td> <td>Introduce breaking changes.</td> <td>引入不兼容的变动</td></tr> <tr><td>🍱</td> <td><code>:bento:</code></td> <td>Add or update assets.</td> <td>添加或更新静态资源</td></tr> <tr><td>♿️</td> <td><code>:wheelchair:</code></td> <td>Improve accessibility.</td> <td>提高可访问性</td></tr> <tr><td>💡</td> <td><code>:bulb:</code></td> <td>Add or update comments in source code.</td> <td>在源代码中添加或更新注释</td></tr> <tr><td>🍻</td> <td><code>:beers:</code></td> <td>Write code drunkenly.</td> <td>醉醺醺地编写代码</td></tr> <tr><td>💬</td> <td><code>:speech_balloon:</code></td> <td>Add or update text and literals.</td> <td>添加或更新用于文本、对话框等文本字面量相关的更改</td></tr> <tr><td>🗃</td> <td><code>:card_file_box:</code></td> <td>Perform database related changes.</td> <td>执行与数据库相关的更改</td></tr> <tr><td>🔊</td> <td><code>:loud_sound:</code></td> <td>Add or update logs.</td> <td>添加或更新日志</td></tr> <tr><td>🔇</td> <td><code>:mute:</code></td> <td>Remove logs.</td> <td>删除日志</td></tr> <tr><td>👥</td> <td><code>:busts_in_silhouette:</code></td> <td>Add or update contributor(s).</td> <td>添加或更新贡献者</td></tr> <tr><td>🚸</td> <td><code>:children_crossing:</code></td> <td>Improve user experience / usability.</td> <td>改善用户体验/可用性</td></tr> <tr><td>🏗</td> <td><code>:building_construction:</code></td> <td>Make architectural changes.</td> <td>改变架构</td></tr> <tr><td>📱</td> <td><code>:iphone:</code></td> <td>Work on responsive design.</td> <td>进行响应式设计</td></tr> <tr><td>🤡</td> <td><code>:clown_face:</code></td> <td>Mock things.</td> <td>添加或更改 mock</td></tr> <tr><td>🥚</td> <td><code>:egg:</code></td> <td>Add or update an easter egg.</td> <td>添加或更新彩蛋内容</td></tr> <tr><td>🙈</td> <td><code>:see_no_evil:</code></td> <td>Add or update a .gitignore file.</td> <td>添加或更新 .gitignore 文件</td></tr> <tr><td>📸</td> <td><code>:camera_flash:</code></td> <td>Add or updatesnapshots.</td> <td>添加或更新快照</td></tr> <tr><td>⚗️</td> <td><code>:alembic:</code></td> <td>Perform experiments.</td> <td>实验性内容</td></tr> <tr><td>🔍</td> <td><code>:mag:</code></td> <td>Improve SEO.</td> <td>提高 SEO</td></tr> <tr><td>🏷</td> <td><code>:label:</code></td> <td>Add or update types.</td> <td>添加或更新类型声明</td></tr> <tr><td>🌱</td> <td><code>:seedling:</code></td> <td>Add or update seed files.</td> <td>添加或更新种子文件</td></tr> <tr><td>🚩</td> <td><code>:triangular_flag_on_post:</code></td> <td>Add, update, or remove feature flags.</td> <td>添加、更新或删除功能标志</td></tr> <tr><td>🥅</td> <td><code>:goal_net:</code></td> <td>Catch errors.</td> <td>捕获错误</td></tr> <tr><td>💫</td> <td><code>:dizzy:</code></td> <td>Add or update animations and transitions.</td> <td>添加或更新动画和过渡</td></tr> <tr><td>🗑</td> <td><code>:wastebasket:</code></td> <td>Deprecate code that needs to be cleaned up.</td> <td>清理冗余代码</td></tr> <tr><td>🛂</td> <td><code>:passport_control:</code></td> <td>Work on code related to authorization, roles and permissions.</td> <td>处理与授权、账户和权限相关的代码   ｜</td></tr> <tr><td>🩹 </td> <td><code>:adhesive_bandage:</code></td> <td>Simple fix for a non-critical issue.</td> <td>简单修复一个非关键问题</td></tr> <tr><td>🧐 </td> <td><code>:monocle_face:</code></td> <td>Data exploration/inspection.</td> <td>数据探索/检查</td></tr> <tr><td>⚰️</td> <td><code>:coffin:</code></td> <td>Remove dead code.</td> <td>清除无效代码</td></tr> <tr><td>🧪 </td> <td><code>:test_tube:</code></td> <td>Add a failing test.</td> <td>添加一个运行失败的测试用例（TDD 第一步）</td></tr> <tr><td>👔</td> <td><code>:necktie:</code></td> <td>Add or update business logic.</td> <td>添加或更新业务逻辑</td></tr> <tr><td>🩺</td> <td><code>:stethoscope:</code></td> <td>Add or update healthcheck.</td> <td>添加或更新健康检查</td></tr> <tr><td>🧱</td> <td><code>:bricks:</code></td> <td>Infrastructure related changes.</td> <td>基础设施相关改动</td></tr> <tr><td>🧑‍💻</td> <td><code>:technologist:</code></td> <td>Improve developer experience.</td> <td>提升开发体验</td></tr> <tr><td>🔐</td> <td><code>:closed_lock_with_key:</code></td> <td>Add or update secrets.</td> <td>添加或更新加密内容</td></tr> <tr><td>💸</td> <td><code>:money_with_wings:</code></td> <td>Add sponsorships or money related infrastructure.</td> <td>添加赞助商或者资金支持相关内容</td></tr> <tr><td>🧵</td> <td><code>:thread:</code></td> <td>Add or update code related to multithreading or concurrency.</td> <td>添加或更新多线程或与并发相关的代码</td></tr></tbody></table>