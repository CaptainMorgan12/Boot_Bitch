/* Boot Bitch — the Num Lock early-boot micro-binary (0.2.32, phase 3).
 *
 * The shipped build-own payload for the Num Lock early-boot repair
 * (plan-numlock-early-boot-implementation.md §2): ONE ioctl family —
 * KDGKBLED/KDSKBLED on the Linux VT console layer — with no library
 * dependencies beyond libc. It runs at the initrd's earliest
 * deterministic point (the init-top script / the dracut pre-trigger /
 * the mkinitfs init hook / the mkinitcpio hook) and sets Num Lock on
 * every pre-allocated VT explicitly — the per-VT loop is the mechanism;
 * the "-D default" idea was DISPROVEN in the round-21 drill (KDSKBLED
 * sets no kernel default; a freshly allocated VT reads OFF), so this
 * binary NEVER relies on a default and always loops.
 *
 * Behavior contract (the probes' expectations):
 *   numlock-on [-a] [on|off|read] [dev ...]
 *   on|off   set (or clear) LED_NUM on each device via KDSKBLED
 *   read     read each device via KDGKBLED and print on|off
 *   -a       loop over /dev/tty0..tty8 (the init-top mode)
 *   exit 0   every device set/read successfully
 *   exit !=0 at least one device failed (64 = usage)
 * Markers: one stdout line per device (the machine contract:
 *   "boot-bitch: numlock on (/dev/tty0)" / "boot-bitch: numlock read
 *   /dev/tty0: on"), plus ONE /dev/kmsg line per set-mode run
 *   ("boot-bitch: numlock on (marker)") — the kmsg channel is the
 *   serial-capturable evidence (the line lands on every registered
 *   console and in the journal). A direct /dev/ttyS0 write-open is
 *   FORBIDDEN: a phantom 8250 port with no carrier blocks it (the
 *   TUXEDO round-21b hang). EVERY open carries O_NONBLOCK — the
 *   init-top context must never block on a console device. The kmsg
 *   marker is best-effort (its failure never fails the run).
 *
 * MIT (matches the package license field).
 */
#include <errno.h>
#include <fcntl.h>
#include <linux/kd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int read_led(const char *dev)
{
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        printf("boot-bitch: numlock read %s: open-failed (%s)\n", dev,
               strerror(errno));
        return -1;
    }
    unsigned char flags = 0;
    if (ioctl(fd, KDGKBLED, &flags) < 0) {
        printf("boot-bitch: numlock read %s: ioctl-failed (%s)\n", dev,
               strerror(errno));
        close(fd);
        return -1;
    }
    printf("boot-bitch: numlock read %s: %s\n", dev,
           (flags & LED_NUM) ? "on" : "off");
    close(fd);
    return 0;
}

static int set_led(const char *dev, int on)
{
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        printf("boot-bitch: numlock set %s: open-failed (%s)\n", dev,
               strerror(errno));
        return 1;
    }
    unsigned char flags = 0;
    if (ioctl(fd, KDGKBLED, &flags) < 0) {
        printf("boot-bitch: numlock set %s: read-failed (%s)\n", dev,
               strerror(errno));
        close(fd);
        return 2;
    }
    if (on)
        flags |= LED_NUM;
    else
        flags &= (unsigned char)~LED_NUM;
    if (ioctl(fd, KDSKBLED, flags) < 0) {
        printf("boot-bitch: numlock set %s: set-failed (%s)\n", dev,
               strerror(errno));
        close(fd);
        return 3;
    }
    /* The marker goes to the DEVICE fd (the console) as well as stdout —
       at the init-top phase stdout does not reliably reach the serial
       console, while a console-device write lands on every registered
       console (the round-21 Boot-A finding). */
    dprintf(fd, "boot-bitch: numlock %s (%s)\n", on ? "on" : "off", dev);
    printf("boot-bitch: numlock %s (%s)\n", on ? "on" : "off", dev);
    close(fd);
    return 0;
}

/* The serial-capturable kmsg marker: one line per set-mode run, written to
   the kernel message channel — never a direct ttyS0 open (the round-21b
   hang class). Best-effort: an unopenable kmsg never fails the run. */
static void kmsg_marker(const char *mode)
{
    int kfd = open("/dev/kmsg", O_WRONLY | O_NOCTTY | O_NONBLOCK);
    if (kfd < 0)
        return;
    dprintf(kfd, "boot-bitch: numlock %s (marker)\n", mode);
    close(kfd);
}

int main(int argc, char **argv)
{
    int all = 0;
    const char *mode = "on";
    int i = 1;
    for (; i < argc; ++i) {
        if (strcmp(argv[i], "-a") == 0) {
            all = 1;
        } else if (strcmp(argv[i], "on") == 0 ||
                   strcmp(argv[i], "off") == 0 ||
                   strcmp(argv[i], "read") == 0) {
            mode = argv[i];
        } else {
            break;
        }
    }
    static const char *vts[] = { "/dev/tty0", "/dev/tty1", "/dev/tty2",
                                 "/dev/tty3", "/dev/tty4", "/dev/tty5",
                                 "/dev/tty6", "/dev/tty7", "/dev/tty8" };
    int rc = 0;
    if (all) {
        for (size_t k = 0; k < sizeof(vts) / sizeof(vts[0]); ++k) {
            if (strcmp(mode, "read") == 0)
                rc |= (read_led(vts[k]) != 0);
            else
                rc |= set_led(vts[k], strcmp(mode, "on") == 0);
        }
    } else {
        if (i >= argc) {
            fprintf(stderr, "usage: numlock-on [-a] [on|off|read] [dev ...]\n");
            return 64;
        }
        for (; i < argc; ++i) {
            if (strcmp(mode, "read") == 0)
                rc |= (read_led(argv[i]) != 0);
            else
                rc |= set_led(argv[i], strcmp(mode, "on") == 0);
        }
    }
    if (strcmp(mode, "read") != 0)
        kmsg_marker(mode);
    return rc;
}
