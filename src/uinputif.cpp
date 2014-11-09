/*
 * (C) 2014 Kimmo Lindholm <kimmo.lindholm@gmail.com> Kimmoli
 *
 * uinput functions
 *
 *
 *
 *
 */

#include "uinputif.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

static const char conf_devname[] = "tohkbd";

int UinputIf::fd = -1;

UinputIf::UinputIf(QObject *parent) :
    QObject(parent)
{
}


/*
 * open and configure uinput device for tohkbd
 *
 */

int UinputIf::openUinputDevice()
{
    int i;
    struct uinput_user_dev uidev;


    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        printf("uinput: error: open\n");
        return false;
    }

    /* Enable EV_KEY events */
    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
    {
        printf("uinput: error: ioctl UI_SET_EVBIT EV_KEY\n");
        return false;
    }

    /* Enable EV_SYN events */
    if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0)
    {
        printf("uinput: error: ioctl UI_SET_EVBIT EV_SYN\n");
        return false;
    }

    /* Enable EV_SW events */
    if (ioctl(fd, UI_SET_EVBIT, EV_SW) < 0)
    {
        printf("uinput: error: ioctl UI_SET_EVBIT EV_SW\n");
        return false;
    }

    /* Enable all keys */
    for (i = KEY_ESC; i < KEY_MAX; i++)
    {
        if (ioctl(fd, UI_SET_KEYBIT, i) < 0)
        {
            printf("uinput: error: ioctl UI_SET_KEYBIT %d\n", i);
            return false;
        }
    }

    /* Enable SW_KEYPAD_SLIDE */
    if (ioctl(fd, UI_SET_SWBIT, SW_KEYPAD_SLIDE) < 0)
    {
        printf("uinput: error: ioctl UI_SET_SWBIT SW_KEYPAD_SLIDE\n");
        return false;
    }

    memset(&uidev, 0, sizeof(uidev));
    strncpy(uidev.name, conf_devname, UINPUT_MAX_NAME_SIZE);
    uidev.id.bustype = BUS_VIRTUAL;
    uidev.id.vendor  = 0x1989;
    uidev.id.product = 0x0003;
    uidev.id.version = 1;

    if (write(fd, &uidev, sizeof(uidev)) < 0)
    {
        printf("uinput: error: write uidev\n");
        return false;
    }

    if (ioctl(fd, UI_DEV_CREATE) < 0)
    {
        printf("uinput: error: ioctl UI_DEV_CREATE\n");
        return false;
    }

    usleep(100000); /* wait magic 100ms here*/

    return true;
}

/*
 * Send EV_SYN event
 *
 */

int UinputIf::synUinputDevice()
{
    struct input_event     ev;

    usleep(40000);
    memset(&ev, 0, sizeof(struct input_event));
    gettimeofday(&ev.time, NULL);
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(fd, &ev, sizeof(struct input_event)) < 0)
    {
        printf("uinput: error: EV_SYN write\n");
        return false;
    }
    return true;
}


/*
 * submits key presses
 * code = KEY_0
 * val = 1 down, 0 up
 *
 */

int UinputIf::sendUinputKeyPress(unsigned int code, int val)
{
    struct input_event     ev;

    usleep(25000); /* sorcery */
    memset(&ev, 0, sizeof(struct input_event));
    gettimeofday(&ev.time, NULL);
    ev.type = EV_KEY;
    ev.code = code;
    ev.value = val;
    if (write(fd, &ev, sizeof(struct input_event)) < 0)
    {
        printf("uinput: error: EV_KEY write\n");
        return false;
    }

    return true;
}

/*
 * submits switch event
 * code = SW_KEYPAD_SLIDE
 * val = 0 CLOSED, 1 OPEN
 *
 */

int UinputIf::sendUinputSwitch(unsigned int code, int val)
{
    struct input_event     ev;

    usleep(25000); /* sorcery */
    memset(&ev, 0, sizeof(struct input_event));
    gettimeofday(&ev.time, NULL);
    ev.type = EV_SW;
    ev.code = code;
    ev.value = val;
    if (write(fd, &ev, sizeof(struct input_event)) < 0)
    {
        printf("uinput: error: EV_SW write\n");
        return false;
    }

    return true;
}


/*
 * destroys and closes the opened uinput device
 *
 */

int UinputIf::closeUinputDevice()
{
    usleep(100000);

    if (ioctl(fd, UI_DEV_DESTROY) < 0)
    {
        printf("uinput: error: ioctl\n");
        return false;
    }

    close(fd);
    return true;
}
