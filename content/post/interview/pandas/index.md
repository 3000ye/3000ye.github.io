---
title: Pandas for Quant Data Process
description: 
date: 2026-06-01T01:44:26+08:00
# image: assets/gitssh.png
math: true
toc: true
tags:
    - Quant
    - Python
categories:
    - Interview
---

# 量化数据处理中的 Pandas 完整手册

> 面向量化研究 / 数据工程面试的实战速查与原理手册。覆盖数据结构、读写、清洗、索引、时间序列、分组聚合、滚动窗口、合并对齐、性能优化与典型量化场景。

---

## 目录

1. [核心数据结构](#1-核心数据结构)
2. [数据读写 I/O](#2-数据读写-io)
3. [查看与基础信息](#3-查看与基础信息)
4. [索引与选择 loc/iloc](#4-索引与选择-lociloc)
5. [数据清洗与缺失值](#5-数据清洗与缺失值)
6. [数据类型与内存优化](#6-数据类型与内存优化)
7. [向量化运算与 apply/map](#7-向量化运算与-applymap)
8. [时间序列](#8-时间序列)
9. [分组聚合 groupby](#9-分组聚合-groupby)
10. [滚动与扩展窗口](#10-滚动与扩展窗口)
11. [合并、连接与对齐](#11-合并连接与对齐)
12. [重塑：pivot/melt/stack](#12-重塑-pivotmeltstack)
13. [MultiIndex 多层索引](#13-multiindex-多层索引)
14. [性能优化](#14-性能优化)
15. [量化典型场景实战](#15-量化典型场景实战)
16. [常见面试问答](#16-常见面试问答)

---

## 1. 核心数据结构

### Series
一维带标签数组，量化中常用于单只标的的价格 / 收益率序列。

```python
import pandas as pd
import numpy as np

s = pd.Series([100.2, 101.5, 99.8], index=pd.to_datetime(['2024-01-01','2024-01-02','2024-01-03']), name='close')
s.values      # 底层 numpy 数组
s.index       # 索引对象
s.dtype       # float64
s.name        # 'close'
```

### DataFrame
二维表格，量化中是行情面板（行=时间，列=字段或标的）。

```python
df = pd.DataFrame({
    'open':  [100, 101, 102],
    'high':  [103, 104, 105],
    'low':   [ 99, 100, 101],
    'close': [102, 103, 104],
    'volume':[1e6, 1.2e6, 0.9e6],
}, index=pd.date_range('2024-01-01', periods=3, freq='D'))

df.shape       # (3, 5)
df.columns     # Index(['open','high','low','close','volume'])
df.dtypes      # 每列类型
df.index       # DatetimeIndex
```

**面试要点**：Series 是 DataFrame 的列；二者共享 Index 体系。Index 不可变（immutable），这使其可被安全哈希与对齐。

---

## 2. 数据读写 I/O

```python
# CSV —— 量化最常见
df = pd.read_csv('bars.csv',
    parse_dates=['datetime'],     # 直接解析为时间
    index_col='datetime',         # 设为索引
    dtype={'symbol': 'category'}, # 节省内存
    usecols=['datetime','symbol','close','volume'])

df.to_csv('out.csv')

# Parquet —— 列式存储，量化推荐（快、省、保留 dtype）
df.to_parquet('bars.parquet', compression='snappy')
df = pd.read_parquet('bars.parquet', columns=['close','volume'])

# HDF5 —— 大规模时序高性能读写
df.to_hdf('store.h5', key='bars', format='table', mode='w')
df = pd.read_hdf('store.h5', 'bars', where='index > "2024-01-01"')

# Feather —— 快速跨语言交换
df.to_feather('bars.feather')

# 数据库
import sqlalchemy
engine = sqlalchemy.create_engine('postgresql://...')
df = pd.read_sql('SELECT * FROM bars WHERE symbol=%(s)s', engine, params={'s':'AAPL'})

# Excel（研报/手工数据）
df = pd.read_excel('factor.xlsx', sheet_name='daily')
```

**面试要点**：
- Parquet/Feather 保留 dtype 且压缩，远优于 CSV，是因子库与行情库首选。
- `read_csv` 大文件用 `chunksize` 分块或 `dtype` 预指定避免推断开销。
- HDF5 `format='table'` 支持磁盘端查询（`where`），适合超内存数据集。

---

## 3. 查看与基础信息

```python
df.head(5); df.tail(5); df.sample(5)
df.info(memory_usage='deep')   # 类型 + 真实内存
df.describe()                  # 数值统计
df.describe(include='all')     # 含非数值
df.memory_usage(deep=True)     # 每列内存
df.nunique(); df['symbol'].value_counts()
df.isna().sum()                # 每列缺失数
df.duplicated().sum()          # 重复行数
```

---

## 4. 索引与选择 loc/iloc

```python
# 标签索引 loc（含端点）
df.loc['2024-01-02']                 # 一行
df.loc['2024-01-01':'2024-01-03']    # 时间切片，闭区间
df.loc[:, ['close','volume']]        # 选列
df.loc[df['close'] > 102, 'close']   # 布尔 + 列

# 位置索引 iloc（不含右端点）
df.iloc[0]          # 第一行
df.iloc[-5:]        # 最后5行
df.iloc[0:3, 0:2]   # 行列位置切片

# 标量快速访问
df.at['2024-01-01', 'close']   # 标签标量，比 loc 快
df.iat[0, 3]                   # 位置标量

# 布尔过滤（量化选股核心）
df[(df['close'] > df['open']) & (df['volume'] > 1e6)]
df[df['symbol'].isin(['AAPL','MSFT'])]
df.query('close > open and volume > 1e6')   # 可读性更好

# where / mask
df['close'].where(df['close'] > 0, np.nan)  # 不满足置 NaN
```

**面试要点（高频陷阱）**：
- **链式赋值 SettingWithCopyWarning**：`df[df.a>0]['b'] = 1` 不可靠，可能改不到原对象。正确写法 `df.loc[df.a>0, 'b'] = 1`。
- `loc` 用标签且**包含**右端点；`iloc` 用位置且**不含**右端点。
- `[]` 单列返回 Series，`[['col']]` 返回 DataFrame。
- `query`/`eval` 在大数据上可用 numexpr 加速且省内存。

---

## 5. 数据清洗与缺失值

```python
# 缺失检测
df.isna(); df.notna(); df.isna().any(axis=1)

# 填充
df.fillna(0)
df['close'].ffill()        # 前向填充（价格停牌常用）
df['close'].bfill()        # 后向填充
df.fillna(df.mean())       # 均值填充
df['close'].interpolate(method='time')  # 时间插值

# 删除
df.dropna()                       # 删含NaN行
df.dropna(subset=['close'])       # 仅按某列
df.dropna(thresh=3)               # 至少3个非空才保留
df.dropna(axis=1, how='all')      # 删全空列

# 去重
df.drop_duplicates(subset=['symbol','datetime'], keep='last')

# 替换 / 裁剪（去极值）
df['ret'].replace([np.inf, -np.inf], np.nan)
df['ret'].clip(lower=-0.1, upper=0.1)   # 收益率截断

# 重命名 / 排序
df.rename(columns={'close':'px'})
df.sort_index()                          # 按时间排序
df.sort_values(['symbol','datetime'])
df.reset_index(); df.set_index('datetime')
```

**面试要点**：
- 价格序列停牌一般 `ffill`；**收益率不可 ffill**（会凭空造收益），应填 0 或保留 NaN。
- 去极值常用：截断（clip/分位数 winsorize）或 MAD（中位数绝对偏差）。
- `inplace=True` 已不推荐（不省内存且影响链式），新版趋向返回新对象。

```python
# 因子去极值：分位数 winsorize
def winsorize(s, lower=0.01, upper=0.99):
    lo, hi = s.quantile([lower, upper])
    return s.clip(lo, hi)

# MAD 去极值
def mad_clip(s, n=3):
    med = s.median()
    mad = (s - med).abs().median()
    return s.clip(med - n*1.4826*mad, med + n*1.4826*mad)
```

---

## 6. 数据类型与内存优化

```python
df['symbol'] = df['symbol'].astype('category')   # 字符串重复多 → 大幅省内存
df['volume'] = pd.to_numeric(df['volume'], downcast='integer')
df['ret']    = pd.to_numeric(df['ret'], downcast='float')   # float64→float32
df['dt']     = pd.to_datetime(df['dt'])

# 自动降型函数
def reduce_mem(df):
    for c in df.select_dtypes('integer'):
        df[c] = pd.to_numeric(df[c], downcast='integer')
    for c in df.select_dtypes('float'):
        df[c] = pd.to_numeric(df[c], downcast='float')
    for c in df.select_dtypes('object'):
        if df[c].nunique() / len(df) < 0.5:
            df[c] = df[c].astype('category')
    return df
```

**面试要点**：category 对低基数高重复列（股票代码、行业、交易所）极有效；float32 在因子值上通常够用，内存减半。

---

## 7. 向量化运算与 apply/map

```python
# 向量化（首选，C 层执行）
df['ret']     = df['close'].pct_change()
df['log_ret'] = np.log(df['close']).diff()
df['range']   = df['high'] - df['low']
df['vwap']    = (df['close'] * df['volume']).cumsum() / df['volume'].cumsum()

# 累计
df['cum_ret'] = (1 + df['ret']).cumprod() - 1
df['close'].cummax(); df['close'].cummin()

# map：Series 元素映射
df['sector'] = df['symbol'].map(symbol2sector)

# apply：按行/列
df.apply(np.sum, axis=0)             # 列方向
df.apply(lambda r: r['high']-r['low'], axis=1)  # 行方向（慢，尽量避免）

# applymap（元素级，已更名 map for DataFrame）
df[['open','close']].apply(lambda x: x.round(2))

# np.where / np.select 条件向量化
df['signal'] = np.where(df['ret'] > 0, 1, -1)
df['grade'] = np.select(
    [df['ret']>0.05, df['ret']>0, df['ret']<=0],
    ['strong','up','down'], default='na')
```

**面试要点（性能金句）**：优先级 **向量化 > apply > 显式循环**。`apply(axis=1)` 本质是 Python 循环，万行以上明显变慢；能用 numpy 向量化就别用 apply。

---

## 8. 时间序列

```python
idx = pd.date_range('2024-01-01', periods=100, freq='B')  # B=工作日

# 重采样 resample（降频/升频）
df.resample('W').last()              # 周线收盘
df.resample('M').agg({'open':'first','high':'max','low':'min','close':'last','volume':'sum'})
df.resample('5min').ohlc()          # tick→5分钟K线
df.resample('D').ffill()            # 升频前向填充

# 频率转换
df.asfreq('B', method='ffill')      # 补齐工作日

# 移动 / 滞后（因子常用）
df['close'].shift(1)                # 滞后1期（避免未来函数）
df['close'].shift(-1)              # 前移（构造标签 forward return）
df['close'].diff(1)                # 一阶差分

# 时间属性
df.index.year; df.index.month; df.index.dayofweek
df.index.is_month_end
df.between_time('09:30','11:30')    # 日内时段
df.tz_localize('Asia/Shanghai').tz_convert('UTC')  # 时区

# 偏移
from pandas.tseries.offsets import BDay, MonthEnd
df.index + BDay(1)
```

**面试要点（极重要）**：
- **未来函数 / look-ahead bias**：计算 t 时刻信号只能用 ≤t 的数据。用 `shift(1)` 把因子滞后一期再与未来收益对齐，是回测防偷看的关键。
- resample 默认右闭右标签依频率而定，做 K 线合成要确认 `label` / `closed` 参数。
- tick 合成 K 线：`resample('1min').ohlc()` + volume `.sum()`。

```python
# 构造前瞻收益作为标签（label），与当期因子对齐
df['fwd_ret_1d'] = df['close'].shift(-1) / df['close'] - 1
df['factor_lag'] = df['factor'].shift(1)   # 因子滞后，防未来函数
```

---

## 9. 分组聚合 groupby

```python
g = df.groupby('symbol')

g['close'].mean()
g.agg({'ret':['mean','std'], 'volume':'sum'})
g['ret'].agg(sharpe=lambda x: x.mean()/x.std()*np.sqrt(252))  # 命名聚合

# transform：返回与原索引对齐的结果（截面标准化核心）
df['z'] = df.groupby('date')['factor'].transform(lambda x: (x - x.mean())/x.std())

# filter：按组条件筛选
df.groupby('symbol').filter(lambda x: len(x) > 100)

# apply：最灵活，返回任意结构
df.groupby('symbol').apply(lambda x: x.assign(rank=x['factor'].rank()))

# 分组排名（选股打分常用）
df['rank'] = df.groupby('date')['factor'].rank(ascending=False, pct=True)

# 分箱分组（分层回测）
df['quantile'] = df.groupby('date')['factor'].transform(
    lambda x: pd.qcut(x, 5, labels=False, duplicates='drop'))
```

**面试要点（agg vs transform vs apply）**：
- `agg`：聚合，每组返回**标量** → 行数变少。
- `transform`：每组返回**同长度**序列 → 行数不变，自动对齐，适合截面去均值/标准化。
- `apply`：最通用但最慢，返回结构灵活。
- 截面（cross-section）操作几乎都按 `date` 分组用 `transform`/`rank`/`qcut`。

```python
# 分层回测：每日按因子分5组，看各组未来收益
layered = (df.groupby(['date','quantile'])['fwd_ret_1d'].mean()
             .unstack('quantile'))
layered.cumsum().plot()  # 多空分层净值
```

---

## 10. 滚动与扩展窗口

```python
# rolling 移动窗口
df['ma20']  = df['close'].rolling(20).mean()             # 20日均线
df['std20'] = df['close'].rolling(20).std()
df['ma20'].rolling(window=20, min_periods=5).mean()      # 容忍部分缺失

# 滚动相关 / 协方差（配对交易、beta）
df['corr'] = df['x'].rolling(60).corr(df['y'])
beta = df['ret'].rolling(60).cov(mkt['ret']) / mkt['ret'].rolling(60).var()

# 自定义滚动函数 apply（慢，可加 raw=True 提速）
df['zscore'] = df['close'].rolling(20).apply(
    lambda x: (x[-1]-x.mean())/x.std(), raw=True)

# 指数加权 EWM（衰减权重）
df['ema'] = df['close'].ewm(span=20, adjust=False).mean()
df['ewm_vol'] = df['ret'].ewm(span=60).std()

# expanding 扩展窗口（从头到当前）
df['cummax'] = df['close'].expanding().max()
df['drawdown'] = df['close']/df['close'].expanding().max() - 1   # 回撤

# 基于时间的窗口（不规则间隔）
df['close'].rolling('30D').mean()   # 30个日历日

# 滚动窗口高级：rolling().agg 多函数
df['close'].rolling(20).agg(['mean','std','min','max'])
```

**面试要点**：
- `min_periods` 控制窗口前期 NaN，回测中要谨慎避免引入偏差。
- 自定义 `rolling.apply` 设 `raw=True` 传入 numpy 数组，速度快很多。
- 最大回撤标准算法 = `(净值 / 净值累计最大值 - 1).min()`。
- EWM 用于波动率估计（RiskMetrics）和自适应均线。

```python
# 最大回撤
def max_drawdown(nav):
    return (nav / nav.cummax() - 1).min()

# 滚动夏普
def rolling_sharpe(ret, window=252):
    return ret.rolling(window).mean() / ret.rolling(window).std() * np.sqrt(252)
```

---

## 11. 合并、连接与对齐

```python
# merge（类 SQL join）
pd.merge(prices, fundamentals, on=['symbol','date'], how='left')
pd.merge(a, b, left_on='code', right_on='symbol', how='inner')

# join（按索引）
prices.join(sector_map, on='symbol')

# concat（堆叠）
pd.concat([df1, df2], axis=0)        # 纵向：拼接多只标的/多日
pd.concat([px, vol], axis=1)         # 横向：拼接多字段
pd.concat([d1,d2], keys=['a','b'])   # 生成 MultiIndex

# merge_asof —— 量化神器（按最近时间对齐）
pd.merge_asof(trades, quotes, on='time',
              by='symbol', direction='backward',
              tolerance=pd.Timedelta('1s'))

# 索引对齐运算（自动对齐，缺失→NaN）
df_a['close'] + df_b['close']        # 按索引自动对齐
df.align(other, join='inner', axis=0)
```

**面试要点**：
- `merge_asof` 用于把成交（trades）匹配到最近的报价（quotes），或低频因子对齐到高频价格，是高频/事件数据对齐的核心工具。
- concat 纵向拼接注意 `ignore_index` 与索引重复问题。
- Pandas 算术运算**自动按索引对齐**，这是与 numpy 的关键区别——也是隐藏 bug 源（错位求和）。

---

## 12. 重塑 pivot/melt/stack

```python
# 长 → 宽：pivot（量化常用：行=日期，列=股票，值=收盘价）
wide = df.pivot(index='date', columns='symbol', values='close')

# pivot_table（可聚合，处理重复）
pd.pivot_table(df, index='date', columns='sector',
               values='ret', aggfunc='mean')

# 宽 → 长：melt
long = wide.reset_index().melt(id_vars='date',
        var_name='symbol', value_name='close')

# stack / unstack（多层索引转换）
df.stack()      # 列 → 行（变长）
df.unstack()    # 行 → 列（变宽）
panel.unstack('symbol')

# crosstab 频数交叉表
pd.crosstab(df['sector'], df['signal'])

# explode 展开列表列
df.explode('holdings')
```

**面试要点**：量化中典型数据形态是 **长表（tidy：date,symbol,factor）** 与 **宽表（行日期×列股票）** 互转。回测用宽表做矩阵运算高效；存储与多因子合并用长表灵活。`pivot` 要求无重复键，有重复用 `pivot_table`。

---

## 13. MultiIndex 多层索引

```python
df = df.set_index(['date','symbol']).sort_index()  # 面板数据标准结构

df.loc['2024-01-01']                  # 选某天所有股票
df.loc[('2024-01-01','AAPL')]         # 精确定位
df.loc[(slice(None),'AAPL'),:]        # 所有日期的AAPL
idx = pd.IndexSlice
df.loc[idx['2024-01':'2024-03', ['AAPL','MSFT']], :]

df.xs('AAPL', level='symbol')         # 横切某层
df.groupby(level='date')['factor'].rank()   # 按层分组（截面）
df.unstack('symbol')                  # 转宽表
df.swaplevel().sort_index()           # 交换层级
df.reset_index(level='symbol')
```

**面试要点**：(date, symbol) 双层索引是面板数据事实标准。**务必先 `sort_index()`**，否则切片报 `UnsortedIndexError` 且性能差。截面操作 = `groupby(level='date')`，时序操作 = `groupby(level='symbol')`。

---

## 14. 性能优化

```python
# 1. 向量化优先，避免 iterrows
for i, row in df.iterrows():   # ❌ 极慢
    ...
df['x'] = df['a'] * df['b']    # ✅

# 2. itertuples 比 iterrows 快很多（非得循环时）
for row in df.itertuples():
    ...

# 3. eval / query 用 numexpr 加速大表表达式
df.eval('mid = (high + low) / 2', inplace=True)
df.query('volume > 1e6 and close > open')

# 4. 选对 dtype（category / float32 / 见第6节）

# 5. 避免在循环里 concat / append（O(n²)）
parts = [...]; df = pd.concat(parts)   # ✅ 一次性拼

# 6. 用 numpy 底层数组做核心计算
arr = df['close'].to_numpy()

# 7. 大数据：分块读 + Parquet 列裁剪 + 只读需要的列
pd.read_parquet('big.parquet', columns=['close'])

# 8. 超内存：Polars / Dask / DuckDB 替代
import duckdb
duckdb.query("SELECT symbol, mean(ret) FROM 'bars.parquet' GROUP BY symbol").df()
```

**性能对比直觉**：iterrows ≪ apply(axis=1) < itertuples ≪ 向量化 ≈ numpy。万行级别向量化通常比 iterrows 快 100~1000 倍。

**面试要点**：
- 知道为什么慢：Pandas 逐行操作要走 Python 对象层，丧失 numpy 的 C 连续内存与 SIMD。
- 知道何时换工具：单机超内存上 Polars（多线程、惰性）、DuckDB（SQL on parquet）、Dask（分布式）。

---

## 15. 量化典型场景实战

### 15.1 行情清洗与收益率
```python
df = (pd.read_parquet('bars.parquet')
        .sort_values(['symbol','datetime'])
        .drop_duplicates(['symbol','datetime']))
df['ret'] = df.groupby('symbol')['close'].pct_change()
df['log_ret'] = df.groupby('symbol')['close'].transform(lambda x: np.log(x).diff())
```

### 15.2 计算技术指标
```python
# 均线、布林带
df['ma20'] = df.groupby('symbol')['close'].transform(lambda x: x.rolling(20).mean())
df['std20']= df.groupby('symbol')['close'].transform(lambda x: x.rolling(20).std())
df['boll_up'] = df['ma20'] + 2*df['std20']
df['boll_dn'] = df['ma20'] - 2*df['std20']

# RSI
def rsi(close, n=14):
    d = close.diff()
    up = d.clip(lower=0).rolling(n).mean()
    dn = (-d.clip(upper=0)).rolling(n).mean()
    return 100 - 100/(1 + up/dn)
df['rsi'] = df.groupby('symbol')['close'].transform(rsi)

# MACD
def macd(close, fast=12, slow=26, signal=9):
    ema_f = close.ewm(span=fast, adjust=False).mean()
    ema_s = close.ewm(span=slow, adjust=False).mean()
    dif = ema_f - ema_s
    dea = dif.ewm(span=signal, adjust=False).mean()
    return dif, dea, (dif-dea)*2
```

### 15.3 因子标准化与中性化
```python
# 截面 z-score 标准化
df['factor_z'] = df.groupby('date')['factor'].transform(lambda x: (x-x.mean())/x.std())

# 截面排序标准化（更稳健，抗极值）
df['factor_rank'] = df.groupby('date')['factor'].rank(pct=True)

# 行业中性化（去行业均值）
df['factor_neutral'] = df['factor_z'] - df.groupby(['date','sector'])['factor_z'].transform('mean')
```

### 15.4 IC 计算（因子有效性）
```python
# IC = 因子值与未来收益的截面相关
df['fwd_ret'] = df.groupby('symbol')['close'].shift(-1) / df['close'] - 1
ic = df.groupby('date').apply(lambda x: x['factor_z'].corr(x['fwd_ret']))
ic_mean = ic.mean()
icir = ic.mean() / ic.std()          # IC_IR
rank_ic = df.groupby('date').apply(   # RankIC（Spearman）
    lambda x: x['factor_z'].corr(x['fwd_ret'], method='spearman'))
```

### 15.5 分层回测与多空净值
```python
df['grp'] = df.groupby('date')['factor_z'].transform(
    lambda x: pd.qcut(x, 5, labels=False, duplicates='drop'))
grp_ret = df.groupby(['date','grp'])['fwd_ret'].mean().unstack('grp')
ls = grp_ret[4] - grp_ret[0]         # 多空组合日收益
nav = (1 + ls).cumprod()             # 多空净值
ann = nav.iloc[-1]**(252/len(nav)) - 1
sharpe = ls.mean()/ls.std()*np.sqrt(252)
mdd = (nav/nav.cummax()-1).min()
```

### 15.6 组合收益与绩效
```python
weights = pd.DataFrame(...)          # 行日期×列股票
port_ret = (weights.shift(1) * ret_wide).sum(axis=1)   # shift防未来函数
nav = (1+port_ret).cumprod()
turnover = weights.diff().abs().sum(axis=1) / 2          # 换手率
```

### 15.7 Tick 合成 K 线
```python
bars = ticks.set_index('time').groupby('symbol').resample('1min').agg(
    open=('price','first'), high=('price','max'),
    low=('price','min'),   close=('price','last'),
    volume=('size','sum'))
```

**面试金句**：所有信号 / 权重在用于下一期收益前都要 `shift(1)`——这是回测正确性的生命线（防 look-ahead bias）。

---

## 16. 常见面试问答

**Q: loc 和 iloc 的区别？**
loc 基于标签、闭区间；iloc 基于整数位置、左闭右开。

**Q: apply、map、transform、agg 区别？**
map 仅 Series 元素映射；apply 灵活按行/列/组，最慢；transform 返回同长度并对齐，适合截面标准化；agg 聚合成标量。

**Q: 为什么 iterrows 慢？怎么优化？**
逐行生成 Series 对象走 Python 层，丧失向量化。改用向量化运算 / numpy / itertuples / eval。

**Q: SettingWithCopyWarning 是什么？**
链式索引可能操作的是副本，赋值未必落到原对象。用单次 `.loc[行, 列] = 值`。

**Q: 怎么避免回测未来函数？**
因子滞后 `shift(1)`；标签用未来收益 `shift(-1)`；滚动窗口只用历史；resample 注意闭区间；合并时间序列用 merge_asof backward。

**Q: 长表 vs 宽表？**
长表（date,symbol,value）利于存储/合并/多因子；宽表（日期×股票）利于矩阵运算/回测。pivot/melt 互转。

**Q: 缺失值怎么处理（量化语境）？**
价格停牌 ffill；收益率不可 ffill（填0或NaN）；因子缺失按截面中位数/均值填或剔除；插值用 time 方法。

**Q: 怎么处理超内存数据？**
Parquet 列裁剪 + dtype 降型 + 分块；或换 Polars / DuckDB / Dask。

**Q: category 类型何时用？**
低基数高重复的字符串列（股票代码、行业、交易所），大幅省内存且加速 groupby。

**Q: merge_asof 用在哪？**
按最近时间对齐不同频/不对齐的时序：成交配报价、低频因子贴到高频价格。

---

### 复习优先级（面试前一晚）
1. loc/iloc、布尔过滤、SettingWithCopy
2. groupby 的 agg/transform/apply 区别 + 截面 transform
3. shift 防未来函数 + IC/分层回测逻辑
4. resample / rolling / ewm
5. merge_asof + 长宽表互转
6. 向量化 vs apply 性能

> 核心心法：**量化 Pandas = 截面（groupby date + transform/rank）× 时序（groupby symbol + rolling/shift）的二维操作，全程防未来函数。**
