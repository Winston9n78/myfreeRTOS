# myfreeRTOS
- 学习和尝试写RTOS内核，主要参考野火的《FreeRTOS内核实现与应用开发》
- 书里代码有些细节没有给出（没什么大碍），在代码实现里已经加上，丰富了一下注释
- 主要是想学习和加深对内核的理解
- 双向链表与ARM是基础



## 知识准备

### 链表

RTOS将任务连接起来的基础数据结构是双向链表。双向链表中一个节点的内容包括：

- 前向节点指针：指向链表下一个节点
- 后向节点指针：指向列表下一个节点
- 排序辅助值：辅助链表进行排序的值，比如升降序插入节点
- 指向内核对象指针：指向TCB（Task Control Block），即该节点挂载的内容所在，一般一个节点就是由用于连接的指针和挂载的内容组成
- 指向所在链表指针：即指向根节点
- 精简节点：只包含前后向节点和排序辅助值，不包含上面的
- 根节点：只包含精简节点和节点计数器

### 任务

四个的基本元素：

- 栈数组：用于存放和该任务相关的变量常量函数入口地址等
- 栈大小：字面意思，一般用128个字，即512字节
- 任务句柄：void * 类型的变量
- 任务控制块：包含任务名，优先级，栈顶指针，任务栈起始地址，任务名，任务节点，其中任务节点用于指向这个任务挂载在的链表上。
- 任务形参

### ARM

需要用到的Cortex-M3的内容:R0-R15寄存器组。

SV中断:svc 0

PendSV中断:

中断向量

中断屏蔽:basepri



## 实验第一部分

- 实现任务的轮流切换，通过**SV中断**与**PendSV中断**进行两个任务的调度。是人为设置的轮流切换，还没有加上优先级功能。
- 使用keil对Cortex-M进行仿真，可以查看各种寄存器的状态，方波就是任务函数在反转标志位

![实验一](https://github.com/Winston9n78/myfreeRTOS/blob/main/README.assets/keil1.png?raw=true)




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

信号量，消息队列，事件组，软件定时器，内存管理，中断管理
