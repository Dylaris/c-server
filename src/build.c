#define ZST_IMPLEMENTATION
#include "zst.h"

#define CC "gcc"

int main(int argc, char **argv)
{
    zst_forger_update_self(CC, argc, argv, "-I", "../");

    zst_cmd_t cmd;
    zst_cmd_init(&cmd);

    zst_cmd_append_arg(&cmd, CC, "-o", "client", "client.c");
    zst_cmd_append_arg(&cmd, "&&");
    zst_cmd_append_arg(&cmd, CC, "-o", "server", "server.c");

    zst_cmd_run(&cmd);

    zst_cmd_free(&cmd);
    return 0;
}
