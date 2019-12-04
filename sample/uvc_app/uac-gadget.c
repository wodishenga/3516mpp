/*
 * UVC gadget test application
 *
 * Copyright (C) 2010 Ideas on board SPRL <laurent.pinchart@ideasonboard.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 */

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <linux/usb/ch9.h>

#include "hicamera.h"
#include "uvc.h"
#include "video.h"
#include "uac_gadgete.h"
#include "uvc_venc_glue.h"
#include "hiaudio.h"
#include "log.h"
#include "hi_ctrl.h"
#include "config_svc.h"

#define CACHE_COUNT (4)

#if 1
 #define v4l2_fourcc(a, b, c, d) \
    ((__u32)(a) | ((__u32)(b) << 8) | ((__u32)(c) << 16) | ((__u32)(d) << 24))

 #define VIDIOC_S_FMT _IOWR('V', 5, struct v4l2_format)
 #define VIDIOC_REQBUFS _IOWR('V', 8, struct v4l2_requestbuffers)
 #define VIDIOC_QUERYBUF _IOWR('V', 9, struct v4l2_buffer)
 #define VIDIOC_QUERYCAP _IOR('V', 0, struct v4l2_capability)
 #define VIDIOC_STREAMOFF _IOW('V', 19, int)
 #define VIDIOC_QBUF _IOWR('V', 15, struct v4l2_buffer)
 #define VIDIOC_S_CTRL _IOWR('V', 28, struct v4l2_control)
 #define VIDIOC_S_DV_TIMINGS _IOWR('V', 87, struct v4l2_dv_timings)
 #define VIDIOC_G_DV_TIMINGS _IOWR('V', 88, struct v4l2_dv_timings)
 #define VIDIOC_DQEVENT _IOR('V', 89, struct v4l2_event)
 #define VIDIOC_SUBSCRIBE_EVENT _IOW('V', 90, struct v4l2_event_subscription)
 #define VIDIOC_UNSUBSCRIBE_EVENT _IOW('V', 91, struct v4l2_event_subscription)
 #define VIDIOC_STREAMON _IOW('V', 18, int)
 #define VIDIOC_DQBUF _IOWR('V', 17, struct v4l2_buffer)

 #define V4L2_PIX_FMT_YUYV v4l2_fourcc('Y', 'U', 'Y', 'V')/* 16  YUV 4:2:2     */
 #define V4L2_PIX_FMT_YUV420 v4l2_fourcc('Y', 'U', '1', '2')/* 16  YUV 4:2:0     */
 #define V4L2_PIX_FMT_NV12 v4l2_fourcc('N', 'V', '1', '2')/* 16  YUV 4:2:0     */
 #define V4L2_PIX_FMT_MJPEG v4l2_fourcc('M', 'J', 'P', 'G')/* Motion-JPEG   */
 #define V4L2_PIX_FMT_JPEG v4l2_fourcc('J', 'P', 'E', 'G')/* JFIF JPEG     */
 #define V4L2_PIX_FMT_DV v4l2_fourcc('d', 'v', 's', 'd')/* 1394          */
 #define V4L2_PIX_FMT_MPEG v4l2_fourcc('M', 'P', 'E', 'G')/* MPEG-1/2/4 Multiplexed */
 #define V4L2_PIX_FMT_H264 v4l2_fourcc('H', '2', '6', '4')/* H264 with start codes */
 #define V4L2_PIX_FMT_H264_NO_SC v4l2_fourcc('A', 'V', 'C', '1')/* H264 without start codes */
 #define V4L2_PIX_FMT_H264_MVC v4l2_fourcc('M', '2', '6', '4')/* H264 MVC */
 #define V4L2_PIX_FMT_H263 v4l2_fourcc('H', '2', '6', '3')/* H263          */
 #define V4L2_PIX_FMT_MPEG1 v4l2_fourcc('M', 'P', 'G', '1')/* MPEG-1 ES     */
 #define V4L2_PIX_FMT_MPEG2 v4l2_fourcc('M', 'P', 'G', '2')/* MPEG-2 ES     */
 #define V4L2_PIX_FMT_MPEG4 v4l2_fourcc('M', 'P', 'G', '4')/* MPEG-4 ES     */
 #define V4L2_PIX_FMT_XVID v4l2_fourcc('X', 'V', 'I', 'D')/* Xvid           */
 #define V4L2_PIX_FMT_VC1_ANNEX_G v4l2_fourcc('V', 'C', '1', 'G')/* SMPTE 421M Annex G compliant stream */
 #define V4L2_PIX_FMT_VC1_ANNEX_L v4l2_fourcc('V', 'C', '1', 'L')/* SMPTE 421M Annex L compliant stream */
 #define V4L2_PIX_FMT_VP8 v4l2_fourcc('V', 'P', '8', '0')/* VP8 */

 #define V4L2_EVENT_PRIVATE_START 0x08000000
 #define UVC_EVENT_FIRST (V4L2_EVENT_PRIVATE_START + 0)
 #define UVC_EVENT_CONNECT (V4L2_EVENT_PRIVATE_START + 0)
 #define UVC_EVENT_DISCONNECT (V4L2_EVENT_PRIVATE_START + 1)
 #define UVC_EVENT_STREAMON (V4L2_EVENT_PRIVATE_START + 2)
 #define UVC_EVENT_STREAMOFF (V4L2_EVENT_PRIVATE_START + 3)
 #define UVC_EVENT_SETUP (V4L2_EVENT_PRIVATE_START + 4)
 #define UVC_EVENT_DATA (V4L2_EVENT_PRIVATE_START + 5)
 #define UVC_EVENT_LAST (V4L2_EVENT_PRIVATE_START + 5)

