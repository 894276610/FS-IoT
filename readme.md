# 记录调试的过程 

12：55 我发现在 debug 模式下，没有查找设备在训练集突发的情况下，程序会正常运行。
但是在-o3优化下则会异常终止。 Killed.

12:57 我发现 UNSW2016 accuracy 已经达到了 99.87. 我觉得很高。仍有以下问题：

* Nest Dropcam	被分到了 Dropcam 当中.

有一些零星的错误。
我想要探究为什么会出错。
我需要在-o3的情况下也能正常运行。

DOING: DEBUG THE PROGRAM TO RUN IN -O3 MODE.

3月11日
    尝试 review = false 是否能够正常运行。 => 结果是不能正常运行 在训练结束之后killed.
    => 尝试在各处打上std::cout判断究竟是哪段代码的问题。=> 问题没有出现。
    => 再一次运行，malloc(): unaligned tcache chunk detected. 
    => 拷贝到 deepseek 寻求解答。
    => 将 num_threads 设为 1，观察是否仍崩溃。若问题消失，则确认是多线程竞争导致.
    => 确实在 num_threads 设置为1之后，没有崩溃发生，再试一次，确认。 14:11
    => 14:30  确实在 num_threads 设置为1之后，没有崩溃发生
    => 也就是说，在正常的03下，只需要1分钟多一点就可以完成。
    => 可以看看究竟代码哪块出了问题。
    => 加入Address AddressSanitizer, 但是没出现异常, 出现了几处内存泄漏。 
    => 按照 deepseek 的建议, 使用 try_emplace, reserve 进行修正。
    => 再试, 仍然存在内存泄露。 移除address sanitizer 并使用valgrind 测试 valgrind --leak-check=full --track-origins=yes ./your_program，但 valgrindu运行2个小时还没好。放弃！
    => 我想，如果没问题，那就用 -o3直接运行了。

    => 发现是没加 reviewBook, 运行很顺利。
    => 加了 reviewBook, segment fault, 我猜测是UpdateFlags里面出了问题。加了std::cout进行界定。
    => 我服了，在debug模式没有问题。在-o3模式下std::cout多也没问题。把std::cout判断打印删了就报错。怎么调试啊。
    => 看到zhihuer的一句话，说在多线程不要说 debug, release的问题，要去加锁！我立即想到，确实。fUMap没加锁。可能导致竞争状态。
    加上之前 AddressSanitizer 的检查内存泄露也是指向这里。我觉得我悟了。 21：26 （这个问题搞定，今天就值得了）

3月12日
    => 调整代码，看全错的部分。
# 改进实验

99.87很高，我想看看当时我写论文的时候的准确率。

# 投稿
ACM WiSec




