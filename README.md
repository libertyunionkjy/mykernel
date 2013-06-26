# mykernel

It is a platform to write your own OS kernel,its based on Linux Kernel 3.9.4 source code.

+ Set up this platform
    + install QEMU and download [Linux Kernel 3.9.4 source code](https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.9.3.tar.xz)
    + cd linux-3.9.4
    + patch -p1 < ../[mykernel_for_linux3.9.4sc.patch](https://raw.github.com/mengning/mykernel/master/mykernel_for_linux3.9.4sc.patch)
    + make allnoconfig
    + make
    + qemu -kernel arch/x86/boot/bzImage ��qemu�����������Կ���my_start_kernel��ִ�У�ͬʱmy_timer_handlerʱ���жϴ�������������ִ�С�
    + cd mykernel �����Կ���qemu������������ݵĴ���mymain.c��myinterrupt.c
    + ��ǰ��һ��CPUִ��C����������Ļ�����ͬʱ�����жϴ�������������Ļ��������ǳ�ʼ������ϵͳ������
    + ��ֻҪ��mymain.c�����ϼ���д��������PCB�ͽ������������ȴ��룬��myinterrupt.c�Ļ�������ɽ����л����룬һ�������е�СOS kernel������ˡ�
    + start to write your own OS kernel,injoy it!

+ mykernel patch generated by this command: 
    + diff -Naur linux-3.9.4 linux-3.9.4.new/ > mykernel_for_linux3.9.4sc.patch