/*struct uvc_streaming_control {
    __u16 bmHint;
    __u8  bFormatIndex;
    __u8  bFrameIndex;
    __u32 dwFrameInterval;
    __u16 wKeyFrameRate;
    __u16 wPFrameRate;
    __u16 wCompQuality;
    __u16 wCompWindowSize;
    __u16 wDelay;
    __u32 dwMaxVideoFrameSize;
    __u32 dwMaxPayloadTransferSize;
    __u32 dwClockFrequency;
    __u8  bmFramingInfo;
    __u8  bPreferedVersion;
    __u8  bMinVersion;
    __u8  bMaxVersion;
} __attribute__((__packed__));
 */
struct v4l2_event_subscription
{
    __u32 type;
    __u32 id;
    __u32 flags;
    __u32 reserved[5];
};

/* Payload for V4L2_EVENT_VSYNC */
struct v4l2_event_vsync
{
    /* Can be V4L2_FIELD_ANY, _NONE, _TOP or _BOTTOM */
    __u8 field;
} __attribute__ ((packed));

/* Payload for V4L2_EVENT_CTRL */
 #define V4L2_EVENT_CTRL_CH_VALUE (1 << 0)
 #define V4L2_EVENT_CTRL_CH_FLAGS (1 << 1)
 #define V4L2_EVENT_CTRL_CH_RANGE (1 << 2)

struct v4l2_event_ctrl
{
    __u32 changes;
    __u32 type;
    union
    {
        __s32 value;
        __s64 value64;
    };
    __u32 flags;
    __s32 minimum;
    __s32 maximum;
    __s32 step;
    __s32 default_value;
};

struct v4l2_event_frame_sync
{
    __u32 frame_sequence;
};

struct v4l2_event
{
    __u32 type;
    union
    {
        struct v4l2_event_vsync      vsync;
        struct v4l2_event_ctrl       ctrl;
        struct v4l2_event_frame_sync frame_sync;
        __u8                         data[64];
    }               u;
    __u32           pending;
    __u32           sequence;
    struct timespec timestamp;
    __u32           id;
    __u32           reserved[8];
};
enum v4l2_buf_type
{
    V4L2_BUF_TYPE_VIDEO_CAPTURE = 1,
    V4L2_BUF_TYPE_VIDEO_OUTPUT  = 2,
    V4L2_BUF_TYPE_VIDEO_OVERLAY = 3,
    V4L2_BUF_TYPE_VBI_CAPTURE = 4,
    V4L2_BUF_TYPE_VBI_OUTPUT = 5,
    V4L2_BUF_TYPE_SLICED_VBI_CAPTURE = 6,
    V4L2_BUF_TYPE_SLICED_VBI_OUTPUT = 7,
 #if 1
    /* Experimental */
    V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
 #endif
    V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
    V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE = 10,

