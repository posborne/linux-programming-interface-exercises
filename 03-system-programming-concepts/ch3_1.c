#include <stdio.h>
#include <linux/reboot.h>
#include <unistd.h>

int main()
{
	printf("LINUX_REBOOT_MAGIC1 = 0x%08X (%d)\n", LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC1);
	printf("LINUX_REBOOT_MAGIC2 = 0x%08X (%d)\n", LINUX_REBOOT_MAGIC2, LINUX_REBOOT_MAGIC2);
	return 0;
}
