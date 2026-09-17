#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/init.h>

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

static struct kobj_attribute version_attribute =
    __ATTR(version, 0444, version_show, NULL);

static struct kobj_attribute banner_attribute =
    __ATTR(banner, 0444, banner_show, NULL);

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

    ret = sysfs_create_file(void_kobj, &banner_attribute.attr);
    if (ret) {
        // Log or ignore
    }

    return 0;
}
core_initcall(void_sysfs_init);