    /* Deprecated, do not use */
    V4L2_BUF_TYPE_PRIVATE = 0x80,
};

enum v4l2_memory
{
    V4L2_MEMORY_MMAP = 1,
    V4L2_MEMORY_USERPTR = 2,
    V4L2_MEMORY_OVERLAY = 3,
    V4L2_MEMORY_DMABUF = 4,
};

enum v4l2_field
{
    V4L2_FIELD_ANY  = 0,          /* driver can choose from none,
                                  top, bottom, interlaced
                                  depending on whatever it thinks
                                  is approximate ... */
    V4L2_FIELD_NONE = 1, /* this device has no fields ... */
    V4L2_FIELD_TOP = 2, /* top field only */
    V4L2_FIELD_BOTTOM = 3, /* bottom field only */
    V4L2_FIELD_INTERLACED = 4, /* both fields interlaced */
    V4L2_FIELD_SEQ_TB = 5,        /* both fields sequential into one
                                  buffer, top-bottom order */
    V4L2_FIELD_SEQ_BT = 6, /* same as above + bottom-top order */
    V4L2_FIELD_ALTERNATE = 7,     /* both fields alternating into
                                  separate buffers */
    V4L2_FIELD_INTERLACED_TB = 8, /* both fields interlaced, top field
                                  first and the top field is
                                  transmitted first */
    V4L2_FIELD_INTERLACED_BT = 9, /* both fields interlaced, top field
                                  first and the bottom field is
                                  transmitted first */
};
struct v4l2_capability
{
    __u8  driver[16];
    __u8  card[32];
    __u8  bus_info[32];
    __u32 version;
    __u32 capabilities;
    __u32 device_caps;
    __u32 reserved[3];
};
struct v4l2_timecode
{
    __u32 type;
    __u32 flags;
    __u8  frames;
    __u8  seconds;
    __u8  minutes;
    __u8  hours;
    __u8  userbits[4];
};
struct v4l2_buffer
{
    __u32                index;
    __u32                type;
    __u32                bytesused;
    __u32                flags;
    __u32                field;
    struct timeval       timestamp;
    struct v4l2_timecode timecode;
    __u32                sequence;

    /* memory location */
    __u32 memory;
    union
    {
        __u32              offset;
        unsigned long      userptr;
        struct v4l2_plane* planes;
        __s32              fd;
    }     m;
    __u32 length;
    __u32 reserved2;
    __u32 reserved;
};
struct v4l2_requestbuffers
{
    __u32 count;
    __u32 type;           /* enum v4l2_buf_type */
    __u32 memory;         /* enum v4l2_memory */
    __u32 reserved[2];
};
struct v4l2_pix_format
{
    __u32 width;
    __u32 height;
    __u32 pixelformat;
    __u32 field;          /* enum v4l2_field */
    __u32 bytesperline;   /* for padding, zero if unused */
    __u32 sizeimage;
    __u32 colorspace;     /* enum v4l2_colorspace */
    __u32 priv;           /* private data, depends on pixelformat */
};
 #define VIDEO_MAX_PLANES 8
struct v4l2_plane_pix_format
{
    __u32 sizeimage;
    __u16 bytesperline;
    __u16 reserved[7];
} __attribute__ ((packed));
struct v4l2_pix_format_mplane
{
    __u32 width;
    __u32 height;
    __u32 pixelformat;
    __u32 field;
    __u32 colorspace;

    struct v4l2_plane_pix_format plane_fmt[VIDEO_MAX_PLANES];
    __u8                         num_planes;
    __u8                         reserved[11];
} __attribute__ ((packed));
struct v4l2_rect
{
    __s32 left;
    __s32 top;
    __s32 width;
    __s32 height;
};
 #define __user
