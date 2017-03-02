/**
 *Originally created for testing purpose.
Made by Kritpal Singh.
For any further query email me @ kritpal.sing@gmail.com
Created for Openwrt SDK
  //home/kritpal/coconut/openwrt-2.4.2011-trunk/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-gcc acl_plus.c -o acl -I./include/;cp acl /tftpboot/
 * @param value
 * @param ifname
 * @return 
 */
#include<stdio.h>
#include<string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <errno.h>
#include <linux/wireless.h>
#include "ieee80211_external.h"
#include "ieee80211_ioctl.h"

static int
atheros_set_macacl_cmd(int value, char *ifname) {
    int op = IEEE80211_PARAM_MACCMD;
    int sock;
    struct iwreq iwr;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
//    printf("Sock=%d\n", sock);
    if (sock < 0) {
//        printf("socket error...\n");
        return 0;
    }

    memset(&iwr, 0, sizeof (iwr));
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.mode = op;
    memcpy(iwr.u.name + sizeof (__u32), &value, sizeof (value));

    if (ioctl(sock, IEEE80211_IOCTL_SETPARAM, &iwr) < 0) {
        //        perror("ioctl[IEEE80211_IOCTL_SETPARAM]");
        //        printf("%s: %s: Failed to set parameter (op %d "
        //                "arg %d)", __func__, ifname, op,
        //                value);
        return -1;
    }
    return 0;
}

static int
atheros_add_macacl_addr(unsigned char *addr, char *ifname) {

    struct ifreq req;

    memset(&req, 0, sizeof (req));

    memcpy(req.ifr_hwaddr.sa_data, addr, IEEE80211_ADDR_LEN);
    strncpy(req.ifr_name, ifname, IFNAMSIZ);
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
//    printf("Sock=%d\n", sock);
    if (sock < 0) {
//        printf("\n %s...%d\n", __func__, __LINE__);
        return -1;
    }
    if (ioctl(sock, IEEE80211_IOCTL_ADDMAC, &req) < 0) {
//        perror("ioctl[IEEE80211_IOCTL_ADDMAC]");
        return -1;
    }

    return 0;
}

static int
atheros_del_macacl_addr(unsigned char *addr, char *ifname) {

    struct ifreq req;

    memset(&req, 0, sizeof (req));
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
//    printf("Sock=%d\n", sock);
    memcpy(req.ifr_hwaddr.sa_data, addr, IEEE80211_ADDR_LEN);
    strncpy(req.ifr_name, ifname, IFNAMSIZ);

    if (sock < 0) {
//        printf("\n %s...%d\n", __func__, __LINE__);
        return -1;
    }
    if (ioctl(sock, IEEE80211_IOCTL_DELMAC, &req) < 0) {
//        perror("ioctl[IEEE80211_IOCTL_ADDMAC]");
        return -1;
    }

    return 0;
}

static int
ieee80211_send_deauth(const char* ifname, unsigned char *addr) {
    struct iwreq iwr;
    struct ieee80211_wlanconfig config;
    int s = -1;


    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
//        printf("socket(SOCK_DRAGM)");
        return -1;
    }

    memset(&iwr, 0, sizeof (struct iwreq));
    //    memset(&iwr, 0, sizeof (struct iwreq));
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);

    config.cmdtype = IEEE80211_WLANCONFIG_DEAUTH_STA;
    //config.data.hw_channels.hw_mode = hw_mode;
    memcpy(config.data.wc_addr.mac, addr, IEEE80211_ADDR_LEN);
    // memcpy(config.data.wc_addr.bssid, bssid, IEEE80211_ADDR_LEN);
    iwr.u.data.pointer = (void*) &config;
    iwr.u.data.length = sizeof (struct ieee80211_wlanconfig);

//    printf("\n %s.......................%d\n", __func__, __LINE__);
    if (ioctl(s, IEEE80211_IOCTL_CONFIG_GENERIC, &iwr) < 0) {
//        perror("config_generic failed beacuse of invalid values");
        return -1;
    }


//    printf("\n");
    return 0;
}

int main(int argc, char**argv) {
    unsigned char mac[6];

    if (argc < 3) {
        printf("ERROR:usage-- ./acl set 2\n ./acl add 12:11:11:11:11:11\n");
        return 0;

    }
    if (strcmp(argv[1], "set") == 0) {
        atheros_set_macacl_cmd(atoi(argv[2]), "wlan10");
        atheros_set_macacl_cmd(atoi(argv[2]), "wlan00");
        atheros_set_macacl_cmd(atoi(argv[2]), "wlan30");
        atheros_set_macacl_cmd(atoi(argv[2]), "wlan40");
    } else if (strcmp(argv[1], "add") == 0) {

        sscanf(argv[2], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        printf("argv[2]=%s\tmac=%x:%x:%x:%x:%x:%x\n", argv[2], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        atheros_add_macacl_addr(mac, "wlan10");
        ieee80211_send_deauth("wlan10", mac);
        atheros_add_macacl_addr(mac, "wlan00");
        ieee80211_send_deauth("wlan00", mac);

        atheros_add_macacl_addr(mac, "wlan30");
        ieee80211_send_deauth("wlan30", mac);

        atheros_add_macacl_addr(mac, "wlan40");
        ieee80211_send_deauth("wlan40", mac);
    } else if (strcmp(argv[1], "del") == 0) {

        sscanf(argv[2], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        printf("argv[2]=%s\tmac=%x:%x:%x:%x:%x:%x\n", argv[2], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        atheros_del_macacl_addr(mac, "wlan10");
        atheros_del_macacl_addr(mac, "wlan00");
        atheros_del_macacl_addr(mac, "wlan30");
        atheros_del_macacl_addr(mac, "wlan40");
    }
}



















