# myfreeRTOS
- 制作一个RTOS内核，主要参考野火的《FreeRTOS内核实现与应用开发》

- 书里代码有些细节没有给出（没什么大碍），在代码实现里已经加上，丰富了一下注释

## 实验第一部分

- 实现任务的轮流切换，通过SV中断与PendSV中断进行两个任务的调度。是人为设置的轮流切换，还没有加上优先级功能。
- 使用keil对Cortex-M进行仿真，可以查看各种寄存器的状态，曲线就是直线任务的函数在反转标志位

![](https://github.com/Winston9n78/myfreeRTOSREADME.assets/keil截图-16520289396441.png)
