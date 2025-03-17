# 调试

BUG[Closed]: 程序在-o3优化下则会异常终止。 Killed. 或者出现 malloc(): unaligned tcache chunk detected. segment fault.（3.11）
DEBUG: fUMap加锁
1. 进入 debug 模式，没有查找设备在训练集突发，程序正常运行。 12：55
2. Deepseek 建议： 将 num_threads 设为 1，观察是否仍崩溃。若问题消失，则确认是多线程竞争导致.
3. 确实在 num_threads 设置为1之后，没有崩溃发生。 14:11
4. 加入Address AddressSanitizer, 但是没出现异常, 出现了几处内存泄漏。发生在 fUmap 结构。
5. 按照 deepseek 的建议, 使用 try_emplace, reserve 进行修正。(这里是问题发生点，但是修复的方式不正确)
6. 再试, 仍然存在内存泄露。 尝试使用valgrind，但是太慢遂放弃。
7. std::cout 排查，发现加入多个 std::cout 问题消失。（std::cout 耗时导致多线程竞争状态消失）
8. 看到知乎说在多线程抱怨 debug, release 设置的问题，要去加锁！在fUMap加锁，问题解决。

BUG[Pending]: 程序在我进行 reviewBook 查询最近的正确的训练集样本报告 Killed 错误(3.12)
DEBUG:
1. 采用MVP理论，使用 UNSW201620 代替 UNSW2016, 发现又没有什么问题了。


# 改进

OBSERVATION: 
* Nest Dropcam	被分到了 Dropcam 当中.
* 有一些零星的错误.
* 99.87和我写论文的时候的准确率差不多.
WANT: 我想要探究为什么会出错
DECISION: 修改代码，将记录所有错误改为只记录isWrong = 1的条目。
FURTHER: 查看reviewBook, 观察发生了什么错误。
我应该详细记录每次运行的配置信息，并且和输出的结果一同记录下来。方便以后查询。

* 应该在训练集就把这些按照一从从一簇簇进行划分。
* 训练集合的压缩还是不够精准。
* 目前的压缩是把非常相似的放在了一起，但是有一些是相似但不完全相似的一类呢。
有很大随机性质的group，可能就需要使用某种规则。


# ReviewBook 观察
Triby Speaker 错误分类：
现象：找到相同的key但是不同的出现次数，距离相差非常少。
改进方法：相同的key的可以先纳入计算。

HP Printer/ Nest Dropcam 错误分类：
现象：由于压缩了训练集，导致最终KNN的结果出错。但是这一例中distance > 0.5, 并且突发的报文数量多，在200-300

LiFx Bulb: 一类的，出现了新的特性，导致和原有的都匹配不上，distance > 0.5
改进方法：只差0.08以内的都还可以算是挽救一下，distance > 0.5 别救了。


# 投稿
ACM WiSec [30%]
ICCC [?]

