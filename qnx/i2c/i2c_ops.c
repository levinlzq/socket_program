
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/slog2.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <hw/i2c.h>

static int init_slog2(void)
{
    extern char *__progname;
    slog2_buffer_t buffer_handle[1];
    slog2_buffer_set_config_t  buffer_config;
    buffer_config.buffer_set_name = __progname;
    buffer_config.num_buffers = 1;
    buffer_config.verbosity_level = SLOG2_INFO;
    buffer_config.buffer_config[0].buffer_name = "i2c_ops";
    buffer_config.buffer_config[0].num_pages = 1;

    /* Register the buffer set. */
    if(slog2_register(&buffer_config, &buffer_handle[0], 0) == -1) {
        fprintf(stderr, "I2C Ops: Error registering slogger2 buffer!\n");
        return -1;
    }
    /* default handle will be the first buffer */
    slog2_set_default_buffer(buffer_handle[0]);
    return 0;
}

static void help_usage(void)
{
    return;
}

int main(int argc, char **argv)
{
    int opt;
    int ret_status;
    int i2c_fd, i2c_addr = 0;
    char i2c_dev[32];
    uint8_t bus_reset = false;
    if (init_slog2()) {
        return 1;
    }

    strcpy(i2c_dev, "/dev/i2c0");

    while ((opt = getopt(argc, argv, "hRd:a:")) != EOF) {
        switch (opt)
        {
            case 'h':
                help_usage();
            break;
            case 'd':
                strncpy(i2c_dev, optarg, sizeof(i2c_dev) - 1);
                slog2f(NULL, 0, SLOG2_INFO, "i2c_dev: %s\n", i2c_dev);
            break;
            case 'a':
                i2c_addr = atoi(optarg);
                slog2f(NULL, 0, SLOG2_INFO, "i2c_addr: %#x\n", i2c_addr);
            break;
            case 'R':
                bus_reset = true;
            break;
            default:
                fprintf(stderr, "Wrong argument \n");
                exit(-1);
        }
    }

    i2c_fd = open(i2c_dev, O_RDWR);

    ret_status = devctl(i2c_fd, DCMD_I2C_BUS_RESET, NULL, 0, NULL);

    slog2f(NULL, 0, SLOG2_INFO, "devctl: Bus Reset Status: %d\n", ret_status);

    close(i2c_fd);

    return 0;
}

