# myfreeRTOS
- 学习和尝试写RTOS内核，主要参考野火的《FreeRTOS内核实现与应用开发》
- 书里代码有些细节没有给出（没什么大碍），在代码实现里已经加上，丰富了一下注释
- 主要是想学习和加深对内核的理解
- 双向链表与ARM是基础



## 知识准备



## 实验第一部分

- 实现任务的轮流切换，通过**SV中断**与**PendSV中断**进行两个任务的调度。是人为设置的轮流切换，还没有加上优先级功能。
- 使用keil对Cortex-M进行仿真，可以查看各种寄存器的状态，方波就是任务函数在反转标志位

![实验一](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil%E6%88%AA%E5%9B%BE.png?raw=true)




## 实验第二部分

- 使用阻塞延时代替软件延时，任务都阻塞时执行空闲任务，直到阻塞结束
- 使用systick中断计算阻塞时间
- 暂时没有优先级，在中断中切换任务
- 实验现象如图，两个任务看上去在并行执行

![实验二](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil2.png?raw=true)

待解决bug

- [ ] 空闲任务初始化函数放在task.c中时，任务控制块的栈空间无法分配，寄存器状态显示不可读？通过单步调试发现问题，目前放在main里



## 实验第三部分

- 加入优先级机制

- 通过更新**pxCurrentTCB**指向改变运行任务

- 注重理解

  ```C
  taskSELECT_HIGHEST_PRIORITY_TASK();
  ```

  切换**pxCurrentTCB**部分的逻辑

- 注意

  ```c
  taskRESET_READY_PRIORITY( pxTCB一>uxPriority );
  ```

  目前是将阻塞任务优先级位图标置0，实际上需要从就绪列表去除，在下一章引入延时列表时实现

- 实验现象如图，看起来在同时运行两个任务

![实验三](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil3.png?raw=true)



## 实验第四部分

- 引入任务延时列表
- 实验现象还是和以前一样，不过在阻塞延时时将任务切换到延时列表中

![实验四](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil4.png?raw=true)

## 实验第五部分

- 同一优先级多任务的时间片管理机制

- 在之前优先级中已经实现，这个实验主要去理解原理

  

![实验五](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil5-1.png?raw=true)

![实验五](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil5-2.png?raw=true)



## RTOS内核应用开发学习笔记
