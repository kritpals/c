/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally vreated for Openwrt SDK 
 Created by:Kritpal Singh
/home/kritpal/coconut/openwrt-2.4.2011-trunk/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-gcc getWlanstainfo.c -o staList -I ./include/ -I /home/kritpal/coconut/openwrt-2.4.2011-trunk/staging_dir/target-arm_uClibc-0.9.32_eabi/usr/include/;cp staList /tftpboot/

 */

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "oid.h"
#include <linux/wireless.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//#include <ieee80211_external.h>
#include <unistd.h>

static int list_stations(const char *ifname, char *mac);

int main(int argc, char **argv) {
    int i, s;
    struct iwreq iwr;
    RT_802_11_MAC_TABLE table = {0};

    s = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr.ifr_name, "ra0", sizeof (iwr.ifr_name));

    iwr.u.data.pointer = (caddr_t) & table;


    if (s < 0) {
        //websError(stdout, 500, "ioctl sock failed!");
        return -1;
    }
    while (1) {
        printf("****************************************************************************\n");
        if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
            //websError(stdout, 500, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
            close(s);
            return -1;
        }

        for (i = 0; i < table.Num; i++) {
            printf("%02X:%02X:%02X:%02X:%02X:%02X\t",
                    table.Entry[i].Addr[0], table.Entry[i].Addr[1],
                    table.Entry[i].Addr[2], table.Entry[i].Addr[3],
                    table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
            //printf(stdout, T("<td>%d</td><td>%d</td><td>%d</td>"),
            //		table.Entry[i].Aid, table.Entry[i].Psm, table.Entry[i].MimoPs);
            printf("Rssi0=%d\t Rssi1=%d\tAverage RSSi=%d\n", table.Entry[i].AvgRssi0, table.Entry[i].AvgRssi1, (table.Entry[i].AvgRssi0 + table.Entry[i].AvgRssi1) / 2);
            //            printf("RX=%u,TX=%u\n", table.Entry[i].T, table.Entry[i].TxRate.field.eTxBF);
        }
        printf("****************************************************************************\n");
        sleep(2);
    }
    close(s);
    //    int ret;
    //    if ((ret = list_stations("wlan10", "c0:ee:fb:31:59:7a"))) {
    //        printf("Rssi in 2.4g=%d\n", ret);
    //        return ret;
    //    } else if ((ret = list_stations("wlan00", "c0:ee:fb:31:59:7a"))) {
    //        printf("Rssi in 5g=%d\n", ret);
    //        return ret;
    //    }
    //    return 0;
}

//static const char *
//ieee80211_ntoa(const uint8_t mac[IEEE80211_ADDR_LEN]) {
//    static char a[18];
//    int i;
//
//    i = snprintf(a, sizeof (a), "%02x:%02x:%02x:%02x:%02x:%02x",
//            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//    return (i < 17 ? NULL : a);
//}
//
///*
// * Convert RSSI to dBm.
// */
//static u_int
//rssi2dbm(u_int rssi) {
//    return rssi - 95;
//}
/*
static int list_stations(const char *ifname, char *mac) {
#define LIST_STATION_ALLOC_SIZE 24*1024
    uint8_t *buf;
    struct iwreq iwr;
    uint8_t *cp;
    int s, len;

    char ntoa[40];
    buf = malloc(LIST_STATION_ALLOC_SIZE);
    if (!buf) {
        fprintf(stderr, "Unable to allocate memory for station list\n");
        return 0;
    }

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        free(buf);
        return 0;
    }

    if (!strncmp(ifname, "wifi", 4)) {
        free(buf);
        return 0;
    }

    (void) memset(&iwr, 0, sizeof (iwr));
    (void) strncpy(iwr.ifr_name, ifname, sizeof (iwr.ifr_name));
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.length = LIST_STATION_ALLOC_SIZE;
    if (ioctl(s, IEEE80211_IOCTL_STA_INFO, &iwr) < 0) {
        free(buf);
        fprintf(stdout, "unable to get station information");
    }
    len = iwr.u.data.length;

    if (len < sizeof (struct ieee80211req_sta_info)) {
        free(buf);

        return 0;
    }
    close(s);
    cp = buf;
    do {
        struct ieee80211req_sta_info *si;

        si = (struct ieee80211req_sta_info *) cp;
        snprintf(ntoa, sizeof (ntoa), "%02x:%02x:%02x:%02x:%02x:%02x", si->isi_macaddr[0], si->isi_macaddr[1], si->isi_macaddr[2], si->isi_macaddr[3], si->isi_macaddr[4], si->isi_macaddr[5]);


        if (!strcasecmp(mac, ntoa)) {
            free(buf);

            return (si->isi_rssi - 95);
        }
        cp += si->isi_len, len -= si->isi_len;
    } while (len >= sizeof (struct ieee80211req_sta_info));

    free(buf);
    return 0;
}
 */