struct v4l2_clip
{
    struct v4l2_rect c;
    struct v4l2_clip __user* next;
};
struct v4l2_window
{
    struct v4l2_rect w;
    __u32            field;          /* enum v4l2_field */
    __u32            chromakey;
    struct v4l2_clip __user* clips;
    __u32            clipcount;
    void __user*     bitmap;
    __u8             global_alpha;
};
struct v4l2_vbi_format
{
    __u32 sampling_rate;          /* in 1 Hz */
    __u32 offset;
    __u32 samples_per_line;
    __u32 sample_format;          /* V4L2_PIX_FMT_* */
    __s32 start[2];
    __u32 count[2];
    __u32 flags;                  /* V4L2_VBI_* */
    __u32 reserved[2];            /* must be zero */
};
struct v4l2_sliced_vbi_format
{
    __u16 service_set;

    /* service_lines[0][...] specifies lines 0-23 (1-23 used) of the first field
       service_lines[1][...] specifies lines 0-23 (1-23 used) of the second field
       (equals frame lines 313-336 for 625 line video
       standards, 263-286 for 525 line standards) */
    __u16 service_lines[2][24];
    __u32 io_size;
    __u32 reserved[2];            /* must be zero */
};
struct v4l2_format
{
    __u32 type;
    union
    {
        struct v4l2_pix_format        pix;       /* V4L2_BUF_TYPE_VIDEO_CAPTURE */
        struct v4l2_pix_format_mplane pix_mp;  /* V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE */
        struct v4l2_window            win;       /* V4L2_BUF_TYPE_VIDEO_OVERLAY */
        struct v4l2_vbi_format        vbi;       /* V4L2_BUF_TYPE_VBI_CAPTURE */
        struct v4l2_sliced_vbi_format sliced;  /* V4L2_BUF_TYPE_SLICED_VBI_CAPTURE */
        __u8                          raw_data[200]; /* user-defined */
    } fmt;
};
#endif


#define clamp(val, min, max) ({                 \
                                  typeof(val)__val = (val);              \
                                  typeof(min)__min = (min);              \
                                  typeof(max)__max = (max);              \
                                  (void) (&__val == &__min);              \
                                  (void) (&__val == &__max);              \
                                  __val = __val < __min ? __min : __val;   \
                                  __val > __max ? __max : __val; })

#define ARRAY_SIZE(a) ((sizeof(a) / sizeof(a[0])))

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum
{
    IO_METHOD_MMAP = 0x1,
    IO_METHOD_USERPTR = 0x2,
} io_method;


typedef struct uvc_probe_t
{
    unsigned char set;
    unsigned char get;
    unsigned char max;
    unsigned char min;
} uvc_probe_t;

struct uac_device
{
    int fd;
    int tSampleFreq;
    int nbufs;
    int streaming;
};

#define WAITED_NODE_SIZE (4)
static frame_node_t *__waited_node[WAITED_NODE_SIZE];

extern unsigned int g_bulk;
static void clear_waited_node()
{
    int i = 0;
    uvc_cache_t *uvc_cache = uvc_cache_get();

    for (i = 0; i < WAITED_NODE_SIZE; i++)
    {
        if ((__waited_node[i] != 0) && uvc_cache)
        {
            put_node_to_queue(uvc_cache->free_queue, __waited_node[i]);
            __waited_node[i] = 0;
        }
    }
}

static struct uac_device*uac_open(const char *devname)
{
    struct uac_device* dev;
    struct v4l2_capability cap;
    int ret;
    int fd;

    fd = open(devname, O_RDWR | O_NONBLOCK);

    if (fd == -1)
    {
        return NULL;
    }

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);

    if (ret < 0)
    {
        LOG("unable to query device: %s (%d)\n", strerror(errno),
            errno);
        close(fd);
        return NULL;
    }

    //LOG("open succeeded(%s:caps=0x%04x)\n", devname, cap.capabilities);

    /* check the devie is a audio or not*/
    if (!(cap.capabilities & 0x00020000))
    {
        close(fd);
        return NULL;
    }

    //LOG("device is %s on bus %s\n", cap.card, cap.bus_info);

    dev = (struct uac_device*)malloc(sizeof * dev);

    if (dev == NULL)
    {
        close(fd);
        return NULL;
    }

    memset(dev, 0, sizeof * dev);
    dev->fd = fd;

    return dev;
}

