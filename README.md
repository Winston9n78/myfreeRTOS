# myfreeRTOS
- 制作一个RTOS内核，主要参考野火的《FreeRTOS内核实现与应用开发》

- 书里代码有些细节没有给出（没什么大碍），在代码实现里已经加上，丰富了一下注释

## 实验第一部分

- 实现任务的轮流切换，通过SV中断与PendSV中断进行两个任务的调度。是人为设置的轮流切换，还没有加上优先级功能。
- 使用keil对Cortex-M进行仿真，可以查看各种寄存器的状态，曲线就是直线任务的函数在反转标志位

![实验一](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil%E6%88%AA%E5%9B%BE.png?raw=true)




## 实验第二部分

- 使用阻塞延时代替软件延时，任务都阻塞时执行空闲任务，直到阻塞结束
- 使用systick中断计算阻塞时间
- 暂时没有优先级，在中断中切换任务
- 实验现象如图，两个任务看上去在并行执行

![实验二](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil2.png?raw=true)



## 实验第三部分

- 加入优先级机制



## 实验第四部分

- 同一优先级多任务的时间片管理机制
