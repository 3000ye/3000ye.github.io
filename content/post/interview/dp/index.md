---
title: DP
description: 背包 DP 与多维 DP 完整培训文档
date: 2026-06-01T16:57:26+08:00
# image: assets/gitssh.png
math: true
toc: true
tags:
    - ICPC
categories:
    - Interview
---

# 背包 DP 与多维 DP 完整培训文档

> 从零推导到熟练应用的动态规划专题。覆盖动规思维框架、五大背包模型、状态压缩、多维 DP 设计方法论与高频题型。所有代码以 Python 为主，给出 C++ 关键差异。

---

## 目录

1. [动态规划思维框架](#1-动态规划思维框架)
2. [0-1 背包（基础与核心）](#2-0-1-背包)
3. [完全背包](#3-完全背包)
4. [多重背包](#4-多重背包)
5. [分组背包](#5-分组背包)
6. [二维费用背包](#6-二维费用背包)
7. [背包问题求方案数 / 具体方案](#7-背包问题求方案数--具体方案)
8. [多维 DP 设计方法论](#8-多维-dp-设计方法论)
9. [经典多维 DP 题型](#9-经典多维-dp-题型)
10. [状态压缩 DP](#10-状态压缩-dp)
11. [区间 DP / 树形 DP / 数位 DP 概览](#11-其它高维-dp-概览)
12. [优化技巧总览](#12-优化技巧总览)
13. [面试速查与决策树](#13-面试速查与决策树)

---

## 1. 动态规划思维框架

动态规划解决的是**有重叠子问题 + 最优子结构**的问题。求解任何 DP 题，按固定五步走：

1. **确定状态（dp 数组含义）**：`dp[i][j]` 到底代表什么？这是最关键一步。
2. **状态转移方程**：`dp[i][j]` 如何由更小的状态推出？
3. **初始化（base case）**：边界状态的值。
4. **遍历顺序**：保证计算 `dp[i][j]` 时它依赖的状态已算好。
5. **返回值**：答案在哪个状态。

**DP vs 记忆化搜索**：自底向上递推（迭代）= DP；自顶向下递归 + 缓存 = 记忆化搜索。二者等价，记忆化更直观、DP 常能做空间优化。

**判断能否用 DP**：
- 问题可分解为子问题，且子问题重复出现（重叠子问题）。
- 局部最优能拼出全局最优（最优子结构）。
- 无后效性：某状态一旦确定，后续决策不改变它。

---

## 2. 0-1 背包

### 问题定义
有 `n` 件物品，背包容量 `W`。第 `i` 件物品重量 `w[i]`、价值 `v[i]`，**每件最多取一次**。求不超过容量能装的最大价值。

### 二维 DP 推导（先理解这个）

**状态**：`dp[i][j]` = 从前 `i` 件物品中选，容量恰为 `j`（或≤j）时的最大价值。

**转移**：对第 `i` 件物品，要么不拿，要么拿（前提是装得下）：
```
不拿：dp[i][j] = dp[i-1][j]
拿  ：dp[i][j] = dp[i-1][j - w[i]] + v[i]   (需 j >= w[i])
dp[i][j] = max(上面两者)
```

**初始化**：`dp[0][*] = 0`（没有物品价值为 0）。

```python
def knapsack_01_2d(w, v, W):
    n = len(w)
    dp = [[0] * (W + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        for j in range(W + 1):
            dp[i][j] = dp[i-1][j]                    # 不拿第 i 件
            if j >= w[i-1]:                            # 装得下才考虑拿
                dp[i][j] = max(dp[i][j],
                               dp[i-1][j - w[i-1]] + v[i-1])
    return dp[n][W]
```

### 一维滚动数组优化（必须掌握）

观察转移只依赖**上一行** `dp[i-1][...]`，可压成一维 `dp[j]`：

```python
def knapsack_01_1d(w, v, W):
    n = len(w)
    dp = [0] * (W + 1)
    for i in range(n):
        for j in range(W, w[i] - 1, -1):   # ⚠️ 容量倒序遍历
            dp[j] = max(dp[j], dp[j - w[i]] + v[i])
    return dp[W]
```

**为什么 0-1 背包内层必须倒序？**（最高频面试考点）
一维数组里 `dp[j]` 依赖 `dp[j - w[i]]`。如果**正序**遍历，`dp[j - w[i]]` 在本轮已被更新成"包含第 i 件物品"的值，导致第 i 件被重复选取，退化成完全背包。**倒序**保证 `dp[j - w[i]]` 仍是上一轮（不含第 i 件）的值，每件只取一次。

**复杂度**：时间 `O(n·W)`，空间一维 `O(W)`。

### 常见变体
- **恰好装满**：初始化 `dp[0]=0`，其余 `dp[j]=-inf`，最后看 `dp[W]` 是否 > -inf。
- **求是否能装满**（子集和）：`dp[j]` 为布尔，`dp[j] |= dp[j-w[i]]`。

---

## 3. 完全背包

### 问题定义
与 0-1 背包相同，但**每件物品可取无限次**。

### 一维 DP（与 0-1 的唯一区别：正序遍历）

```python
def knapsack_complete(w, v, W):
    n = len(w)
    dp = [0] * (W + 1)
    for i in range(n):
        for j in range(w[i], W + 1):   # ⚠️ 容量正序遍历
            dp[j] = max(dp[j], dp[j - w[i]] + v[i])
    return dp[W]
```

**为什么完全背包正序？**
正序时 `dp[j - w[i]]` 可能已包含第 i 件物品的结果，于是同一物品被多次累加——这正是"无限次取用"想要的效果。

**记忆口诀**：
- 0-1 背包 → 倒序（每件只用一次）
- 完全背包 → 正序（每件可用多次）
- 物品与容量两层循环，**求最值时谁内谁外都行**；**求组合/排列数时顺序有讲究**（见第 7 节）。

### 二维形式（便于理解）
```
dp[i][j] = max(dp[i-1][j], dp[i][j - w[i]] + v[i])
                            ^^^^ 注意是 i 不是 i-1（可重复取）
```

---

## 4. 多重背包

### 问题定义
第 `i` 件物品有数量上限 `c[i]` 件，价值 `v[i]`、重量 `w[i]`。

### 方法一：朴素展开（拆成 0-1 背包）
把每件物品按数量拆成 `c[i]` 个独立物品，复杂度 `O(W·Σc[i])`，数量大时超时。

### 方法二：二进制优化（重点掌握）
把 `c[i]` 件拆成 `1,2,4,...,2^k, 余数` 几组，每组当作一件 0-1 物品。任意 `0~c[i]` 的取数都能由这些 2 的幂组合出来。拆分后约 `log(c[i])` 件，复杂度降到 `O(W·Σlog c[i])`。

```python
def knapsack_multiple_binary(w, v, c, W):
    items = []                      # 拆分后的 (重量, 价值)
    for wi, vi, ci in zip(w, v, c):
        k = 1
        while k < ci:
            items.append((wi * k, vi * k))
            ci -= k
            k <<= 1
        if ci > 0:
            items.append((wi * ci, vi * ci))
    dp = [0] * (W + 1)
    for wi, vi in items:            # 当作 0-1 背包
        for j in range(W, wi - 1, -1):
            dp[j] = max(dp[j], dp[j - wi] + vi)
    return dp[W]
```

### 方法三：单调队列优化（最优 O(n·W)）
将转移按 `j mod w[i]` 分组，每组是滑动窗口最大值问题，用单调队列。适合 `c[i]` 极大的场景。面试能说出思路即可，代码较长。

```python
from collections import deque
def knapsack_multiple_monoqueue(w, v, c, W):
    dp = [0] * (W + 1)
    for wi, vi, ci in zip(w, v, c):
        for r in range(wi):                      # 按余数分组
            q = deque()                          # 存 (下标k, 校正值)
            j = r
            cnt = 0
            while j <= W:
                cur = dp[j] - cnt * vi
                while q and q[-1][1] <= cur:
                    q.pop()
                q.append((cnt, cur))
                while q[0][0] < cnt - ci:        # 超出数量窗口
                    q.popleft()
                dp[j] = q[0][1] + cnt * vi
                cnt += 1
                j += wi
    return dp[W]
```

**复杂度对比**：朴素 `O(W·Σc)` → 二进制 `O(W·Σlog c)` → 单调队列 `O(n·W)`。

---

## 5. 分组背包

### 问题定义
物品分成若干组，**每组最多选一件**（也可一件不选）。

### 状态与转移
`dp[j]` = 容量 j 的最大价值。三层循环：组 → 容量（倒序）→ 组内物品。

```python
def knapsack_group(groups, W):
    # groups: List[List[(w, v)]]
    dp = [0] * (W + 1)
    for group in groups:                  # 枚举每一组
        for j in range(W, -1, -1):        # ⚠️ 容量倒序（组内只选一件）
            for (wi, vi) in group:        # 枚举组内物品
                if j >= wi:
                    dp[j] = max(dp[j], dp[j - wi] + vi)
    return dp[W]
```

**关键**：容量循环在组内物品循环**外层且倒序**，保证每组只贡献一件。若把容量放最内层会破坏"每组选一件"的约束。

---

## 6. 二维费用背包

### 问题定义
每件物品有**两种代价**（如重量 + 体积），分别有容量上限 `W` 和 `V`。这是"多维 DP"在背包上的直接体现。

### 状态与转移
`dp[j][k]` = 重量不超 j、体积不超 k 的最大价值。两个代价维度都倒序（0-1 情形）。

```python
def knapsack_2d_cost(w, vol, val, W, V):
    dp = [[0] * (V + 1) for _ in range(W + 1)]
    for i in range(len(val)):
        for j in range(W, w[i] - 1, -1):       # 重量维倒序
            for k in range(V, vol[i] - 1, -1):  # 体积维倒序
                dp[j][k] = max(dp[j][k], dp[j - w[i]][k - vol[i]] + val[i])
    return dp[W][V]
```

**复杂度** `O(n·W·V)`。这正是从一维背包到多维 DP 的桥梁——**多加一个约束就多加一维状态**。

> 经典题：LeetCode 474「一和零」——字符串集合，限制 0 的个数 m 和 1 的个数 n，求最多能选多少字符串。本质就是二维费用 0-1 背包，两个费用是"0 的数量"和"1 的数量"，价值是"1"（选中计数）。

```python
# LeetCode 474 一和零
def findMaxForm(strs, m, n):
    dp = [[0] * (n + 1) for _ in range(m + 1)]
    for s in strs:
        zeros = s.count('0'); ones = len(s) - zeros
        for i in range(m, zeros - 1, -1):       # 两维都倒序
            for j in range(n, ones - 1, -1):
                dp[i][j] = max(dp[i][j], dp[i - zeros][j - ones] + 1)
    return dp[m][n]
```

---

## 7. 背包问题求方案数 / 具体方案

### 7.1 求方案数（组合数 vs 排列数）

把转移里的 `max` 换成 `+`（累加方案数），初始化 `dp[0]=1`。

**组合数（不计顺序）**：外层物品、内层容量。每件物品只在一个"层"被考虑，避免重复计数。
```python
# LeetCode 518 零钱兑换 II：求凑成 amount 的硬币组合数
def change(amount, coins):
    dp = [0] * (amount + 1)
    dp[0] = 1
    for coin in coins:                  # 物品在外
        for j in range(coin, amount + 1):
            dp[j] += dp[j - coin]
    return dp[amount]
```

**排列数（计顺序）**：外层容量、内层物品。同一组合的不同顺序被分别统计。
```python
# LeetCode 377 组合总和 IV：实际求排列数
def combinationSum4(nums, target):
    dp = [0] * (target + 1)
    dp[0] = 1
    for j in range(1, target + 1):      # 容量在外
        for x in nums:                  # 物品在内
            if j >= x:
                dp[j] += dp[j - x]
    return dp[target]
```

**核心区别（高频考点）**：
- **求组合数 → 物品外层、容量内层**
- **求排列数 → 容量外层、物品内层**
- 求最值时两层顺序不影响结果（但完全/0-1 的容量方向规则仍要遵守）。

### 7.2 求最少 / 最多物品数
```python
# LeetCode 322 零钱兑换：凑成 amount 的最少硬币数
def coinChange(coins, amount):
    INF = float('inf')
    dp = [0] + [INF] * amount
    for coin in coins:
        for j in range(coin, amount + 1):
            dp[j] = min(dp[j], dp[j - coin] + 1)
    return dp[amount] if dp[amount] != INF else -1
```

### 7.3 回溯求具体方案
保留二维 `dp`，从 `dp[n][W]` 逆推：若 `dp[i][j] != dp[i-1][j]` 说明第 i 件被选。
```python
def knapsack_trace(w, v, W):
    n = len(w)
    dp = [[0]*(W+1) for _ in range(n+1)]
    for i in range(1, n+1):
        for j in range(W+1):
            dp[i][j] = dp[i-1][j]
            if j >= w[i-1]:
                dp[i][j] = max(dp[i][j], dp[i-1][j-w[i-1]] + v[i-1])
    chosen, j = [], W
    for i in range(n, 0, -1):
        if dp[i][j] != dp[i-1][j]:   # 第 i 件被选
            chosen.append(i-1)
            j -= w[i-1]
    return dp[n][W], chosen[::-1]
```

---

## 8. 多维 DP 设计方法论

"多维 DP" = 状态需要**两个及以上变量**才能唯一描述。设计步骤：

### 8.1 识别需要几维状态
问自己：**"要确定当前局面，我需要记住哪些信息？"** 每个独立、会影响后续决策的"上下文"就是一维。
- 走网格：位置 (i, j) → 二维。
- 两个序列对比（LCS/编辑距离）：两个指针 (i, j) → 二维。
- 股票买卖：天数 i + 持仓状态 + 已交易次数 k → 三维。
- 背包加约束：物品 i + 剩余容量1 + 剩余容量2 → 多维。

### 8.2 通用模板
```
dp[维度1][维度2]...[维度k] = 该局面下的最优解
转移：枚举"最后一步"的所有决策，从前驱状态转移
```

### 8.3 降维技巧
- 若 `dp[i]` 只依赖 `dp[i-1]`：滚动数组，第一维压成 2 行或 1 行。
- 状态量小但取值离散：用状压（见第 10 节）。

### 8.4 设计检查清单
1. 状态定义能否唯一确定一个子问题？（无歧义、无后效性）
2. 转移是否覆盖了"最后一步"的所有选择？
3. 初始化是否对应真实的边界局面？
4. 遍历顺序是否保证依赖项先算？
5. 答案是单个状态还是要在多个状态里取最值？

---

## 9. 经典多维 DP 题型

### 9.1 网格路径（二维）
```python
# 最小路径和 LeetCode 64
def minPathSum(grid):
    m, n = len(grid), len(grid[0])
    dp = [[0]*n for _ in range(m)]
    dp[0][0] = grid[0][0]
    for i in range(m):
        for j in range(n):
            if i==0 and j==0: continue
            up   = dp[i-1][j] if i>0 else float('inf')
            left = dp[i][j-1] if j>0 else float('inf')
            dp[i][j] = grid[i][j] + min(up, left)
    return dp[m-1][n-1]
```

### 9.2 最长公共子序列 LCS（双序列二维）
```python
# LeetCode 1143
def lcs(a, b):
    m, n = len(a), len(b)
    dp = [[0]*(n+1) for _ in range(m+1)]
    for i in range(1, m+1):
        for j in range(1, n+1):
            if a[i-1] == b[j-1]:
                dp[i][j] = dp[i-1][j-1] + 1
            else:
                dp[i][j] = max(dp[i-1][j], dp[i][j-1])
    return dp[m][n]
```

### 9.3 编辑距离（双序列二维，三种操作）
```python
# LeetCode 72
def minDistance(a, b):
    m, n = len(a), len(b)
    dp = [[0]*(n+1) for _ in range(m+1)]
    for i in range(m+1): dp[i][0] = i      # 删
    for j in range(n+1): dp[0][j] = j      # 插
    for i in range(1, m+1):
        for j in range(1, n+1):
            if a[i-1] == b[j-1]:
                dp[i][j] = dp[i-1][j-1]
            else:
                dp[i][j] = 1 + min(dp[i-1][j],    # 删
                                   dp[i][j-1],    # 插
                                   dp[i-1][j-1])  # 改
    return dp[m][n]
```

### 9.4 股票买卖（天数 × 状态 × 次数，三维）
```python
# LeetCode 188 最多 k 笔交易
def maxProfit(k, prices):
    if not prices: return 0
    n = len(prices)
    # dp[j][0/1] = 第 i 天、已用 j 次买入机会、持/不持 的最大利润
    buy  = [-float('inf')] * (k+1)
    sell = [0] * (k+1)
    for p in prices:
        for j in range(1, k+1):
            buy[j]  = max(buy[j], sell[j-1] - p)   # 第 j 次买入
            sell[j] = max(sell[j], buy[j] + p)     # 第 j 次卖出
    return sell[k]
```

### 9.5 打家劫舍 III / 不同路径 II 等
- 不同路径含障碍：障碍格 `dp=0`。
- 打家劫舍：`dp[i] = max(dp[i-1], dp[i-2] + nums[i])`（一维），环形/树形升维。

---

## 10. 状态压缩 DP

当某一维状态是**集合**（每个元素选/不选），且集合大小不大（通常 ≤ 20），用二进制整数表示集合，即"状压 DP"。

### 10.1 位运算基础
```python
mask | (1 << i)        # 把第 i 位置 1（加入元素 i）
mask & ~(1 << i)       # 把第 i 位置 0（移除元素 i）
mask & (1 << i)        # 判断第 i 位是否为 1
bin(mask).count('1')   # 集合元素个数（popcount）
for sub in range(mask): sub = (sub-1) & mask   # 枚举子集
```

### 10.2 经典：TSP 旅行商（状压 + 二维）
`dp[mask][i]` = 已访问集合为 mask、当前停在城市 i 的最短路径。
```python
def tsp(dist):
    n = len(dist)
    INF = float('inf')
    dp = [[INF]*n for _ in range(1<<n)]
    dp[1][0] = 0                              # 从城市0出发
    for mask in range(1<<n):
        for i in range(n):
            if not (mask & (1<<i)): continue
            if dp[mask][i] == INF: continue
            for j in range(n):
                if mask & (1<<j): continue   # j 已访问
                nm = mask | (1<<j)
                dp[nm][j] = min(dp[nm][j], dp[mask][i] + dist[i][j])
    return min(dp[(1<<n)-1][i] + dist[i][0] for i in range(n))
```
复杂度 `O(2^n · n^2)`。

### 10.3 经典：棋盘类（按行状压）
如"种花/放国王/玉米田"：`dp[i][state]` 表示第 i 行摆放状态为 state 时的方案数，转移枚举上一行的合法 state，检查行内/行间冲突。

### 10.4 子集和划分（状压枚举子集）
"火柴拼正方形""划分 k 个相等子集"等，用 `dp[mask]` 记录用了哪些元素是否合法。

**何时想到状压**：数据范围出现 `n ≤ 20`、需要记录"哪些已用/已访问"的集合、且无法用普通维度压缩时。

---

## 11. 其它高维 DP 概览

### 区间 DP
`dp[i][j]` = 区间 [i,j] 的最优解。枚举分割点 k。**遍历顺序：按区间长度从小到大**。
```python
# 戳气球 LeetCode 312
def maxCoins(nums):
    a = [1] + nums + [1]
    n = len(a)
    dp = [[0]*n for _ in range(n)]
    for length in range(2, n):           # 区间长度
        for i in range(n - length):
            j = i + length
            for k in range(i+1, j):      # 最后戳破的气球 k
                dp[i][j] = max(dp[i][j],
                               dp[i][k] + a[i]*a[k]*a[j] + dp[k][j])
    return dp[0][n-1]
```
典型题：石子合并、最长回文子序列、矩阵链乘法。

### 树形 DP
`dp[node][状态]`，通过 DFS 后序自底向上合并子树信息。典型：树上打家劫舍（`dp[u][0/1]` 偷/不偷当前节点）、树的直径、树上背包（把容量当一维，子树合并是分组背包）。

### 数位 DP
按数位逐位填，状态含：当前位、是否贴上界 `limit`、前导零 `lead`、以及题目相关约束。用记忆化搜索写最清晰。典型：统计区间内满足某性质的数的个数。

---

## 12. 优化技巧总览

| 技巧 | 适用 | 效果 |
|---|---|---|
| 滚动数组 | 状态只依赖上一层 | 空间降一维 |
| 一维倒序/正序 | 0-1/完全背包 | 空间 O(W) |
| 二进制拆分 | 多重背包 | 时间 W·Σlog c |
| 单调队列 | 多重背包/滑窗最值 | 时间 O(nW) |
| 状态压缩 | 集合状态 n≤20 | 把集合塞进一维 |
| 记忆化搜索 | 状态稀疏/转移复杂 | 只算到达的状态 |
| 前缀和/差分 | 区间求和类转移 | 转移降复杂度 |
| 斜率优化/四边形不等式 | 特定凸性转移 | O(n²)→O(n)/O(nlogn) |

---

## 13. 面试速查与决策树

### 背包类型判别
```
每件物品能取几次？
├─ 恰好一次 ............ 0-1 背包（一维倒序）
├─ 无限次 .............. 完全背包（一维正序）
├─ 有限 c 次 ........... 多重背包（二进制/单调队列）
├─ 每组选一件 .......... 分组背包（容量在组内物品外层、倒序）
└─ 多种代价约束 ........ 二维费用背包（每维各自方向遍历）

求什么？
├─ 最大/最小价值 ....... max/min，两层循环顺序自由（守容量方向）
├─ 能否装满 ............ 布尔 dp，或 -inf 初始化
├─ 方案数（组合）...... +=，物品外层、容量内层
├─ 方案数（排列）...... +=，容量外层、物品内层
└─ 最少物品数 ......... min + 1，INF 初始化
```

### 高频必背口诀
1. **0-1 倒序、完全正序**——一维数组的容量遍历方向。
2. **组合数物品外、排列数容量外**。
3. **分组背包：容量循环在组内物品之上且倒序**。
4. **多一个约束 → 多一维状态**（二维费用背包/股票次数 k）。
5. **DP 五步**：状态定义 → 转移方程 → 初始化 → 遍历顺序 → 返回值。
6. **滚动数组**降维前先想清楚依赖来自上一层还是本层。

### 必刷题清单
- 0-1：LC 416 分割等和子集、LC 1049 最后一块石头
- 完全：LC 322 零钱兑换、LC 279 完全平方数
- 方案数：LC 518 / LC 377
- 二维费用：LC 474 一和零
- 多维：LC 64 / 1143 / 72 / 188
- 状压：LC 847 / 698 / 1349
- 区间：LC 312 / 516 / 1000
- 树形：LC 337 / 124

> **核心心法**：背包是"在容量约束下做选择"的 DP；多维 DP 是"每多一个独立约束/上下文就多一维状态"。先把状态定义写对（含义无歧义），转移方程往往自然浮现。