static void uac_close(struct uac_device* dev)
{
    close(dev->fd);
    free(dev);
}

/* ---------------------------------------------------------------------------
 * Video streaming
 */

static void uac_video_fill_buffer_userptr(struct uac_device* dev, struct v4l2_buffer* buf)
{
    uac_cache_t *uac_cache = uac_cache_get();
    frame_node_t *node = 0;
    frame_queue_t *q = 0, *fq = 0;
    int retry_count = 0;

retry:
    if (uac_cache)
    {
        q  = uac_cache->ok_queue;
        fq = uac_cache->free_queue;
        get_node_from_queue(q, &node);
    }

    if ((__waited_node[buf->index] != 0) && uac_cache)
    {
        put_node_to_queue(fq, __waited_node[buf->index]);
        __waited_node[buf->index] = 0;
    }

    if (node != 0)
    {
        buf->bytesused = node->used;
        buf->m.userptr = (unsigned long) node->mem;
        buf->length = node->length;
        __waited_node[buf->index] = node;
    }
    else if (retry_count++ < 10)
    {
        // the perfect solution is using locker and waiting queue's notify.
        // but here just only simply used usleep method and try again.
        // it works fine now.
        usleep(5);
        goto retry;
    }

}

static int uac_video_process_userptr(struct uac_device* dev)
{
    struct v4l2_buffer buf;
    int ret;

    memset(&buf, 0, sizeof buf);
    buf.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    buf.memory = V4L2_MEMORY_USERPTR;

    if ((ret = ioctl(dev->fd, VIDIOC_DQBUF, &buf)) < 0)
    {
        return ret;
    }

    uac_video_fill_buffer_userptr(dev, &buf);

    if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0)
    {
        LOG("Unable to requeue buffer: %s (%d).\n", strerror(errno),
            errno);
        return ret;
    }

    return 0;
}

static int uac_video_reqbufs_userptr(struct uac_device* dev, int nbufs)
{
    struct v4l2_requestbuffers rb;
    int ret;

    memset(&rb, 0, sizeof rb);
    rb.count = nbufs;
    rb.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    rb.memory = V4L2_MEMORY_USERPTR;

//    LOG("%u buffers allocated.\n", rb.count);

    ret = ioctl(dev->fd, VIDIOC_REQBUFS, &rb);

    if (ret < 0)
    {
        LOG("Unable to allocate buffers: %s (%d).\n",
             strerror(errno), errno);
        return ret;
    }

//    LOG("%u buffers allocated.\n", rb.count);

    dev->nbufs = rb.count;

    return 0;
}

static int uac_video_stream_userptr(struct uac_device* dev)
{
    struct v4l2_buffer buf;
    unsigned int i;
    int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    int ret = 0;

//    LOG("%s:Starting uac stream.\n", __func__);

    for (i = 0; i < (dev->nbufs); ++i)
    {
        memset(&buf, 0, sizeof buf);

        buf.index = i;
        buf.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf.memory = V4L2_MEMORY_USERPTR;

        uac_video_fill_buffer_userptr(dev, &buf);

        if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0)
        {
            LOG("Unable to queue buffer(%d): %s (%d).\n", i,
                 strerror(errno), errno);
            break;
        }
    }

    ioctl(dev->fd, VIDIOC_STREAMON, &type);

    dev->streaming = 1;

    return ret;
}

static int uac_video_init(struct uac_device* dev __attribute__ ((__unused__)))
{
    return 0;
}

/* ---------------------------------------------------------------------------
 * Request processing
 */

struct uvc_frame_info
{
    unsigned int width;
    unsigned int height;
    unsigned int intervals[8];
};

struct uvc_format_info
{
    unsigned int                 fcc;
    const struct uvc_frame_info* frames;
};

