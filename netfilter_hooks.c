/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Openwrt SDK 
Driver program to get all DNS query made in the network and access to drop or accept it.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/vmalloc.h>
#include <linux/if_ether.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux webfilter");
MODULE_AUTHOR("Kritpal singh");


static struct nf_hook_ops nfho;

unsigned int port_str_to_int(char *port_str) {
    unsigned int port = 0;
    int i = 0;
    if (port_str == NULL) {
        return 0;
    }
    while (port_str[i] != '\0') {
        port = port * 10 + (port_str[i] - '0');
        ++i;
    }
    return port;
}

void port_int_to_str(unsigned int port, char *port_str) {
    sprintf(port_str, "%u", port);
}

unsigned int ip_str_to_hl(char *ip_str) {
    /*convert the string to byte array first, e.g.: from "131.132.162.25" to [131][132][162][25]*/
    unsigned char ip_array[4];
    int i = 0;
    unsigned int ip = 0;
    if (ip_str == NULL) {
        return 0;
    }
    memset(ip_array, 0, 4);
    while (ip_str[i] != '.') {
        ip_array[0] = ip_array[0]*10 + (ip_str[i++] - '0');
    }
    ++i;
    while (ip_str[i] != '.') {
        ip_array[1] = ip_array[1]*10 + (ip_str[i++] - '0');
    }
    ++i;
    while (ip_str[i] != '.') {
        ip_array[2] = ip_array[2]*10 + (ip_str[i++] - '0');
    }
    ++i;
    while (ip_str[i] != '\0') {
        ip_array[3] = ip_array[3]*10 + (ip_str[i++] - '0');
    }
    /*convert from byte array to host long integer format*/
    ip = (ip_array[0] << 24);
    ip = (ip | (ip_array[1] << 16));
    ip = (ip | (ip_array[2] << 8));
    ip = (ip | ip_array[3]);
    //printk(KERN_INFO "ip_str_to_hl convert %s to %un", ip_str, ip);
    return ip;
}

void ip_hl_to_str(unsigned int ip, char *ip_str) {
    /*convert hl to byte array first*/
    unsigned char ip_array[4];
    memset(ip_array, 0, 4);
    ip_array[0] = (ip_array[0] | (ip >> 24));
    ip_array[1] = (ip_array[1] | (ip >> 16));
    ip_array[2] = (ip_array[2] | (ip >> 8));
    ip_array[3] = (ip_array[3] | ip);
    sprintf(ip_str, "%u.%u.%u.%u", ip_array[0], ip_array[1], ip_array[2], ip_array[3]);
}

unsigned int hook_func_in(unsigned int hooknum, struct sk_buff *skb,

        const struct net_device *in, const struct net_device *out,

        int (*okfn)(struct sk_buff *)) {

    /*get src address, src netmask, src port, dest ip, dest netmask, dest port, protocol*/

    struct iphdr *ip_header = (struct iphdr *) skb_network_header(skb);

    //hdr->h_source;
    struct udphdr *udp_header;

    struct tcphdr *tcp_header;

    struct list_head *p;

    struct mf_rule *a_rule;
    char src_ip_str[16], dest_ip_str[16];

    int i = 0;

    /**get src and dest ip addresses**/
    if (ip_header->protocol == 17) {

        unsigned int src_ip = (unsigned int) ip_header->saddr;
        unsigned char src_mac[50];
        unsigned int dest_ip = (unsigned int) ip_header->daddr;

        unsigned int src_port = 0;

        unsigned int dest_port = 0;

        /***get src and dest port number***/


        struct ethhdr *hdr = (struct ethhdr *) skb_mac_header(skb);
        sprintf(src_mac, "%02X:%02X:%02X:%02X:%02X:%02X", hdr->h_source[0], hdr->h_source[1], hdr->h_source[2], hdr->h_source[3], hdr->h_source[4], hdr->h_source[5]);
        udp_header = (struct udphdr *) (skb_transport_header(skb) + 20);

        src_port = (unsigned int) ntohs(udp_header->source);

        dest_port = (unsigned int) ntohs(udp_header->dest);


        ip_hl_to_str(ntohl(src_ip), src_ip_str);
        ip_hl_to_str(ntohl(dest_ip), dest_ip_str);
        if (dest_port == 53) {
            //Get dns paylod 
            unsigned char * payload = (unsigned char *) (skb_transport_header(skb) + 20 + 12 + sizeof (struct udphdr));
            int len;
            int j = 0;
            int cur_ind;
            unsigned char domain_sub[1024];
            unsigned char *ptr_to_domain;
            len = payload[0];
            cur_ind = 1;
            ptr_to_domain = domain_sub;
            while (len != 0) {
                if (j >= 1024) {
                    printk(KERN_INFO "Size of the URL is too big to handle .. return \n");
                    /* Size of the URL is too big to handle .. return */
                    return 0;
                }
                for (i = cur_ind; i < cur_ind + len; i++) {
                    ptr_to_domain[j++] = payload[i];
                }
                ptr_to_domain[j++] = '.';

                cur_ind = cur_ind + len + 1;
                len = payload[cur_ind - 1];
            }
            ptr_to_domain[j - 1] = 0;

            printk(KERN_INFO "UDP IN : Domain=%s,src mac=%s, src ip: %s, src port: %u; dest ip:  %s, dest port: %u,;", ptr_to_domain, src_mac, src_ip_str, src_port, dest_ip_str, dest_port);
            if (strcasecmp(src_mac, "10:60:2b:39:60:84") == 0 && strstr(ptr_to_domain, "facebook.com") != NULL) {
                return NF_DROP;
            }

        }
    }
    return NF_ACCEPT;

}

/* Initialization routine */
int init_module() {
    printk(KERN_INFO "initialize kernel modulen");

    // Fill in the hook structure for incoming packet hook

    nfho.hook = hook_func_in;

    nfho.hooknum = 0; //NF_IP_PRE_ROUTING

    nfho.pf = PF_INET;

    nfho.priority = NF_IP_PRI_FIRST;

    nf_register_hook(&nfho); // Register the hook


    return 0;
}

/* Cleanup routine */
void cleanup_module() {

    nf_unregister_hook(&nfho);

    printk(KERN_INFO "kernel module unloaded.n");

}
