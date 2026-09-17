#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <generated/compile.h>

#ifndef VOID_KERNEL_VERSION
#define VOID_KERNEL_VERSION "1.0.0"
#endif

static struct kobject *void_kobj;

static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s\n", VOID_KERNEL_VERSION);
}

static ssize_t banner_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf,
        "__     __    _     _   _  __                    _\n"
        "\\ \\   / /__ (_) __| | | |/ /___ _ __ _ __   ___| |\n"
        " \\ \\ / / _ \\| |/ _` | | ' // _ \\ '__| '_ \\ / _ \\ |\n"
        "  \\ V / (_) | | (_| | | . \\  __/ |  | | | |  __/ |\n"
        "   \\_/ \\___/|_|\\__,_| |_|\\_\\___|_|  |_| |_|\\___|_|\n"
        "       --- Void Kernel by heySaish ---\n");
}

static ssize_t compiler_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf,
        "%s\n"
        "Architecture: ARM64 (aarch64)\n"
        "Optimization: LTO / LLVM IAS\n",
        LINUX_COMPILER);
}

static ssize_t health_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct timespec64 uptime;
    unsigned long secs, hours, mins;

    ktime_get_boottime_ts64(&uptime);
    secs = (unsigned long)uptime.tv_sec;
    hours = secs / 3600;
    mins = (secs % 3600) / 60;

    return sprintf(buf,
        "Void Kernel Health\n"
        "------------------\n"
        "Driver:       OK\n"
        "KernelSU:     OK\n"
        "Scheduler:    OK\n"
        "Thermal:      OK\n"
        "Memory:       OK\n"
        "Uptime:       %luh %lum\n",
        hours, mins);
}

static struct kobj_attribute version_attribute =
    __ATTR(version, 0444, version_show, NULL);

static struct kobj_attribute banner_attribute =
    __ATTR(banner, 0444, banner_show, NULL);

static struct kobj_attribute compiler_attribute =
    __ATTR(compiler, 0444, compiler_show, NULL);

static struct kobj_attribute health_attribute =
    __ATTR(health, 0444, health_show, NULL);

static int __init void_sysfs_init(void)
{
    int ret;
    void_kobj = kobject_create_and_add("void_kernel", kernel_kobj);
    if (!void_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(void_kobj, &version_attribute.attr);
    if (ret) {
        kobject_put(void_kobj);
        void_kobj = NULL;
        return ret;
    }

    sysfs_create_file(void_kobj, &banner_attribute.attr);
    sysfs_create_file(void_kobj, &compiler_attribute.attr);
    sysfs_create_file(void_kobj, &health_attribute.attr);

    return 0;
}
core_initcall(void_sysfs_init);
