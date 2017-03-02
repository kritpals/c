/**
Originally created for testing purpose.
Made by Kritpal Singh.
For any further query email me @ kritpal.sing@gmail.com
Created for Mediatek Ralink SDK
 * /opt/bin/arm-openwrt-linux-gcc acl_plus.c -o acl -I./include/;cp acl /tftpboot/
 * @param value
 * @param ifname
 * @return 
 */
#include <stdio.h>
#include<string.h>
#include "iwlib.h"
#define fprintf(x, y, ...) //To comment frpintf 
#define printf(x,...)		//To comment printf
static const int priv_type_size[] = {
    0, /* IW_PRIV_TYPE_NONE */
    1, /* IW_PRIV_TYPE_BYTE */
    1, /* IW_PRIV_TYPE_CHAR */
    0, /* Not defined */
    sizeof (__u32), /* IW_PRIV_TYPE_INT */
    sizeof (struct iw_freq), /* IW_PRIV_TYPE_FLOAT */
    sizeof (struct sockaddr), /* IW_PRIV_TYPE_ADDR */
    0, /* Not defined */
};

int iw_get_priv_info(int skfd,
        const char * ifname,
        iwprivargs ** ppriv) {
    struct iwreq wrq;
    iwprivargs * priv = NULL; /* Not allocated yet */
    int maxpriv = 16; /* Minimum for compatibility WE<13 */
    iwprivargs * newpriv;

    /* Some driver may return a very large number of ioctls. Some
     * others a very small number. We now use a dynamic allocation
     * of the array to satisfy everybody. Of course, as we don't know
     * in advance the size of the array, we try various increasing
     * sizes. Jean II */
    do {
        /* (Re)allocate the buffer */
        newpriv = (iwprivargs *) realloc(priv, maxpriv * sizeof (priv[0]));
        if (newpriv == NULL) {
            fprintf(stderr, "%s: Allocation failed\n", __FUNCTION__);
            break;
        }
        priv = newpriv;

        /* Ask the driver if it's large enough */
        wrq.u.data.pointer = (caddr_t) priv;
        wrq.u.data.length = maxpriv;
        wrq.u.data.flags = 0;
        printf("SIOCG=%x\n", SIOCGIWPRIV);
        if (iw_get_ext(skfd, ifname, SIOCGIWPRIV, &wrq) >= 0) {
            /* Success. Pass the buffer by pointer */
            *ppriv = priv;
            printf("priv set_args=%hu\n",priv->set_args);
            /* Return the number of ioctls */
            return (wrq.u.data.length);
        }

        /* Only E2BIG means the buffer was too small, abort on other errors */
        if (errno != E2BIG) {
            /* Most likely "not supported". Don't barf. */
            break;
        }

        /* Failed. We probably need a bigger buffer. Check if the kernel
         * gave us any hints. */
        if (wrq.u.data.length > maxpriv)
            maxpriv = wrq.u.data.length;
        else
            maxpriv *= 2;
    } while (maxpriv < 1000);

    /* Cleanup */
    if (priv)
        free(priv);
    *ppriv = NULL;

    return (-1);
}

int
iw_get_priv_size(int args) {
    int num = args & IW_PRIV_SIZE_MASK;
    int type = (args & IW_PRIV_TYPE_MASK) >> 12;

    return (num * priv_type_size[type]);
}

static inline int set_private(char * args, /* Command line args */
        char * ifname, char *cmdname) /* Dev name */ {
    iwprivargs * priv;

    int sock;
    int number; /* Max of private ioctl */
    int ret;
    struct iwreq wrq;
    u_char buffer[8192]; /* Only that big in v25 and later */
    int i = 0; /* Start with first command arg */
    int k; /* Index in private description table */
    int temp;
    int subcmd = 0; /* sub-ioctl index */
    int offset = 0; /* Space for sub-ioctl index */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Sock=%d\n", sock);
    if (sock < 0) {
        printf("socket error...\n");
        return 0;
    }

    /* Read the private ioctls */
    number = iw_get_priv_info(sock, ifname, &priv);
    //    fprintf(stderr, "priv name=%s\n",
    //            priv->name);
    /* Is there any ? */
    if (number <= 0) {
        /* Should I skip this message ? */
        fprintf(stderr, "%-8.16s  no private ioctls.\n\n",
                ifname);
        if (priv)
            free(priv);
        return 0;
    }
    printf("set_args=%hu,K=%d\n", priv[k].set_args,k);
    /* Do it */
    wrq.u.data.length = strlen(args) + 1;
    if (wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
        wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;
    fprintf(stderr, "case IW_PRIV_TYPE_CHAR\n");
    /* Fetch string */
    memcpy(buffer, args, wrq.u.data.length);
    buffer[sizeof (buffer) - 1] = '\0';
    fprintf(stderr, "Buffer=%s\n", buffer);
    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
    if ((priv[k].set_args & IW_PRIV_SIZE_FIXED) &&
            ((iw_get_priv_size(priv[k].set_args) + offset) <= IFNAMSIZ)) {
        /* First case : all SET args fit within wrq */
        if (offset)
            wrq.u.mode = subcmd;
        memcpy(wrq.u.name + offset, buffer, IFNAMSIZ - offset);
    } else {
        if ((priv[k].set_args == 0) &&
                (priv[k].get_args & IW_PRIV_SIZE_FIXED) &&
                (iw_get_priv_size(priv[k].get_args) <= IFNAMSIZ)) {
            /* Second case : no SET args, GET args fit within wrq */
            if (offset)
                wrq.u.mode = subcmd;
        } else {
            /* Third case : args won't fit in wrq, or variable number of args */
            printf("Third case\n");
            wrq.u.data.pointer = (caddr_t) buffer;
            wrq.u.data.flags = subcmd;
        }
    }

    /* Perform the private ioctl */
    if (ioctl(sock, priv[k].cmd, &wrq) < 0) {
        fprintf(stderr, "Interface doesn't accept private ioctl...\n");
        fprintf(stderr, "%s (%X): %s\n", cmdname, priv[k].cmd, strerror(errno));
        return 0;
    }
    printf("ssssss\n");
    /* If we have to get some data */

    free(priv);
    close(sock);
    return 1;
}

int main(int argc, char**argv) {

    if (argc <= 1) {
        printf("ERROR:usage-- ./acl 12:11:11:11:11:11\n");
        return 0;
    }
    printf("Argv=%s\n", argv[1]);
    set_private(argv[1], "ra0", "set");
    set_private(argv[1], "ra1", "set");

}

