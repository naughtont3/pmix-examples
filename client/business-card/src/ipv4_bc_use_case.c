#include <pmix.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netdb.h>
#include <arpa/inet.h>

static const char *bc_key = "business-card";
struct bc_ipv4 {
    struct in_addr ip;
    in_port_t port;
};
static int create_socket(void);

int main(void) {
    struct bc_ipv4 *business_cards;
    pmix_status_t pmi_errno;
    pmix_proc_t wildcard_proc;
    pmix_proc_t proc;
    pmix_value_t put_value;
    pmix_value_t *get_value;
    uint32_t job_size;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char msgbuf[128];
    int fd;
    int rc;

    /* initialize PMIx */
    pmi_errno = PMIx_Init(&proc, NULL, 0);
    assert(pmi_errno == PMIX_SUCCESS);

    /* get job size */
    PMIX_LOAD_PROCID(&wildcard_proc, proc.nspace, PMIX_RANK_WILDCARD);
    pmi_errno = PMIx_Get(&wildcard_proc, PMIX_JOB_SIZE, NULL, 0, &get_value);
    assert(pmi_errno == PMIX_SUCCESS);
    job_size = get_value->data.uint32;
    PMIX_VALUE_RELEASE(get_value);

    /* allocate business card array */
    business_cards = malloc(sizeof(struct bc_ipv4) * job_size);
    assert(business_cards != NULL);

    /* open socket and put card into PMIx */
    fd = create_socket();
    rc = getsockname(fd, (struct sockaddr *)&addr, &addrlen);
    assert(rc == 0);
    business_cards[proc.rank].ip = addr.sin_addr;
    business_cards[proc.rank].port = addr.sin_port;

    put_value.type = PMIX_BYTE_OBJECT;
    put_value.data.bo.bytes = (void *)&business_cards[proc.rank];
    put_value.data.bo.size = sizeof(struct bc_ipv4);
    pmi_errno = PMIx_Put(PMIX_GLOBAL, bc_key, &put_value);
    assert(pmi_errno == PMIX_SUCCESS);
    pmi_errno = PMIx_Commit();
    assert(pmi_errno == PMIX_SUCCESS);

    /* rank 0 will listen for connections */
    if (proc.rank == 0) {
        rc = listen(fd, job_size - 1);
        assert(rc == 0);
    }

    /* collective fence with peer processes */
    pmix_info_t *info;
    int flag = 1;
    PMIX_INFO_CREATE(info, 1);
    PMIX_INFO_LOAD(info, PMIX_COLLECT_DATA, &flag, PMIX_BOOL);
    pmi_errno = PMIx_Fence(&wildcard_proc, 1, info, 1);
    assert(pmi_errno == PMIX_SUCCESS);
    PMIX_INFO_FREE(info, 1);

    /* get cards from peers */
    pmix_proc_t peer_proc;
    PMIX_LOAD_NSPACE(peer_proc.nspace, proc.nspace);
    for (uint32_t i = 0; i < job_size; i++) {
        /* skip our own card */
        if (i == proc.rank) {
            continue;
        }

        /* get card for rank i from PMIx */
        peer_proc.rank = i;
        pmi_errno = PMIx_Get(&peer_proc, bc_key, NULL, 0, &get_value);
        assert(pmi_errno == PMIX_SUCCESS && get_value->type == PMIX_BYTE_OBJECT);

        /* copy the card to local array */
        assert(get_value->data.bo.size == sizeof(struct bc_ipv4));
        business_cards[i] = *(struct bc_ipv4 *)get_value->data.bo.bytes;
        PMIX_VALUE_RELEASE(get_value);
    }

    if (proc.rank != 0) {
        /* connect to rank 0 */
        addr.sin_family = AF_INET;
        addr.sin_addr = business_cards[0].ip;
        addr.sin_port = business_cards[0].port;
        int rc = connect(fd, (struct sockaddr *)&addr, addrlen);
        if (rc == -1) {
            printf("errno = %d, %s\n", errno, strerror(errno));
            exit(1);
        }

        /* send a hello message */
        rc = snprintf(msgbuf, 128, "hello from rank %d!", proc.rank);
        assert(rc > 0 && rc < 128);
        assert(write(fd, msgbuf, strlen(msgbuf) + 1) == (ssize_t)strlen(msgbuf) + 1);
    } else {
        /* accept connections from peers and read messages */
        for (uint32_t i = 0; i < job_size - 1; i++) {
            int conn_fd = accept(fd, (struct sockaddr *)&addr, &addrlen);
            if (conn_fd == -1) {
                printf("errno = %d, %s\n", errno, strerror(errno));
                exit(1);
            }
            assert(read(conn_fd, msgbuf, 128) > 0);
            printf("got message: %s\n", msgbuf);
            close(conn_fd);
        }
    }

    /* free socket and business cards */
    close(fd);
    free(business_cards);

    /* finalize PMIx */
    pmi_errno = PMIx_Finalize(NULL, 0);
    assert(pmi_errno == PMIX_SUCCESS);

    return 0;
}

static int create_socket(void) {
    char hostname[256];
    int rc;
    int fd;
    struct addrinfo *info;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
    };

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        fprintf(stderr, "unable to get hostname\n");
        exit(1);
    }

    rc = getaddrinfo(hostname, NULL, &hints, &info);
    if (rc != 0) {
        fprintf(stderr, "%s\n", gai_strerror(rc));
        exit(1);
    }

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(fd != -1);

    rc = bind(fd, info->ai_addr, info->ai_addrlen);
    assert(rc == 0);

    freeaddrinfo(info);

    return fd;
}