static void disable_uac_video(struct uac_device* dev)
{
    int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    ioctl(dev->fd, VIDIOC_STREAMOFF, &type);

    clear_waited_node();
}

static void enable_uac_video(struct uac_device* dev)
{
    disable_uac_video(dev);
    uac_video_reqbufs_userptr(dev, CACHE_COUNT);
    uac_video_stream_userptr(dev);
}

// extern AUDIO_SAMPLE_RATE_E enInSampleRate;

static void uac_events_process_data(struct uac_device* dev, struct uvc_request_data* data ,struct usb_ctrlrequest* ctrl)
{
    // enInSampleRate = data->data[2]*256*256 + data->data[1]*256 + data->data[0];
    hiaudio_shutdown();
    disable_uac_video(dev);
    hiaudio_startup();
    enable_uac_video(dev);
}

static void uac_events_process(struct uac_device* dev)
{
    struct v4l2_event v4l2_event;
    int ret;
    struct uvc_event* uvc_event = (struct uvc_event*)(void*)&v4l2_event.u.data;

    ret = ioctl(dev->fd, VIDIOC_DQEVENT, &v4l2_event);

    if (ret < 0)
    {
        LOG("VIDIOC_DQEVENT failed: %s (%d)\n", strerror(errno),
            errno);
        return;
    }

    switch (v4l2_event.type)
    {
    case UVC_EVENT_CONNECT:
        break;
    case UVC_EVENT_DISCONNECT:
        return;
    case UVC_EVENT_STREAMON:
   //     LOG("\033[;31m handle uac streamon event\033[0m\n");
        hiaudio_startup();
        enable_uac_video(dev);
        return;
    case UVC_EVENT_STREAMOFF:
    //    LOG("\033[;31m handle uac streamoff\033[0m\n");
        hiaudio_shutdown();
        disable_uac_video(dev);
        dev->streaming = 0;
        return;
    case UVC_EVENT_DATA:
    //    LOG("\033[;31m handle uac data event length %d data %d %d %d \033[0m\n",uvc_event->data.length,uvc_event->data.data[0],uvc_event->data.data[1],uvc_event->data.data[2]);
        uac_events_process_data(dev,&uvc_event->data,&uvc_event->req);
        return;
    default:
        LOG("\033[;31m Unknow command from uac driver\033[0m\n");
        break;
    }
}

static void uac_events_init(struct uac_device* dev)
{
    struct v4l2_event_subscription sub;

    memset(&sub, 0, sizeof sub);

    sub.type = UVC_EVENT_CONNECT;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_DISCONNECT;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_STREAMON;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_STREAMOFF;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_DATA;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
}

/* ---------------------------------------------------------------------------
 * main
 */

static struct uac_device *__uac_device = 0;

int open_uac_device(const char *devpath)
{
    struct uac_device* dev;

    char *device = (char*)devpath;

    dev = uac_open(device);
    if (dev == 0)
    {
        return -1;
    }

    uac_video_init(dev);
    uac_events_init(dev);

    __uac_device = dev;

    return 0;
}

int close_uac_device()
{
    if (__uac_device != 0)
    {
        disable_uac_video(__uac_device);
        uac_close(__uac_device);
    }

    __uac_device = 0;

    return 0;
}

int run_uac_device()
{
    fd_set efds, wfds;
    struct timeval tv;
    int r;

    if (!__uac_device)
    {
        return -1;
    }

    tv.tv_sec  = 2;
    tv.tv_usec = 0;

    FD_ZERO(&efds);
    FD_ZERO(&wfds);
    FD_SET(__uac_device->fd, &efds);

    if (__uac_device->streaming == 1)
        FD_SET(__uac_device->fd, &wfds);

    r = select(__uac_device->fd + 1, NULL, &wfds, &efds, &tv);
    if (r > 0)
    {
        if (FD_ISSET(__uac_device->fd, &efds))
        {
            uac_events_process(__uac_device);
        }

        if (FD_ISSET(__uac_device->fd, &wfds))
        {
            uac_video_process_userptr(__uac_device);
        }
    }

    return r;
}
